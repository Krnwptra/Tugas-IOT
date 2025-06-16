#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>  // Versi 6.x

// Ganti dengan data WiFi yang berhasil terkoneksi
const char* ssid = "OPPO A96";
const char* password = "00000000";

// Token dari BotFather (pastikan sudah benar)
const char* botToken = "7506216795:AAGq5HbIYB8_yc6dbZERRHbH-zNxp85v-aE";
const char* telegramApiBase = "https://api.telegram.org/bot";

// Pin relay
const int relay1Pin = D1;
const int relay2Pin = D2;

// Variabel untuk update Telegram
long lastUpdateId = 0;

WiFiClientSecure client;

void setup() {
  Serial.begin(115200);

  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);

  // Matikan relay saat boot (asumsi relay aktif LOW)
  digitalWrite(relay1Pin, HIGH);
  digitalWrite(relay2Pin, HIGH);

  // Koneksi WiFi
  WiFi.begin(ssid, password);
  Serial.print("⏳ Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Terhubung!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Abaikan sertifikat SSL (tidak aman untuk produksi)
  client.setInsecure();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    handleTelegramUpdates();
  } else {
    Serial.println("⚠️ WiFi terputus, mencoba reconnect...");
    WiFi.reconnect();
  }

  delay(1000);  // Cek pesan tiap 1 detik
}

void handleTelegramUpdates() {
  HTTPClient https;

  String url = String(telegramApiBase) + botToken + "/getUpdates?offset=" + String(lastUpdateId + 1);
  https.begin(client, url);
  int httpCode = https.GET();

  if (httpCode > 0) {
    String payload = https.getString();

    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print("❌ JSON error: ");
      Serial.println(error.f_str());
      https.end();
      return;
    }

    JsonArray result = doc["result"].as<JsonArray>();

    for (JsonObject update : result) {
      lastUpdateId = update["update_id"].as<long>();
      String text = update["message"]["text"].as<String>();
      long chat_id = update["message"]["chat"]["id"].as<long>();

      Serial.print("📩 Perintah diterima: ");
      Serial.println(text);

      if (text == "/relay1_on") {
        digitalWrite(relay1Pin, LOW);
        sendMessage(chat_id, "✅ Relay 1 AKTIF");
      } else if (text == "/relay1_off") {
        digitalWrite(relay1Pin, HIGH);
        sendMessage(chat_id, "✅ Relay 1 NONAKTIF");
      } else if (text == "/relay2_on") {
        digitalWrite(relay2Pin, LOW);
        sendMessage(chat_id, "✅ Relay 2 AKTIF");
      } else if (text == "/relay2_off") {
        digitalWrite(relay2Pin, HIGH);
        sendMessage(chat_id, "✅ Relay 2 NONAKTIF");
      } else {
        sendMessage(chat_id,
          "Perintah tidak dikenali.\nGunakan salah satu:\n"
          "/relay1_on\n/relay1_off\n/relay2_on\n/relay2_off");
      }
    }
  } else {
    Serial.print("❌ Gagal akses Telegram: ");
    Serial.println(httpCode);
  }

  https.end();
}

void sendMessage(long chat_id, String text) {
  HTTPClient https;
  String url = String(telegramApiBase) + botToken + "/sendMessage";

  https.begin(client, url);
  https.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String postData = "chat_id=" + String(chat_id) + "&text=" + text;

  int httpCode = https.POST(postData);

  if (httpCode > 0) {
    Serial.println("📨 Balasan terkirim");
  } else {
    Serial.print("❌ Gagal kirim pesan: ");
    Serial.println(httpCode);
  }

  https.end();
}
