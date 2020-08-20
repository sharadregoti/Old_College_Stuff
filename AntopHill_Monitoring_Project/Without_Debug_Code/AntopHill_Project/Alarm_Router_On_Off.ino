

// turn on or off alarm or jioRelay
void turnOn_Off(int pos, int value ) {
  digitalWrite(pos, value);
}

// whatever the current time go for deep sleep every 30th minute or 60th(00) minute of each hour
void dynamicDeepSleep() {
  Serial.println("DYNAMIC SLEEP");

  // first parameter: Time zone in floating point (for India); second parameter: 1 for European summer time; 2 for US daylight saving time; 0 for no DST adjustment; (contributed by viewwer, not tested by me)
  dateTime = NTPch.getNTPtime(5.5, 0);

  // check dateTime.valid before using the returned time
  // Use "setSendInterval" or "setRecvTimeout" if required
  if (dateTime.valid) {
    int dptime = DEEPSLEEP_TIME_IN_MINUTES;
    NTPch.printDateTime(dateTime);
    int rnd = 0, val = 0;
    byte actualMinute = dateTime.minute;
    Serial.println(actualMinute);

    if ((dateTime.hour == 12) && (actualMinute <= 4)) {
      digitalWrite(JIO_RELAY, HIGH);
      delay(2000);
      digitalWrite(JIO_RELAY, LOW);
      delay(60000);
    }
//    rnd = roundUp(actualMinute, dptime);
//    val = abs(rnd - actualMinute);
//    Serial.printf("actual time %d", actualMinute);
//    Serial.printf("rnd %d", rnd);
//    Serial.printf("val %d", val);
//    ESP.deepSleep(val * 60 * 1000000); // deep sleep for 30 min dynamically
  }
  //return -1;

}

int roundUp(int round, int multiple) {
  if (multiple == 0) {
    return round;
  }
  int remainder = 0;
  remainder = round % multiple;
  int val = round + multiple - remainder;
  return val;
}
