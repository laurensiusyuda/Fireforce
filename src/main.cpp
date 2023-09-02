#include <Arduino.h>

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

// void setup
void setup() {
  Serial.begin(9600);
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
  // Calculate distance in centimeters based on the speed of sound
  float distance = duration * 0.034 / 2;
  return distance;
}

// baca sensor api 
float baca_nilai_api(){
  int fireStatus = digitalRead(readsensorfire);
  return fireStatus;
}

// void loop
void loop() {
  float nilaiADCgas = baca_nilai_adc(readsensorgas);
  float nilaippmgas = baca_nilai_gas(nilaiADCgas);
  float nilaiapi = baca_nilai_api();
  float jarak = baca_jarak();

  // tampilkan pada serial print 
  Serial.println(nilaippmgas);
  Serial.println(nilaiADCgas);
  Serial.println("Deteksi Api: " + String(nilaiapi == HIGH ? "Ya" : "Tidak"));

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
}