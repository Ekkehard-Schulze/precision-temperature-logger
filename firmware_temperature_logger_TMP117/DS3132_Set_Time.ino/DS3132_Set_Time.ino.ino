/* DS_3231 clock time Setting and Checking of the Time
  (c) 2017 University of Freiburg - Faculty of Biology, ATbio, Jürgen Schmidt, ConFoBi Project 2016-0008
  Development environment: Arduino IDE 1.8.19
  Library RTClibExtended.h: https://github.com/FabioCuomo/FabioCuomo-DS3231
  Some code lines are taken from the demo of the RTClibExtended.h library.
  Date and Time are printed every second to the serial port.
*/



#include <Wire.h>
#include <RTClibExtended.h>
RTC_DS3231 RTC;      //we are using the DS3231 RTC

byte AlarmFlag = 0;
byte ledStatus = 1;
#define ledPin 13         //rote LED

void setup() {

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  Serial.begin(57600);

  Wire.begin();
  RTC.begin();
  RTC.adjust(DateTime(__DATE__, __TIME__));   //set RTC date and time to COMPILE time

  // esmod 20250502 by Ekkehard Schulze: 
  // clear set "lost power bit" (EOSC = bit 7 in control register set to 1 )
  // A single battery-under voltage will set this flag. Without re-setting this flag,
  // the clock will STOP in battery-backup mode (but remember the last date-time), 
  // even when supplemented with a fresh battery, but resume to run and advance 
  // the stored time when Vcc is provided again.
  if (RTC.lostPower()) {
  unsigned char b;
  Wire.beginTransmission(0x68); // address DS3231
  Wire.write(0x0E); // select CONTROL register
  b = Wire.read();
  Wire.endTransmission();
  Wire.beginTransmission(0x68); // address DS3231
  Wire.write(0x0E); // select CONTROL register
  Wire.write(b & 0b01111111   ); // write register (bit 7 is EOSC)
  Wire.endTransmission();
  }
  // end esmod                    

  //clear any pending alarms
  RTC.armAlarm(1, false);
  RTC.clearAlarm(1);
  RTC.alarmInterrupt(1, false);
  RTC.armAlarm(2, false);
  RTC.clearAlarm(2);
  RTC.alarmInterrupt(2, false);
  RTC.writeSqwPinMode(DS3231_OFF);
}

//------------------------------------------------------------

void loop() {

  digitalWrite(ledPin, LOW);
  delay(100);
  digitalWrite(ledPin, HIGH);
  delay(900);

  DateTime now = RTC.now();


  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println("");


}

