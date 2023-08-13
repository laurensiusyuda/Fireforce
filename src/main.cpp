#include <Arduino.h>

// inisialiasai sensor gas 
#define readsensorgas 32

// void setup
void setup() {

}

//Read ADC
float baca_nilai_adc(int pin){
  int nilaiADC = analogRead(pin);
  return nilaiADC;
}

//ADC Konversi Menuju PPM ESP 32
float baca_nilai_gas(int pin){
  int nilaiADC = analogRead(pin);
  int range = 1000;
  float ppm = (range/32)*(3.3/5)*4096;
  return ppm;
}

// void loop
void loop() {
  int nilaiADCgas = baca_nilai_adc(readsensorgas);
}