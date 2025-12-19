/* DS_3231 Uhrzeit stellen und Kontrolle
  (c) 2017 Universität Freiburg - Fakultät für Biologie, ATbio, Juergen Schmidt, ConFoBi Projekt 2016-0008
  Entwicklungsumgebung: Arduino IDE 1.6.11
  Bibliothek RTClibExtended.h: https://github.com/FabioCuomo/FabioCuomo-DS3231
  Einige Programmzeilen stammen aus dem Demo von der RTClibExtended.h Lib


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
  
  // use this for set clock to compile time
  RTC.adjust(DateTime(__DATE__, __TIME__));   //set RTC date and time to COMPILE time

 // un-comment and edit the next line to set a time different to your PC's time, e.g. UTC 
 // RTC.adjust(DateTime(2025, 12, 18, 15, 55, 45)); // insert here:  year, month, day, hour, minute, second
                                                                     


  // esmod 20250502 clear lost power bit (EOSC = bit 7 in control register set to 1 )
  // I guess if you remove Vcc AND the battery, you will reset this bit as well
  // if EOSC is set, the clock will not advance, but stand still, when Vcc is off an Vbatt is on
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

