#include <ESP8266HTTPClient.h>

// selects 1 input channel from 16 channels of 16 * 1 Multiplexer
void setMuxSwitch(int val) {
  int arr[] = {2, 0, 4, 5}; // s0,s1,s2,s3
  Serial.print("Mux Switch Pos :: ");

  int temp = val;
  for (int i = 0; i < (sizeof(arr) / sizeof(arr[0])); i++) {
    val = ((val >> i) & 1);
    Serial.printf(" %d ", val);

    digitalWrite(arr[i], val);
    val = temp;
  }
  delay(100);
}

// sends the data to thingspeak according to the link string specified in the params
void sendDataToThingsSpeak(String link) {
  Serial.println(link);

  HTTPClient http;                      //Declare object of class HTTPClient
  http.begin(link);                     //Specify request destination
  int httpCode = http.GET();            //Send the request
  String payload = http.getString();    //Get the response payload
  if (httpCode == HTTP_CODE_OK) {
    Serial.println(payload);

    //Print request response payload
  } else {
    Serial.println(httpCode);

    //ESP.deepSleep(1e6);
  }
  http.end();    //Close connection
  delay(15000);
}

// sends get request to talkback thingspeak app to get the status & set alarm accordingly
void readTalkBack5() {
  String link = "http://api.thingspeak.com/talkbacks/33334/commands/15955687.json?api_key=BVW3YDSFZ6V16Q4P";
  HTTPClient http;                     //Declare object of class HTTPClient
  http.begin(link);                    //Specify request destination
  int httpCode = http.GET();           //Send the request
  String payload = http.getString();   //Get the response payload

  if (httpCode == HTTP_CODE_OK) {
    Serial.println(payload);           //Print request response payload

    StaticJsonDocument <100> doc;
    deserializeJson(doc, payload);
    const char* command_string_value = doc["command_string"];   // get the value of command_string either 0 or 1
    Serial.println(command_string_value);

    if (command_string_value[0]  == '1') {                      // if true turn on alarm
      //turnOn_Off(ALARM_RELAY, TURN_ON); // turn on alarm
      alarmControl5 = 1;
      digitalWrite(ALARM_RELAY, 0);

      EEPROM.write(DISABLE_DEEPSLEEP_EEPROM_ADD, 0); // diable deep sleep as alarm is on
    } else {
      //turnOn_Off(ALARM_RELAY, TURN_OFF); // turn off alarm
      alarmControl5 = 0;
      if (!alarmControl6) {
        digitalWrite(ALARM_RELAY, 1);
      }

      EEPROM.write(DISABLE_DEEPSLEEP_EEPROM_ADD, 1); // diable deep sleep as alarm is on
    }
  }
  else {
    Serial.println(httpCode);

    ESP.deepSleep(1e6);//Print HTTP return code
  }
  http.end();                         //Close connection
  // todo : deep sleep
}

// sends get request to talkback thingspeak app to get the status & set alarm accordingly
void readTalkBack6() {
  String link = "http://api.thingspeak.com/talkbacks/33334/commands/16070909.json?api_key=BVW3YDSFZ6V16Q4P";
  HTTPClient http;                     //Declare object of class HTTPClient
  http.begin(link);                    //Specify request destination
  int httpCode = http.GET();           //Send the request
  String payload = http.getString();   //Get the response payload

  if (httpCode == HTTP_CODE_OK) {
    Serial.println(payload);           //Print request response payload

    StaticJsonDocument <100> doc;
    deserializeJson(doc, payload);
    const char* command_string_value = doc["command_string"];   // get the value of command_string either 0 or 1
    Serial.println(command_string_value);

    if (command_string_value[0]  == '1') {                      // if true turn on alarm
      //turnOn_Off(ALARM_RELAY, TURN_ON); // turn on alarm
      alarmControl6 = 1;
      digitalWrite(ALARM_RELAY, 0);

      EEPROM.write(DISABLE_DEEPSLEEP_EEPROM_ADD, 0); // diable deep sleep as alarm is on
    } else {
      //turnOn_Off(ALARM_RELAY, TURN_OFF); // turn off alarm
      alarmControl6 = 0;
      if (!alarmControl5) {
        digitalWrite(ALARM_RELAY, 1);
      }

      EEPROM.write(DISABLE_DEEPSLEEP_EEPROM_ADD, 1); // diable deep sleep as alarm is on
    }
  }
  else {
    Serial.println(httpCode);

    ESP.deepSleep(1e6);//Print HTTP return code
  }
  http.end();                         //Close connection
  // todo : deep sleep
}
// read deta stored in eeprom byte by byte and return string
String readByte(int add) {
  String temp_String = "";
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

  return temp_String;
}
