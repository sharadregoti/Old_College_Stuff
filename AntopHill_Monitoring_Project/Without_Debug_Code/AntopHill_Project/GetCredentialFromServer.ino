WiFiClient client1;
IPAddress IP;
WiFiServer server(5000);

String apWifiName, apWifiPass, apLvdt, apLvdtSize;  // store the data fromt tcp terminal app
String serverWifiName = "Nanded";                    // wifi hotspot name
int serverState = 0;
int flagForServer =  0 ;
int k = 0, j = 0, l = 0, i;
int Case = 0; int len_String;

void serverMode()
{
  switch (serverState) {
    //
    case 0 :
      WiFi.mode(WIFI_AP);             // nodemcu in acces point mode
      if (WiFi.softAP(serverWifiName))  // turn on hotspot with no passworrd
      {
        IP = WiFi.softAPIP();         // get ip
        server.begin();
        serverState = 1;
      }
      break;
    //
    case 1 :
      //buzzer();
      if (!client1.connected()) {
        client1 = server.available(); // get a client
      }
      else {
        if (client1.available() > 0)  // if client is sendig some data > 0
        {
          serverState = 2;
        } else if (client1.connected()) {
          if (flagForServer == 0) {
            client1.println("Enter Wifi Name");  // send to tcp terminal app
            flagForServer = 1 ;
          }
        }
      }
      break;
    //
    case 2 :
      switch (Case) {
        case 0:
          if (client1.available())
          {
            apWifiName = client1.readStringUntil('\n');    // get wifi name from tcp app
            apWifiName.trim();
            Case = 1;
            client1.println("Enter Wifi Pass");
          }
          break;
        case 1 :
          if (client1.available())
          {
            apWifiPass = client1.readStringUntil('\n');     // get wifi pass from tcp app
            apWifiPass.trim();
            Case = 2;
            client1.println("Enter Lvdt(comma seperated)");
          }
          break;
        case 2 :
          if (client1.available())
          {
            apLvdt = client1.readStringUntil('\n');         // get lvdt to on from tcp app
            apLvdt.trim();
            Case = 3;
            client1.println("Enter Lvdt size(comma seperated)");
          }
          break;
        case 3 :
          if (client1.available())
          {
            apLvdtSize = client1.readStringUntil('\n');     // get lvdt size from tcp app
            apLvdtSize.trim();
            Case = 4;
            serverState = 3;
          }
          break;
      }
      break;
    //
    case 3 :
      eepromWrite();
      client1.stop();
      EEPROM.write(eepromFlagAdd, 5); // change flag to indicate now enter in client mode
      EEPROM.commit();
      Mode = EEPROM.read(eepromFlagAdd);  // get mode to indiacate now to go to client mode
      server.close();
      serverState = 4;
  }
}

void eepromWrite() {
  writeByte(apWifiName, 20);                          // write wifi name to eeprom starting add. 20
  writeByte(apWifiPass, 50);                         // write wifi pass to eeprom starting add.
  writeLvdtOnOffString(apLvdt, apLvdtSize);          // write lvdt to turn on & their size to eeprom
}

void writeByte(String a, int add) {
  len_String = a.length();
  char Device_ID[len_String + 1];
  a.toCharArray(Device_ID, len_String + 1);
  if (k < 5) {
    EEPROM.write(add, len_String);
    delay(20);
    add++;
    k++;
  }
  for (i = 0; i < len_String; i++ ) {
    EEPROM.write(add, a[i]);
    add++;
    delay(20);
    EEPROM.commit();
  }
}

void writeLvdtOnOffString(String onOff, String sizem) {
  String temp, tempSize;
  int commas = 0;
  sizem += ",";
  for (int i = 0; i < onOff.length(); i++) { // get comma seperated string store it into temp
    if (onOff[i] == ',') {
      commas++; // store no. of commas in onOff
      continue;
    }
    temp += onOff[i]; // if onOff = 1,2,3 the temp == 123 & comma = 2
  }
  int count = 0;   // array length is 1 + commas
  int arr[8] = {0};
  for (int i = 0; i <= sizem.length(); i++) {   // store size in array
    if (sizem[i] == ',') {
      arr[count] = tempSize.toInt();
      tempSize = "";
      count++;
      continue;
    }
    tempSize += sizem[i];  // if sizem == 25,50,50 then tempSize = [25,50,50]
  }

  for (int i = 0; i < NUMBER_OF_LVDT; i++) { // make everything  0 of lvdt on off & lvdt size in eeprom
    EEPROM.write(LVDT_ON_OFF_START_POS + i, 0);
    EEPROM.write(LVDT_50_OR_25_START_POS + i, 0);
  }


  for (int i = 0; i < temp.length(); i++) {
    EEPROM.write(LVDT_ON_OFF_START_POS + (temp[i] - '0') - 1, 1);// make 1 for every lvdt in temp string
    delay(50);
    EEPROM.write(LVDT_50_OR_25_START_POS + (temp[i] - '0') - 1, arr[i]); // put 25 or 50 according to indicate their size
    delay(50);
    EEPROM.commit();

  }
}
