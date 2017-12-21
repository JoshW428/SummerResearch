/*--------------------------------------
    file:parkinson RMband using mini
    Author: Rick Martin
    Date: 5/6/2017
    Vibrates coin motors at a variable rate.

  ----------------------------------------*/

//#include <LiquidCrystal.h>    // include the library code:
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  // initialize the library with the numbers of the interface pins


const int modePin = 9;           // set up a constant for the mode switchPin
const int motorPin1 = 6;           // set up a constant for the first group of motors
const int motorPin2 = 7;           // set up a constant for the second group of motors
const int motorPin3 = 8;           // set up a constant for the third group of motors
const int ratePin = A0;           // set up a constant for the tilt switchPin
const int dutyCyclePin = A1;      // set up a constant for the tilt switchPin
const int minRate = 25;           // minimum pulse rate in pulses per second
const int maxRate = 160;          // maximum pulse rate in pulses per second
const int minDC = 20;             // minimum duty cycle in %
const int maxDC = 80;             // maximum duty cycle in %
char ESC = 0xFE;
int backlight = 4;                // backlight level for LCD (1 to 8)
int contrast = 40;                //contrast 1 to 50
long period;                    // interval at which to track time (milliseconds)
int mode = 0;                    // variable to hold the value of the vibration on/off status
int motorState = 0;
int buttonLock = 0;               // variable to software debounce the switch
int rate = 50;                    // a variable to holde the vibration cycles per second
int rateInput;
int dutyCycle = 50;
int dutyCycleInput;
int buttonState;
int buttonDefault;
int msOn;
int msOff;
char* modeStrings[] = {"Mode: Off   ", "Mode: On 1   ", "Mode: On 2   ", "Mode: On 3   ", "Mode: All On   "};
//char modeStr[13] = "Mode: Off   ";
unsigned long previousMicros = 0;        // will store last time motor was updated

void setup() {
  pinMode(modePin, INPUT);        // set up the mode switch pin as an input
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  digitalWrite(motorPin1, LOW);    // turn off vibration motor initially
  digitalWrite(motorPin2, LOW);    // turn off vibration motor initially
  digitalWrite(motorPin3, LOW);    // turn off vibration motor initially
  // Initialize LCD module
  Serial.begin(9600);
  Serial.write(ESC);
  Serial.write(0x41); //x41 = on, x42 = off
  Serial.write(ESC);
  Serial.write(0x51);//clear screen
  // Set Contrast
  Serial.write(ESC);
  Serial.write(0x52);
  Serial.write(contrast);//set contrast (1 to 50)
  // Set Backlight
  Serial.write(ESC);
  Serial.write(0x53);
  Serial.write(backlight); //set backlight (1 to 8)
  // Set cursor line 1, column 0
  Serial.write(ESC);
  Serial.write(0x45);//set cursor position
  Serial.write(0x40);//move to beginning of line 2
  Serial.print(modeStrings[0]);
  // Set cursor line 2, column 0
  Serial.write(ESC);
  Serial.write(0x45);//set cursor position
  Serial.write(0x0);//move to beginning of line 2
  Serial.print("Fr:50Hz;DC:56%");
  Serial.write(ESC);
  Serial.write(0x48);//turn off underline cursor
  Serial.write(ESC);
  Serial.write(0x46);//home cursor
  int buttonDefault = digitalRead(modePin);//set default button value
}

