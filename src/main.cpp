// TODO: Menambahkan fitur agar tidak ada delay dengan memberikan time interupt 
// TODO: Menamnbahkan fitur buzzer on off jika terdeteksi api

#include <Arduino.h>
// lib lcd I2C
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// inisialiasai pin LCD 
LiquidCrystal_I2C lcd(0x27, 20, 4);

// inisialiasai sensor gas, sensor api, sensor ultrasonik, dan buzzer
#define readsensorgas 32
#define readsensorfire 4
#define buzzerPin 5
#define trigPin 2
#define echoPin 3

// kalibrasi sensor gas 
float vin = 3.3;
float vref = 5;
float totalbit = 32;
float rangeADC = 4096;

// menentukan ambang PPM untuk mendetksi LPG 
float ambang_nilai_gas_aman = 200.0;
float ambang_nilai_gas_normal = 1000.0;
float ambang_nilai_gas_berbahaya = 5000.0;

// menentukan ambang batas jarak api
float ambang_jarak_api = 20.0;

//Deklarasi variabel dan flag
bool flagEksekusiLCD = false; // Flag untuk menandakan waktu untuk mengganti halaman
int halaman = 1; // Variabel untuk melacak halaman yang aktif
unsigned long waktuEksekusiLCD = 0; // Variabel untuk melacak waktu terakhir tugas dijalankan
int timecounter = 0; // Variabel untuk menghitung berapa kali tugas dijalankan

// void setup
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
}

//Membaca ADC sensor gas
float baca_nilai_adc(int pin){
  int nilaiADC = analogRead(pin);
  return nilaiADC;
}

//Membaca sensor gas (ADC konversi)
float baca_nilai_gas(int pin){
  int range = analogRead(pin);
  float ppm = (range/totalbit)*(vin/vref)*rangeADC;
  return ppm;
}

//Membaca sensor ultrasonik 
float baca_jarak() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  // mengubah jarak centimeter
  float distance = duration * 0.034 / 2;
  return distance;
}

//Membaca sensor api 
float baca_nilai_api(){
  int fireStatus = digitalRead(readsensorfire);
  return fireStatus;
}

// Fungsi untuk timer interrupt
void timerInterrupt() {
  if (millis() - waktuEksekusiLCD >= 1000) {
    waktuEksekusiLCD = millis(); // Menyimpan waktu saat tugas dijalankan
    flagEksekusiLCD = true; // Mengaktifkan flag untuk menandakan waktu untuk mengganti halaman
    timecounter ++; // Menghitung berapa kali tugas dijalankan
    if (timecounter >= 5)
    {
      timecounter = 0; // Mereset hitungan waktu jika sudah mencapai 5 detik
      halaman ++; // Mengubah halaman
      if (halaman == 3)
      {
        halaman = 1; // Kembali ke halaman pertama jika mencapai halaman ke-3
      }
    }
  }
}

// void loop
void loop() {
  // memanggil fungsi time interupt 
  timerInterrupt();

  // variabel menampilkan nilai adc dan data sensor 
  float nilaiADCgas = baca_nilai_adc(readsensorgas);
  float nilaippmgas = baca_nilai_gas(nilaiADCgas);
  float nilaiapi = baca_nilai_api();
  float jarak = baca_jarak();

  //Output buzzer, Mengendalikan buzzer berdasarkan kondisi sensor
  if (nilaiapi == HIGH && jarak <= ambang_jarak_api && nilaippmgas > ambang_nilai_gas_berbahaya ) {
    // Jika sensor api mendeteksi api, aktifkan buzzer
    digitalWrite(buzzerPin, HIGH);
  } else if (nilaiapi == LOW && nilaippmgas > ambang_nilai_gas_berbahaya) {
    // Jika tidak ada deteksi api, namun konsentrasi gas berbahaya, aktifkan buzzer
    digitalWrite(buzzerPin, HIGH);
  } else {
    // Untuk kondisi lain, matikan buzzer
    digitalWrite(buzzerPin, LOW);
  }
  
  // Tampilkan data pada serial print 
  Serial.println(nilaippmgas);
  Serial.println(nilaiADCgas);
  Serial.println("Deteksi Api: " + String(nilaiapi == HIGH ? "Ya" : "Tidak"));

  // menggunakan fungsi time interupt agar tidak mengganggu proses dalam loop
  if (flagEksekusiLCD == true)
  {
    flagEksekusiLCD = false;
    // tampilkan pada lcd sensor gas (halaman 1)
    if (halaman == 1)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(": ");      
      lcd.print(nilaiADCgas);
      lcd.setCursor(0, 1);
      lcd.print("Nilai PPM Gas: ");      
      lcd.print(nilaippmgas);
      // Kondisi untuk mendeteksi konsentrasi gas LPG
      if (nilaippmgas < ambang_nilai_gas_aman) {
        lcd.print("Konsentrasi Gas Aman");} 
        else if (nilaippmgas < ambang_nilai_gas_normal)  {
        lcd.print("Konsentrasi Gas Normal");} 
        else if (nilaippmgas < ambang_nilai_gas_berbahaya){
          lcd.print("Konsentrasi Gas Berpotensi Bahaya");}
          else {lcd.print("Konsentrasi Gas Berbahaya");}
    }
    // tampilkan pada lcd sensor api dan sensor ultrasonik (halaman 2)
    else if (halaman == 2)
    {
      if (nilaiapi == HIGH)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Nilai Api : 1 (Terdeteksi)");      
      }else
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Nilai Api : 0 (Tdk Terdeteksi)");      
      }
      // membuat kondisi sensor api dengan sensor ultrasonik untuk mendetksi jarak api
      if (jarak < ambang_jarak_api && nilaiapi == HIGH) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Api Terdeteksi Dalam Jangkuan");
        lcd.print(jarak);
        lcd.print(" cm");
        } else if (nilaiapi == HIGH) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Api Terdeteksi Di Luar Jangkuan");
          } else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("No Fire Detected");}
      }
  }
}