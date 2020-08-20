#include <ESP8266WiFi.h>

String apiKey = "FPXL98N2QO5I4JZV"; 
const char* ssid = "Lenovo";    
const char* password = "123123123";  

const char* server = "api.thingspeak.com";
int Sample;

WiFiClient client;

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() 
{
  if (client.connect(server,80))  
  { 
    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String(Sample++);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
  }
  client.stop();
  Serial.println("Waiting…");       
  delay(20000);
}