void loop() {
  //buttonDefault = digitalRead(modePin);
  buttonState = digitalRead(modePin);  // read the button
  if ((buttonState!=buttonDefault) && (!buttonLock)) {    // check the status of the switch
    mode = (mode + 1) % 5;              // cycle thru the modes
    // Set cursor line 1, column 0
    Serial.write(ESC);
    Serial.write(0x45);//set cursor position
    Serial.write(0x40);//move to beginning of line 2
    Serial.print(modeStrings[mode]);
    // Set cursor line 2, column 0
    Serial.write(ESC);
    Serial.write(0x45);//set cursor position
    Serial.write(0x0);//move to beginning of line 2
    Serial.print("Fr:");
    Serial.print(rate);
    Serial.print("Hz; DC:");
    Serial.print(dutyCycle);
    Serial.print("% ");
    delay(200);
    buttonLock = 1;
    buttonDefault = buttonState;
  }
  else if (buttonState & buttonLock) {
    delay(200);
    buttonLock = 0;
  }
  period = int(500000 / rate);
  if (mode) {
    unsigned long currentMicros = micros();
    if (motorState & (currentMicros - previousMicros >= msOn)) {
      previousMicros = currentMicros;
      motorState = LOW;
      digitalWrite(motorPin1, motorState);
      digitalWrite(motorPin2, motorState);
      digitalWrite(motorPin3, motorState);
    }
    else if (!motorState & (currentMicros - previousMicros >= msOff)) {
      previousMicros = currentMicros;
      motorState = HIGH;
      switch (mode) {
        case 0: break;
        case 1: digitalWrite(motorPin1, motorState); break;
        case 2: digitalWrite(motorPin2, motorState); break;
        case 3: digitalWrite(motorPin3, motorState); break;
        case 4: digitalWrite(motorPin1, motorState);
          digitalWrite(motorPin2, motorState);
          digitalWrite(motorPin3, motorState); break;
      }
    }
  }
  else {
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);    // turn the vibration motor off
    // Set cursor line 1, column 0
    Serial.write(ESC);
    Serial.write(0x45);//set cursor position
    Serial.write(0x40);//move to beginning of line 2
    Serial.print(modeStrings[mode]);
    // Set cursor line 2, column 0
    Serial.write(ESC);
    Serial.write(0x45);//set cursor position
    Serial.write(0x0);//move to beginning of line 2
    Serial.print("Fr:");
    Serial.print(rate);
    Serial.print("Hz; DC:");
    Serial.print(dutyCycle);
    Serial.print("%");
    Serial.write(ESC);
    Serial.write(0x48);//turn off underline cursor
    Serial.write(ESC);
    Serial.write(0x46);//home cursor

    //lcd.setCursor(0, 0);
    //lcd.print("Rate: off");         // Print a message to the LCD.
    //lcd.setCursor(0, 1);      // set the cursor to column 0, line 1; line 1 is the second row, since counting begins with 0
    // lcd.print("Duty Cycle: off");   // print to the second line
//    lcd.clear();
//    lcd.setCursor(0, 0);
//    lcd.print(modeStrings[mode]);         // Print a message to the LCD.
//    lcd.setCursor(0, 1);            // set the cursor to column 0, line 1; line 1 is the second row, since counting begins with 0
//    lcd.print("Rate: ");            // print to the second line
//    lcd.print (rate);
//    lcd.print(",DC: ");
//    lcd.print(dutyCycle);
//    lcd.print("%");
    delay(200);
  }

  rateInput = analogRead(ratePin);
  rate = map(rateInput, 0, 1023, minRate, maxRate);
  dutyCycleInput = analogRead(dutyCyclePin);
  dutyCycle = map(dutyCycleInput, 0, 1023, minDC, maxDC);
  //dutyCycle = 50;
  msOn = int((1000000 / rate) * dutyCycle / 100.0); //int((600/rate)*0.5);
  msOff = int((1000000 / rate) - msOn);
  //Serial.print("period = ");
  //Serial.println(period);
  //Serial.print("rateInput = ");
  //Serial.println(rate);
  //Serial.print("     DC = ");
  //Serial.print(dutyCycle);
  //Serial.print("%  msOn = ");
  //Serial.print(msOn);
  //Serial.print("  msOff = ");
  //Serial.println(msOff);


}
/*
  NKC Electronics 16x2 Serial LCD test

  16x2 Serial LCD pinout for UART serial communication
  pin 1: RX
  pin 2: GND
  pin 3: +5V

  connect pin1 to Arduino digital pin 1
  connect pin2 to Arduino GND
  connect pin3 to Arduino +5V

  Default communication speed is 9600
*/


