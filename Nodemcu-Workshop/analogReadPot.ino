#include <ESP8266WiFi.h>

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.print("ADC Value : ");
  Serial.println((float(analogRead(A0) * 10)) / 1024);
  delay(150);
}