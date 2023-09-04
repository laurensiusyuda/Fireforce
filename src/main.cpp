// TODO: Menambahkan fitur agar tidak ada delay dengan memberikan time interupt 

#include <Arduino.h>
// lib lcd I2C
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// inisialiasai pin LCD 
LiquidCrystal_I2C lcd(0x27, 16, 2);

// inisialiasai sensor gas, sensor api, dan sensor ultrasonik
#define readsensorgas 32
#define readsensorfire 4
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

// membuat fungsi milis
bool flagEksekusiLCD = false;
int halaman = 1;
unsigned long waktuEksekusiLCD = 0;
int timecounter = 0;

// void setup
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

//Read ADC sensor gas
float baca_nilai_adc(int pin){
  int nilaiADC = analogRead(pin);
  return nilaiADC;
}

// Read sensor gas (ADC konversi)
float baca_nilai_gas(int pin){
  int range = analogRead(pin);
  float ppm = (range/totalbit)*(vin/vref)*rangeADC;
  return ppm;
}

// baca sensor ultrasonik 
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

// baca sensor api 
float baca_nilai_api(){
  int fireStatus = digitalRead(readsensorfire);
  return fireStatus;
}

// Fungsi untuk timer interrupt
void timerInterrupt() {
  if (millis() - waktuEksekusiLCD >= 1000) {
    waktuEksekusiLCD = millis();
    flagEksekusiLCD = true;
    timecounter ++;

    if (timecounter >= 5)
    {
      timecounter = 0;
      halaman ++;
      if (halaman == 4)
      {
        halaman = 1;
      }
    }
  }
  
}


// void loop
void loop() {
  // variabel menampilkan nilai adc dan data sensor 
  float nilaiADCgas = baca_nilai_adc(readsensorgas);
  float nilaippmgas = baca_nilai_gas(nilaiADCgas);
  float nilaiapi = baca_nilai_api();
  float jarak = baca_jarak();

  // tampilkan pada serial print 
  Serial.println(nilaippmgas);
  Serial.println(nilaiADCgas);
  Serial.println("Deteksi Api: " + String(nilaiapi == HIGH ? "Ya" : "Tidak"));

  // tampilkan pada lcd sensor gas (halaman 1)
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ADC Gas: ");      
  lcd.print(nilaiADCgas);
  lcd.setCursor(0, 1);
  lcd.print("Nilai PPM Gas: ");      
  lcd.print(nilaippmgas);
  delay(1000);

  // tampilkan pada lcd sensor api dan sensor ultrasonik (halaman 2)
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ADC Gas: ");      
  lcd.print(nilaiADCgas);
  lcd.setCursor(0, 1);
  lcd.print("Nilai PPM Gas: ");      
  lcd.print(nilaippmgas);
  delay(1000);

  // membuat pengkondisian
  // Kondisi untuk mendeteksi konsentrasi gas LPG
  if (nilaippmgas < ambang_nilai_gas_aman) {
    Serial.println("Konsentrasi Gas Aman");
  } else if (nilaippmgas < ambang_nilai_gas_normal) {
    Serial.println("Konsentrasi Gas Normal");
  } else if (nilaippmgas < ambang_nilai_gas_berbahaya) {
    Serial.println("Konsentrasi Gas Berpotensi Bahaya");
  } else {
    Serial.println("Konsentrasi Gas Berbahaya");
  }

  // membuat kondisi sensor api dengan sensor ultrasonik untuk mendetksi jarak api
  if (jarak < ambang_jarak_api && nilaiapi == HIGH) {
    Serial.println("Fire Detected at Close Range!");
    // Add actions or alarms for fire detection at close range here.
  } else if (nilaiapi == HIGH) {
    Serial.println("Fire Detected!");
    // Add actions or alarms for fire detection here.
  } else {
    Serial.println("No Fire Detected");
  }
  delay(1000); // Delay for 1 second before checking again
}




/*
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

!Inisialisasi objek LiquidCrystal_I2C dengan alamat I2C yang sesuai
LiquidCrystal_I2C lcd(0x27, 16, 2); // Sesuaikan dengan alamat LCD I2C Anda

unsigned long previousMillis = 0;
const unsigned long interval = 5000; // Interval perubahan data pada LCD dalam milidetik (5 detik)
int currentPage = 1; // Menggunakan halaman 1 awalnya

void setup() {
  !Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Page 1: Data 1");

  !Inisialisasi pin ADC (misalnya, A0)
  pinMode(A0, INPUT);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    !Setiap 5 detik, ganti data pada halaman tertentu
    if (currentPage == 1) {
      lcd.setCursor(0, 1);
      lcd.print("       "); // Hapus data sebelumnya
      lcd.setCursor(0, 1);
      lcd.print("Data 2");

    !Pindah ke halaman berikutnya setelah mengganti data
      currentPage = 2;
    } else if (currentPage == 2) {
      lcd.setCursor(0, 1);
      lcd.print("       "); // Hapus data sebelumnya
      lcd.setCursor(0, 1);
      lcd.print("Data 3");

    !Kembali ke halaman pertama setelah mengganti data pada halaman terakhir
      currentPage = 1;
    }
  }

  !Kode di loop utama dapat berjalan tanpa delay yang mempengaruhi tampilan LCD
}

*/