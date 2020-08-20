#include <ESP8266WiFi.h>

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  int time = millis() / 1000;
  Serial.print("I was reset ");
  Serial.print(time);
  Serial.println("seconds ago.");
  Serial.println("Can u press the reset Button.");
  delay(1000);
}
