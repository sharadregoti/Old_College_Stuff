#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <EEPROM.h>

String deviceID = "1";
String deviceName = "LVDT";
const int eepromFlagAdd = 5;
const byte interruptPin = 13;
boolean Result;
int Mode;
int clientState = 5;
int serverState = 0;
int Case = 0;
int x, y, z;
int  GPIO_CASE = 0, num, control;
String Command, String_num, String_control;
int Condition = 0;
String temp_String = "";
int len_String;
int k = 0, j = 0, l = 0, i;
int add;
String AP_Device_ID , AP_Password, AP_SSID, AP_PortNum, AP_IP;
String temp_Device_ID = "";
String temp_Password = "";
String temp_SSID = "";
String temp_PortNum = "";
String temp_IP = "";
//const char* fingerprint = "AF 4C CF 05 B1 68 C6 12 B6 8F A1 EB B3 A8 08 3D 35 92 82 49";
//const char* host = "sharad";

void eepromWrite();
void sendData();
String readByte(int add);
void writeByte(String a, int add);
void serverMode();
void client1Mode();
int readMsb();
int readLsb();
char* string2char(String command) ;
int flagForServer =  0 ;


IPAddress IP;
WiFiServer server(5000);
WiFiClient client1;
//WiFiClientSecure client2;

char* string2char(String command) {
  if (command.length() != 0)
  {
    char*p = const_cast<char*>(command.c_str());
    return p;
  }
}


void serverMode()
{
  if (serverState == 0)
  {
    WiFi.mode(WIFI_AP);
    Result = WiFi.softAP("HP_envy", "123123123");
    if (Result == true)
    {
      IP = WiFi.softAPIP();
      Serial.println("IP :: ");
      Serial.println(IP);
      server.begin();
      serverState = 1;
    }
  }
  else if (serverState == 1)
  {
    if (!client1.connected()) {
      client1 = server.available();
    }
    else {
      if (client1.available() > 0)
      {
        serverState = 2;
      } else if (client1.connected()) {
        if (flagForServer == 0) {
          client1.println("Enter Device Id");
          Serial.println("Enter Device Id");
          flagForServer = 1 ;
        }
      }
    }
  }
  else if (serverState == 2)
  {
    switch (Case) {
      case 0:
        if (client1.available())
        {
          AP_Device_ID = client1.readStringUntil('\n');
          AP_Device_ID.trim();
          Serial.println(AP_Device_ID);
          Serial.print("Recieived Data Length :: ");
          Serial.println(AP_Device_ID.length());
          Case = 1;
          client1.println("Enter Router SSID");
        }
        break;
      case 1 :
        if (client1.available())
        {
          AP_SSID = client1.readStringUntil('\n');
          AP_SSID.trim();
          Serial.println(AP_SSID);
          Serial.print("Recieived Data Length :: ");
          Serial.println(AP_SSID.length());
          Case = 2;
          client1.println("Enter Router Password");
        }
        break;
      case 2 :
        if (client1.available())
        {
          AP_Password = client1.readStringUntil('\n');
          AP_Password.trim();
          Serial.println(AP_Password);
          Serial.print("Recieived Data Length :: ");
          Serial.println(AP_Password.length());
          Case = 3;
          client1.println("Enter Server IP");
        }
        break;
      case 3 :
        if (client1.available())
        {
          AP_IP = client1.readStringUntil('\n');
          AP_IP.trim();
          Serial.println(AP_IP);
          Serial.print("Recieived Data Length :: ");
          Serial.println(AP_IP.length());
          Case = 4;
          client1.println("Enter Server Port");
        }
        break;
      case 4 :
        if (client1.available())
        {
          AP_PortNum = client1.readStringUntil('\n');
          AP_PortNum.trim();
          Serial.println(AP_PortNum);
          Serial.print("Recieived Data Length :: ");
          Serial.println(AP_PortNum.length());
          Case = 0;
          serverState = 3;
        }
        break;
    }
  }
  else if (serverState == 3)
  {
    eepromWrite();
    serverState = 4;
  }
  else if (serverState == 4)
  {
    client1.stop();
    EEPROM.write(eepromFlagAdd, 5);
    EEPROM.commit();
    Mode = EEPROM.read(5);
    server.close();
    serverState = 5;
  }
}

void client1Mode()
{
  if (clientState == 5)
  {
    temp_Device_ID = readByte(20);
    temp_SSID =   readByte(50);
    temp_Password =  readByte(90);
    temp_IP = readByte(160);
    temp_PortNum = readByte(210);
    clientState = 6;
    Serial.println("clientState = 6");
  }
  else if (clientState == 6)
  {
    int P = temp_PortNum.toInt();
    WiFi.mode(WIFI_STA);
    WiFi.begin(string2char(temp_SSID), string2char(temp_Password) );
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(250);
    }
    Serial.println("WIFI CONnected");

    delay(100);
    while (!client1.connect(string2char(temp_IP), P))
    {
      Serial.println(" conneecting to server");
      delay(500);
    }
    delay(100);
    clientState = 7;
  }
  else if (clientState == 7) {
    if (client1.connect(string2char(temp_IP), temp_PortNum.toInt()))
    {
      Serial.println("connected]");
      Serial.println("[Sending a request]");
      client1.println(deviceID+"," + deviceName+","  + analogRead(A0));
      Serial.println(deviceID + deviceName + analogRead(A0));
      client1.stop();
      Serial.println("\n[Disconnected]");
    }
    else
    {
      Serial.println("connection failed!]");
      client1.stop();
    }
    delay(100);
    ESP.deepSleep(20e6);
  }
}

void eepromWrite() {
  writeByte(AP_Device_ID, 20);
  writeByte(AP_SSID, 50);
  writeByte(AP_Password, 90);
  writeByte(AP_IP, 160);
  writeByte(AP_PortNum, 210);
}

void writeByte(String a, int add) {
  len_String = a.length();
  char Device_ID[len_String + 1];
  a.toCharArray(Device_ID, len_String + 1);
  if (k < 5) {
    EEPROM.write(add, len_String);
    delay(100);
    add++;
    k++;
  }
  for (i = 0; i < len_String; i++ ) {
    EEPROM.write(add, a[i]);
    add++;
    delay(100);
    EEPROM.commit();
  }
}

String readByte(int add) {
  temp_String = "";
  if (j < 5) {
    len_String = EEPROM.read(add);
    delay(100);
    add++;
    j++;
  }
  char DEVICE_ID[len_String + 1];
  for (i = 0; i < len_String; i++) {
    DEVICE_ID[i] = EEPROM.read(add);
    temp_String.concat(DEVICE_ID[i]);
    add++;
    delay(100);
  }
  Serial.println(temp_String);
  return temp_String;
}



void handleInterrupt() {
  EEPROM.write(eepromFlagAdd, 0);
  EEPROM.commit();
  ESP.deepSleep(2e6); // or use restart() but it needs hardware reset after 1st Serial Flashing
}

void setup() {
  WiFi.disconnect();
  client1.stop();
  serverState = 0;
  Case = 0;
  clientState = 5;
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);
  Serial.begin(115200);
  EEPROM.begin(512);
  Serial.println("In Setup Interrup Attached");
  Mode = EEPROM.read(eepromFlagAdd);
  Serial.print("isServerConfigEnabled :: ");
  Serial.println(Mode);
}

void loop() {
  if (Mode == 0) {
    serverMode();
  }
  else {
    client1Mode();
  }
}
