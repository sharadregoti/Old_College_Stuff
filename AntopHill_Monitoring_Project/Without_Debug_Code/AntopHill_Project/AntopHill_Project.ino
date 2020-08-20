#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <EEPROM.h>
#include <DoubleResetDetector.h>
#include <NTPtimeESP.h>

#define DRD_TIMEOUT 2
#define DRD_ADDRESS 0
#define eepromFlagAdd 5
DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

#define DEEPSLEEP_TIME_IN_MINUTES 1      // time for deep sleep in minutes Note : cannot be > 40 min 

int Mode;                                 // decides server mode or client mode of operation
// for daylight saving for some countris like america,canada

#define DISABLE_DEEPSLEEP_EEPROM_ADD 300
#define BUZZER D8
#define ALARM_RELAY D6
#define JIO_RELAY D5
#define S0 D4
#define S1 D3
#define S2 D2
#define S3 D1

#define TURN_ON 0
#define TURN_OFF 1
NTPtime NTPch("in.pool.ntp.org");   // Choose server pool as required
strDateTime dateTime;
int buzzerFlag = 0;
int alarmControl5 = 0,alarmControl6 = 0;
void setup() {
  pinMode(ALARM_RELAY, OUTPUT);
  pinMode(JIO_RELAY, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  Serial.begin(115200);
  EEPROM.begin(512);
        digitalWrite(ALARM_RELAY, 1);


//  if (drd.detectDoubleReset()) {          // if true change the flag
//    EEPROM.write(eepromFlagAdd, 0);       // change the flag to 0 to indicate server mode
//    EEPROM.commit();
//    ESP.deepSleep(2e6);                   // turn off everything & go to sleep till the timer giver the interrupt
//    // or use restart() but it needs hardware reset after 1st Serial Flashing
//  }

  Mode = EEPROM.read(eepromFlagAdd);   // read the status of flag to choose the mode of operation(server or client)
}

void loop() {

  drd.loop();
  if (Mode == 0) {
    serverMode();
  }
  else {
    int status =  client1Mode();
    // buzzer();
   // while (EEPROM.read(DISABLE_DEEPSLEEP_EEPROM_ADD) && status) {
      dynamicDeepSleep();
    //}
  }
}
