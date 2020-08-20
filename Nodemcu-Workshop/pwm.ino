uint8_t LEDpin = D6;

/* By default PWM frequency is 1000Hz and we are using same 
   for this application hence no need to set */

void setup(){
  Serial.begin(9600);
  analogWrite(LEDpin, 512);  /* set initial 50% duty cycle */
}

void loop(){
  uint16_t dutycycle =  analogRead(A0); /* read continuous POT and set PWM duty cycle according */
  if(dutycycle > 1023) dutycycle = 1023;/* limit dutycycle to 1023 if POT read cross it */
  Serial.print("Duty Cycle: ");  Serial.println(dutycycle);
  analogWrite(LEDpin, dutycycle);
  delay(100);
}
