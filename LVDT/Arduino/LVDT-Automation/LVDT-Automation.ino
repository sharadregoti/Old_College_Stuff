l #include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DELIM "."

volatile byte isServerConfigEnabled = 0;// Flag whose status shows if it need to enter in the server configuration Mode
const byte interruptPin = 13;
const int eepromFlagAdd = 0;

const char * accesspointSSID = "Wifi101";
const char * accesspointpassword = "123456789";
WiFiServer server(80);

IPAddress local_IP(192, 168, 4, 22);
IPAddress gateway(192, 168, 4, 9);
IPAddress subnet(255, 255, 255, 0);

const int eepromDataAdd[]  = {10, 30, 50, 70};
String deviceID = "1";
String deviceName = "LVDT";

WiFiClient client;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);
  Serial.println("In Setup Interrup Attached");
  isServerConfigEnabled = EEPROM.read(eepromFlagAdd);
  Serial.print("isServerConfigEnabled :: ");
  Serial.println(isServerConfigEnabled);
  if (isServerConfigEnabled == 1) {
    if (configureServer()) {
      Serial.println("Server Configured Successfully :)");
      Serial.println("Restaring");//restart todo
      isServerConfigEnabled = 0;
      EEPROM.write(eepromFlagAdd, isServerConfigEnabled);
      EEPROM.commit();
      ESP.deepSleep(2e6);
    } else {
      Serial.println("Server Configuration Failed :(");
      Serial.println("Trying To configure server again ");
    }
  } else {
    clientMode();
  }
}

void loop() {

}

void handleInterrupt() {
  isServerConfigEnabled = 1;
  EEPROM.write(eepromFlagAdd, isServerConfigEnabled);
  EEPROM.commit();
  ESP.deepSleep(2e6); // or use restart() but it needs hardware reset after 1st Serial Flashing
}

boolean configureServer() {
  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  Serial.print("Setting Up Acces Point..... ");
  Serial.println(WiFi.softAP(accesspointSSID, accesspointpassword) ? "Acces Point Ready" : "Acces Point Failed!");
  server.begin();
  Serial.println("Access Point Server Started");
  return configureServerData();
}

boolean configureServerData() {
  int count = 0;
  boolean result = false , inverterFlag = true;
  while (1) {
    client = server.available();
    if (client) {
      Serial.println("Client connected");
      while (client.connected()) {
//        if ( count == 0 && inverterFlag) {
//          Serial.print("Sending Info :: ");
//          Serial.println(count);
//          clientt.println("Enter IP Address :: ");
//          inverterFlag = false;
//        } else if (count == 1 && inverterFlag) {
//          Serial.print("Sending Info :: ");
//          Serial.println(count);
//          clientt.println("Enter PORT Number :: ");
//          inverterFlag = false;
//        } else if (count == 2 && inverterFlag) {
//          Serial.print("Sending Info :: ");
//          Serial.println(count);
//          clientt.println("Enter WIFI Name :: ");
//          inverterFlag = false;
//        } else if (count == 3 && inverterFlag) {
//          Serial.print("Sending Info :: ");
//          Serial.println(count);
//          clientt.println("Enter WIFI Password :: ");
//          inverterFlag = false;
//        } else if (count > 3) {
//          Serial.print("Sending Info :: ");
//          Serial.println(count);
//          Serial.println("Thanks");
//          clientt.println("Thank You :)");
//          result = true;
//          break;
//        }
        if (client.available())
        {
          String line = client.readStringUntil('\r');
          Serial.println("Received Data From Client :: " + line);
          Serial.print("Recieived Data Length :: ");
          Serial.println(line.length());
          // wait for end of client's request, that is marked with an empty line
          if (line.length() == 1 && line[0] == '\n')
          {
            Serial.println("Null String Break Client");
            client.stop();
            break;
          }
//          if (verifyData(count, line)) {
//            Serial.println("verify count :: " + String(count) + "true");
//            writeDataEEPROM(count, line);
//            count++;
//            inverterFlag = true;
//            Serial.println("Count Incremented Inverting Flag True");
//            Serial.print("Count :: ");
//            Serial.println(count);
//          } else {
//            Serial.println("Wrong Data");
//            clientt.println("Wrong Data");
//          }
        }
      }
      client.stop();
      Serial.println("[Client disonnected]");
      Serial.print("Returning result of Sever config data :: ");
      Serial.println(result);
      break;
    }

  }
  return result;
}



