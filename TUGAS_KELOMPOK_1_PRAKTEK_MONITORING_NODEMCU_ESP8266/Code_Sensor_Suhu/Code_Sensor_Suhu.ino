//                                                         //
//       PRAKTIKUM IOT MONITORING SUHU DAN KELEMBAPAN      //
//    TEKNOLOGI INFORMASI - FAKULTAS SAINS DAN TEKNOLOGI   //
//                  UIN WALLISONGO SEMARANG                //
//
//
          // KELOMPOK: 6                        //
          // ANGGOTA:                           //
          //      1. Jesika Natalia             //
          //      2. Kurniawan Putra M.         //
          //      3. M. Nabil Al-Hamamy         // 
//

#include <DHT.h>                    // Library untuk sensor DHT11
#include <ESP8266WiFi.h>             // Library untuk koneksi WiFi menggunakan ESP8266
#include <ThingSpeak.h>              // Library untuk berkomunikasi dengan ThingSpeak
#include <Wire.h>                    // Library I2C untuk komunikasi dengan LCD
#include <LiquidCrystal_I2C.h>       // Library untuk mengendalikan LCD dengan I2C

// GANTI SESUAI DENGAN JARINGAN WIFI
const char* ssid = "Aaa kasian aa";      // Ganti dengan SSID WiFi Anda
const char* password = "hehehehe";   // Ganti dengan password WiFi Anda

// DHT sensor
#define DHTPIN 14   // GPIO14 = D5 pada NodeMCU
DHT dht(DHTPIN, DHT11);               // Inisialisasi sensor DHT11 pada pin D5

// ThingSpeak
unsigned long myChannelNumber = 2930089;  // Ganti dengan Channel ID ThingSpeak Anda
const char* myWriteAPIKey = "LHKKQXXUIP2CEOMC";  // Ganti dengan API Key ThingSpeak Anda

WiFiClient client;   // Client WiFi untuk menghubungkan ke ThingSpeak

// Inisialisasi LCD 16x2 dengan I2C (alamat default 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Alamat default 0x27, sesuaikan jika perlu

void setup() {
  Serial.begin(9600);  // Mulai komunikasi serial pada baud rate 9600
  delay(10);

  // Inisialisasi DHT sensor
  dht.begin();  // Mulai sensor DHT11 untuk membaca suhu dan kelembapan

  // Inisialisasi LCD
  lcd.init();       // Menggunakan init() untuk inisialisasi LCD
  lcd.backlight();  // Mengaktifkan lampu latar LCD
  
  // Menampilkan pesan pembuka pada LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MONITORING SUHU");
  lcd.setCursor(0, 1);
  lcd.print("DAN KELEMBAPAN");
  delay(3000);  

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("KELOMPOK 6");
  lcd.setCursor(0, 1);
  lcd.print("IOT SK");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("JESIKA, KURNIAWAN");
  lcd.setCursor(0, 1);
  lcd.print("dan NABIL");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Prodi TI");
  lcd.setCursor(0, 1);
  lcd.print("UIN Walisongo");
  delay(2000);

  // Koneksi ke WiFi
  WiFi.begin(ssid, password);  // Koneksi ke jaringan WiFi
  Serial.println();
  Serial.println("Menghubungkan ke WiFi...");

    // Menampilkan pesan di LCD saat menghubungkan ke WiFi
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Menghubungkan");
  lcd.setCursor(0, 1);
  lcd.print("ke Wifi.........");

  while (WiFi.status() != WL_CONNECTED) {  // Tunggu hingga WiFi terhubung
    delay(500);
    Serial.print(".");
  }

  // Jika WiFi terhubung, tampilkan pesan di LCD
  if (WiFi.status() == WL_CONNECTED) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Terhubung");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());  // Tampilkan IP lokal setelah terhubung
    Serial.println("");
    Serial.println("WiFi Terhubung");
    delay(3000);  // Menunggu beberapa detik untuk melihat status
  }

  // Jika gagal terhubung ke WiFi, tampilkan pesan di LCD
  else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Gagal Terhubung");
    lcd.setCursor(0, 1);
    lcd.print("Ke WiFi");
    delay(2000);
  }


  // Inisialisasi ThingSpeak
  ThingSpeak.begin(client);  // Inisialisasi ThingSpeak dengan client WiFi
}

void loop() {
  // Membaca suhu dan kelembapan dari sensor DHT11
  float kelembapan = dht.readHumidity();  // Membaca kelembapan
  float suhu = dht.readTemperature();     // Membaca suhu

  // Mengecek apakah pembacaan sensor berhasil
  if (isnan(kelembapan) || isnan(suhu)) {  // Jika pembacaan gagal
    Serial.println("Sensor DHT Error dan Tidak Terdeteksi!");
    return;
  }

  // Menampilkan data suhu dan kelembapan di serial monitor
  Serial.print("Suhu: ");
  Serial.print(suhu);
  Serial.print(" °C  Kelembapan: ");
  Serial.print(kelembapan);
  Serial.println(" %");

  // Menampilkan suhu dan kelembapan di LCD
  lcd.clear();  // Membersihkan layar LCD
  lcd.setCursor(0, 0);  // Set cursor pada baris pertama
  lcd.print("Suhu: ");
  lcd.print(suhu);
  lcd.print(" *C");

  lcd.setCursor(0, 1);  // Set cursor pada baris kedua
  lcd.print("Humidity: ");
  lcd.print(kelembapan);
  lcd.print("%");

  // Mengirim data ke ThingSpeak
  ThingSpeak.setField(1, suhu);  // Field 1 untuk suhu
  ThingSpeak.setField(2, kelembapan);  // Field 2 untuk kelembapan

  // Kirim data ke ThingSpeak
  int responseCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);  // Kirim data ke ThingSpeak

  if (responseCode == 200) {  // Cek apakah pengiriman berhasil
    Serial.println("Berhasil Kirim Data ke ThingSpeak!");
  } else {  // Jika gagal, tampilkan response code
    Serial.println("Data Gagal Terkirim ke ThingSpeak. Response code: " + String(responseCode));
  }

  // Menunggu 20 detik sebelum mengirim data berikutnya
  delay(20000);  // Delay 20 detik
}
