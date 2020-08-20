#include <ESP8266WiFi.h>

const char *ssid = "sharad";
const char *pass = "aINl5jy9";

const char *host = "10.42.0.156";
const int port = 8000;

int flag = 1;

WiFiClient client;

void setup()
{
  Serial.begin(115200);
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(500);
}

void loop()
{
  if (flag == 1)
  {
    if (client.connect(host, port))
    {
      Serial.println("Connected to server");
      flag = 0;
    }
  } else {
    if (client.available())
    {
      String dataFromServer = client.readStringUntil('\r');
      Serial.println(dataFromServer);
    }
    if (client.connected())
    {
      client.println((float(analogRead(A0) * 10)) / 1024);
    }
    else
    {
      Serial.println("No server available disconnecting");
      client.stop();
      flag = 1;
    }
  }
  delay(50);
}