void writeDataEEPROM(int addIndex, String data) {
  Serial.println("Writing Data in EEPROM");
  int i, count = 0;
  EEPROM.write(eepromDataAdd[addIndex], data.length());
  Serial.println("Writing length of data in eeprom :: " + String(data.length()) + "Location :: " + String(eepromDataAdd[addIndex]) + "index :: " +  String(addIndex));
  for (i = eepromDataAdd[addIndex] + 1; i <= data.length() + eepromDataAdd[addIndex]; i++ ) {
    EEPROM.write(i, data[count]);
    delay(10);
    Serial.println("Writing Data in eeprom :: len ::" + String(data.length()) + "Location :: " + String(i) + "count, String index :: " + String(count));
    count++;
  }
  EEPROM.commit();
  Serial.println("Exit write eeprom");
}

boolean verifyData(int count, String data) {
  Serial.println("inside verify data");

  switch (count) {
    case 0 : return verifyIP(data);
      break;
    case 1 : return verifyPort(data);
      break;
    case 2 : return verifyString(data);
      break;
    case 3 : return verifyString(data);
      break;
  }
  return false;
}

boolean verifyIP(String data) {
  Serial.println("inside verify ip");
  char * tab2 = new char [data.length() + 1];
  strcpy (tab2, data.c_str());
  return is_valid_ip(tab2);
}
boolean verifyPort(String data) {
  Serial.println("inside verify port");

  int value = data.toInt();
  if (value > 1024 && value < 65635) {
    return true;
  }
  return false;
}
boolean verifyString(String data) {
  Serial.println("inside verify string");

  int len;
  len = data.length();
  if (len > 0 && len < 20) {
    return true;
  }
  return false;
}


void clientMode() {
  Serial.println("inside client mode");

  String sSSID , sPass, sIP, sPort; // starting s stands for string
  int i;
  sSSID = readeeprom(0);
  sPass = readeeprom(1);
  sIP = readeeprom(2);
  sPort = readeeprom(3);
  Serial.println(sSSID);
  Serial.println(sPass);
  Serial.println(sIP);
  Serial.println(sPort);

  Serial.println("Connecting to " +  sSSID);
  WiFi.begin(convertToCharStar(sSSID), convertToCharStar(sPass));
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
  WiFiClient clientMode;

  Serial.println("[Connecting to  ... " +  sIP);
  if (clientMode.connect(convertToCharStar(sIP), sPort.toInt()))
  {
    Serial.println("connected]");

    Serial.println("[Sending a request]");
    clientMode.println(deviceID + deviceName + analogRead(A0));
    Serial.println(deviceID + deviceName + analogRead(A0));

    clientMode.stop();
    Serial.println("\n[Disconnected]");
  }
  else
  {
    Serial.println("connection failed!]");
    clientMode.stop();
  }
  delay(100);
  ESP.deepSleep(20e6);
}

String readeeprom(int arrayAddIndex) {
  Serial.println("inside read eeprom");

  int i;
  String sData = "";
  byte len, data;
  len = EEPROM.read(eepromDataAdd[arrayAddIndex]);
  Serial.println("length of first location :: " + String(len));

  for (i = eepromDataAdd[arrayAddIndex] + 1; i <= len + eepromDataAdd[arrayAddIndex]; i++ ) {
    data = EEPROM.read(i);
    delay(10);
    sData += char(data);
    Serial.println("read eeprom data :: " + String(data));
    Serial.println("read eeprom string data :: " + sData);
  }
  Serial.println(sData);
  return sData;
}

const char * convertToCharStar(String data) {
  return data.c_str();
}

/* return 1 if string contain only digits, else return 0 */
boolean valid_digit(char *ip_str)
{
  while (*ip_str) {
    if (*ip_str >= '0' && *ip_str <= '9')
      ++ip_str;
    else
      return false;
  }
  return true;
}

/* return 1 if IP string is valid, else return 0 */
boolean is_valid_ip(char *ip_str)
{
  int i, num, dots = 0;
  char *ptr;

  if (ip_str == NULL)
    return false;

  // See following link for strtok()
  // http://pubs.opengroup.org/onlinepubs/009695399/functions/strtok_r.html
  ptr = strtok(ip_str, DELIM);

  if (ptr == NULL)
    return false;

  while (ptr) {

    /* after parsing string, it must contain only digits */
    if (!valid_digit(ptr))
      return false;

    num = atoi(ptr);

    /* check for valid IP */
    if (num >= 0 && num <= 255) {
      /* parse remaining string */
      ptr = strtok(NULL, DELIM);
      if (ptr != NULL)
        ++dots;
    } else
      return false;
  }

  /* valid IP string must contain 3 dots */
  if (dots != 3)
    return false;
  return true;
}
