#include "ESP8266WiFi.h"
 
const char* ssid = "sharad";
const char* password =  "aINl5jy9";
 
WiFiServer wifiServer(80);
WiFiClient client;

void setup() {
 
  Serial.begin(115200);
 
  delay(10);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.println("..");
  }
 
  Serial.print("Connected to WiFi. IP:");
  Serial.println(WiFi.localIP());
 
  wifiServer.begin();
}

void loop() {
   // if client not avialbe the get a client
    if (!client.connected()) {
      client = wifiServer.available();
    }
    else {
      // if client available
      if (client.available() > 0)
      {
          String data = client.readStringUntil('\r');
          Serial.println(data);
      } else if (client.connected()) {
          client.println((float(analogRead(A0)*10))/1024);
      }
    }
}