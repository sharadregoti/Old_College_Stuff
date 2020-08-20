
#define LVDT_ON_OFF_START_POS 250      // eeprom memory starting addres to store the lvdts to initialize out of 8
#define LVDT_50_OR_25_START_POS 270    // eeprom memory staring address to store size of each lvdt accordingley 25 or 50mmm
#define NUMBER_OF_LVDT 8               // number of lvdts
String tempWifiName, tempWifiPass;     // to store wifi name & password from eeprom
int clientState = 0;

// convert string to char *
char* string2char(String command) {
  if (command.length() != 0)
  {
    char*p = const_cast<char*>(command.c_str());
    return p;
  }
}

int client1Mode()
{ Serial.println("client mode");

  switch (clientState) {
    case 0 :                            // read wifi name and pass from eeprom
      tempWifiName = readByte(20);
      tempWifiPass =   readByte(50);
      clientState = 1;
//      buzzer();
      break;

    case 1 :
      {
        WiFi.mode(WIFI_STA);            // nodemcu in station mode
        WiFi.begin(string2char("BMS"), string2char("BMS123123123") );   // connect to wifi router
        while (WiFi.status() != WL_CONNECTED)
        { Serial.print(".");

          delay(250);
        }
        clientState = 2;
        Serial.println("WIFI Connected");

  //      buzzer();
        break;
      }

    case 2 :
    //  buzzer();
      String link = "http://api.thingspeak.com/update?api_key=WTNG8U12URFM9XO9";  // thingspeak channel link for sending data
      for (int i = 0; i < NUMBER_OF_LVDT; i++) {
        if (!EEPROM.read(LVDT_ON_OFF_START_POS + i)) {     // if lvdt is not initalized (value == 0) then skip this iteration
          continue;
        }
        setMuxSwitch(i);  // set mux select line according to i - if i = 5 then select line = 0101
        float a = analogRead(A0);
        delay(100);
        float b = float(EEPROM.read(LVDT_50_OR_25_START_POS + i));
        //float val = ((analogRead(A0) * (float(EEPROM.read(LVDT_50_OR_25_START_POS + i)))) / 1024.000) ; // read from analog pin of nodemu
        float val = (a * b) / 1024;
        delay(100);
        Serial.println(a);
        Serial.println(b);
        Serial.println(val);
        link += "&field" + String(i + 1) + "=" + String(val); // make the required link for thingspeak
      }

      sendDataToThingsSpeak(link); // send data to thingspeak channel
      readTalkBack5(); // for 5 get data from talkback of thingspeak from that set or reset alarm
      readTalkBack6(); // for 6
      clientState = 0;
      return 1;
      break;
  }
  return 0;
}
