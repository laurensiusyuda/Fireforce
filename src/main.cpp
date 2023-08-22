#include <Arduino.h>

// inisialiasai sensor gas  dan sensor api 
#define readsensorgas 32
#define readsensorfire 4

// kalibrasi sensor gas 
float vin = 3.3;
float vref = 5;
float totalbit = 32;
float rangeADC = 4096;


// void setup
void setup() {
  Serial.begin(9600);
}

//Read ADC sensor gas dan sensor api
float baca_nilai_adc(int pin){
  int nilaiADC = analogRead(pin);
  return nilaiADC;
}

// ADC Konversi ppm
float baca_nilai_gas(int pin){
  int range = analogRead(pin);
  float ppm = (range/totalbit)*(vin/vref)*rangeADC;
  return ppm;
}

// read sensor api
float baca_nilai_api(){
  int fireStatus = digitalRead(readsensorfire);
  return fireStatus;
}

// void loop
void loop() {
  float nilaiADCgas = baca_nilai_adc(readsensorgas);
  float nilaippmgas = baca_nilai_gas(nilaiADCgas);
  float nilaiapi = baca_nilai_api();
  
  // tampilkan pada serial print 
  Serial.println(nilaippmgas);
  Serial.println(nilaiADCgas);
  // membuat kondisi dimana jika membaca adanya gas dan api 
}