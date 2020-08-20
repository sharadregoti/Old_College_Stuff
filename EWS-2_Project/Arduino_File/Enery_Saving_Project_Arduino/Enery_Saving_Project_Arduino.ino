/* Working :
    1] the value of change mode is checked, if 1 =  switch 2 temperature sensor, if = 0 switch to ldr sensor
       for changing mode a spdt swich is used
    2] then check the motion sensor if it is high or motion counter is > 0 get the reading of sensor and compare it with the limit set
       if crosses that limit turn on the ac load & start the timer
    3] timer value is decided by interval variable, till the timer is on keep the ac load in on state 
       and increment the motion sensor counter if there is some motion detected, by motion sensor while the timer is running
    4] go to step 2 & check motion sensor or motion counter 

    simultaneously ir recciever is also checked for and data
    if data = to turn on load 
    NOW MOTION SENSOR OR ANY SENSOR WILL NOT WORK, THE LOAD WILL REMAIN ON TILL 
    IT IS MANUALLY TURNED OFF OR USING IR CODE = TO TURN OFF
    if data = to turn off load
    now all the sensor will work

    Load is connected to NC terminal of relay module  

*/



#define DEBUGLEVEL 1

#include <DebugUtils.h>
#include <IRremote.h>
#include "DHT.h"

#define READ_PIR_SENSOR 2     // pir sensor connected to pin 2
#define RELAY_STATE 3
#define OUTPUT_RELAY 3        // relay to drive ac load connected to pin 3
#define LDR_SENSOR A0         // ldr connected to A0
#define CHANGE_MODE 4         // 1 = Temperature Mode & 0 = LDR-Sensor Mode 
#define IR_PIN 7              // ir reciver connected to pin 7
#define TEMPERATURE_SENSOR 5  // temperature sensor connected to pin 5


IRrecv irrecv(IR_PIN);
decode_results results;

DHT dht;

long previousMillis = 0;
long interval = 10000; // timer value
long pirMotionDetectedCounter, ldrAnalogValue;
bool isTimerOn, isIrState;
float temperatureLimit = 26.0; // set the temperature limit, if the temperature is above this limit
// ac load will be triggered
int ldrAnalogValueLimit = 350; // set the ldr limit, if the light intensity is below this limit
// ac load will be triggered

void setup() {
  pinMode(READ_PIR_SENSOR, INPUT);
  pinMode(OUTPUT_RELAY, OUTPUT);
  pinMode(CHANGE_MODE, INPUT);
  Serial.begin(9600);
  digitalWrite(OUTPUT_RELAY, LOW);
  irrecv.enableIRIn();
  irrecv.blink13(true);

  dht.setup(TEMPERATURE_SENSOR);
}

void loop() {
  readIR();                                                   // read ir to see if anything is availabe from ir reciver
  if (!isIrState) {

    if (isTimerOn) {
      unsigned long currentMillis = millis();
      DEBUGPRINTLN2("Timer is On");

      if ((currentMillis - previousMillis) > interval) {
        previousMillis = currentMillis;
        isTimerOn = false;
        DEBUGPRINTLN1("Timer has ELAPSED");
      }
      else {
        long val = currentMillis - previousMillis;
        DEBUGPRINTLN2(val);

        if (digitalRead(READ_PIR_SENSOR)) {
          pirMotionDetectedCounter++;
          DEBUGPRINT2("pirMotionDetectedCounter Value : ");
          DEBUGPRINTLN2(pirMotionDetectedCounter);
        }
      }
    }
    else {
      if (digitalRead(CHANGE_MODE)) {// TEMPERATUR MODE
        if (digitalRead(READ_PIR_SENSOR) || (pirMotionDetectedCounter > 0)) {

          delay(dht.getMinimumSamplingPeriod());
          float temperature = dht.getTemperature();

          DEBUGPRINTLN0("Detected Motion");
          DEBUGPRINTLN1(temperature);

          if (temperature >= temperatureLimit) {
            turnOnRelay();
            isTimerOn = true;
            DEBUGPRINTLN1("Turn On Relay");
            pirMotionDetectedCounter = 0;
          }
        }
        else if (!digitalRead(READ_PIR_SENSOR) && (pirMotionDetectedCounter == 0)) {
          turnOffRelay();
          DEBUGPRINTLN2("Turning Off Relay");
        }
      }
      else {                                                // LDR MODE
        if (digitalRead(READ_PIR_SENSOR) || (pirMotionDetectedCounter > 0)) {
          ldrAnalogValue = analogRead(LDR_SENSOR);

          DEBUGPRINTLN0("Detected Motion");
          DEBUGPRINTLN1(ldrAnalogValue);

          if (ldrAnalogValue < ldrAnalogValueLimit) {
            turnOnRelay();
            isTimerOn = true;
            DEBUGPRINTLN1("Turn On Relay");
            pirMotionDetectedCounter = 0;
          }
        }
        else if (!digitalRead(READ_PIR_SENSOR) && (pirMotionDetectedCounter == 0)) {
          turnOffRelay();
          DEBUGPRINTLN2("Turning Off Relay");
        }
      }
    }
  }
}

// turnOnRelay check the state of relay if it is low, then turns on the relay
void turnOnRelay() {
  if (!digitalRead(RELAY_STATE)) {
    digitalWrite(OUTPUT_RELAY, HIGH);
    delay(50);
  }
}

// turnOffRelay check the state of relay if it is high, then turns off the relay
void turnOffRelay() {
  if (digitalRead(RELAY_STATE)) {
    digitalWrite(OUTPUT_RELAY, LOW);
    delay(50);
  }
}

// readIr get the ir code from ir reciver if signal available
// then takes appropiate actions according to the code
void readIR() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    irrecv.resume();
  }
  if ((results.value == 2160050494) && isIrState) {
    DEBUGPRINTLN1("Off Button Pressed");
    turnOffRelay();
    isIrState = false;
  }
  else if ((results.value == 2160024484) && !isIrState) {
    DEBUGPRINTLN1("On Button Pressed");
    turnOnRelay();
    isIrState = true;
  }
}
