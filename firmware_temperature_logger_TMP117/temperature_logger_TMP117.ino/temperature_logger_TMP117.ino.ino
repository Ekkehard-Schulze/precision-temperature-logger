/*
  Precision temperature logger using up to four Texas Instruments sensors TMP117 with 
  Adafruit Feather 32u4 Adalogger and DS3231 real time clock 

  (c) 2017 Universität Freiburg - Fakultät für Biologie, 
  ATbio, Juergen Schmidt, ConFoBi, Projekts 2016-0008 and 2020-0007
  code for TMP117 sensor and logging interval setting by file "settings.txt" by Ekkehard Schulze

  20251201 code for date time format changed by Ekkehard Schulze to ISO standard

  compiled using Arduino IDE 1.8.19
  ---------------------------------
  
  Board Adafruit Feather 32u4 (default)

  alternative board
  -----------------
  Sparkfun Openlog (activate #define option down below in this code)

  
  https://www.arduino.cc/en/software/
  
  https://downloads.arduino.cc/arduino-1.8.19-windows.zip
  
  https://downloads.arduino.cc/arduino-1.8.19-windows.exe
  
  https://downloads.arduino.cc/arduino-1.8.19-linux64.tar
  
  https://downloads.arduino.cc/arduino-1.8.19-linuxaarch64.tar.xz
  
  https://downloads.arduino.cc/arduino-1.8.19-macosx.zip
  
  libraries reqired in Arduino IDE:
  -------------------
  https://github.com/arduino-libraries
  libraries\LowPower            source: https://github.com/arduino-libraries/ArduinoLowPower
  libraries\RTClibExtended      source: https://github.com/FabioCuomo/FabioCuomo-DS3231

  Wake-up line from RTC pin 3 to D0 (pin 3 or INT2) of the 32u4 Feather Logger

  Some program lines are taken from the demo of the RTClibExtended.h library
  
  The SD-card is formatted in FAT32.

  TMP117 sensor configuration
  --------------------------------------
  up to four TMP117 can be used on the I2C-bus
  each sensor must be configured (wired) for its own individual I2C bus address
  four different addresses are availabe: 0x48, 0x49, 0x4A, and 0x4B
  these generate column names TMP1, TMP2, TMP3, TMP4
  sesnors 0x4A and 0x4B may be placed on the main board (PCB).
  The sensors are auto-detected, whenever the logger is switched on.

  ------------
  Program Flow
  -------------
  If you power the logger up, it starts and checks the battery. 
  If the green LED flashes three times, the battery voltage is above 
  the threshold defined in "mv_Batt_thresh_RedFlash_on_Startup" and you are good to go.
  Three red flashes indicate that it is below the threshold and charging is recommended.
  
  Next the SD card is initialized. If this fails, the green LED is turned on permanently,
  and the logger will not record any data! Only when all LEDs are non-lit the
  logger is in normal mode and waiting for the next wake-up event in order to log then 
  the next data frame.
  
  Next for each recognized sensor a single green blink is issued.
  1 green blink: 1 sensor, 2 green blinks: 2 sensors, 3 green blinks: 3 sensors, 4 green blinks: 4 sensors,

  Remove and insert the SD card only, if the logger is switched off.
  Check SD-card occasionally for file-system corruption (e.g. every 
  10th use) using your PC.

  The variable "LoggerName" is used to identify the logger in the data file.
  Enter the logger name suffixed with the logger's number her below in the code,
  where "LoggerName" is defined. The default used is TMP117_logger_1". 
  Mark your device with logger-name and number.
  
  The name and number of the logger is saved with each data record. 
  If SD cards are swapped, this ensures the data record can still be assigned to the
  correct logger/location. When transferring the sketch to a new device, be sure to adjust the logger 
  name and number.

  The measurement interval is entered in seconds, and 3600 should be divisible by this interval.
  The table separator can be adjusted using the variable "Separator". The default is "\t" (TAB).
  The "battery off threshold" is defined in the variable "OFF_mV_threshold". The default is 3500 mV.
  Below this threshold the logger stops recording.  


  ---------
  Data Set
  ---------
  At every program start or reset, the software version, gain, and resolution are written to the SD card.
  When logging, a table is generated:
  Logger-id, Vbatt, Date_time, TMP1, TMP2, TMP3, TMP4
  Vbatt is the LiIon battery voltage in mV.
  The table separator can be changed in user settings as the "Separator" variable.
  The logger does not delete data. New data is always appended to the end of the data file (FILENAME).


  ----
  LEDs
  ----
  The green LED remains lit if there is an initialization problem with the SD card.
  The green LED flashes green 3 times at startup if the battery voltage is > 4V, otherwise red 3 times.
  The red LED illuminates during measurement and writing.
  Measurements stop when the voltage drops below 3.7V. This threshold is adjustable in the `get_measurements_and_write_to_SD_card` routine.
  The cutoff threshold is specified in mV. If the battery voltage falls below this threshold, no further measurements are performed.


  -------
  Time
  --------
  
  The time is set using a program named "DS3132_Set_Time.ino.ino".
  Before uploading the program to the Featherlogger, check the PC's time. The RTC is synchronized with the PC's time.
  Depending on the upload time, the time on the logger will then deviate by approximately -7 seconds. The time can be checked using the Serial Monitor.
  Afterward, the logger sketch must be re-uploaded. (temperature_logger_TMP117.ino.ino)
  Be sure to adjust the logger name again to you needs.


  ------------------------------------------
  Power consumption with Transcent SD-card:
  -----------------------------------------

  SD write 20ms 60mA
  Sleep 0.35mA
  -> with a 300-second cycle -> 0.383mA on average -> With a 4400mAh battery, 478 days
*/

#define VERSION "20"
#define SUBVERSION "1"

// Attention: select board in Arduino-IDE and additionally select board in the board selector section below in this code
//            ------------------             ===============   (pre-selected default is Adafruit Feather 32u4 Adalogger)

   
//--S-t-a-r-t--o-f-----U-s-e-r---S-e-t-t-i-n-g-s----------<<<<<<<<<<<<<<<<<<<<<<<<<<<<< USER

int Intervall_seconds = 300;   // Measurement interval in seconds. 3600 (= 1 hour) must be an integer divisible by the interval! 
                       // e.g., 1, 2, 3, 4, 5, 6, 10, 12, 15, 20, 30, 300, 600 seconds
                       // the interval can be overruled by config file "settings.txt" from the SD card

String Separator = "\t";                              // .tsv .csv table separator char

#define FILENAME "temp_log.tsv" 

#define read_config_file  // read config file "settings.txt" (UNIX(LF) line end) from SD card. Relevant line:interval=300 . 
                          // Comment lines in "settings.txt"  start with / (single slash)

#define serial_verbose // serial print datastring


//#define  CLOCK_TIME_IS_UTC    // un-comment this is for iso 8601 UTC date-time style e.g. '2025-12-16T15:19:01Z'

//#define  APPEND_UTC_OFFSET_TO_ISO8601_DATE  // un-comment this is for iso 8601 UTC date-time with UTC offset, e.g. '2025-12-16T15:19:01+09:00'
//#define  UTC_OFFSET_APPENDIX  +09:00       // un-comment this is for iso 8601 UTC date-time with UTC offset, e.g. '2025-12-16T15:19:01+09:00'

//------- Board selection, you must just un-comment one single board, default is ADAFRUIT_FEATHER_LOGGER -----------------

#define ADAFRUIT_FEATHER_LOGGER
//#define OPENLOG

//----Start of --o-f---Board Definitions------------------------------------------------------------

#ifdef ADAFRUIT_FEATHER_LOGGER
#warning "Adafruit Feather Logger m32u4 selected"
#define mv_Batt_thresh_RedFlash_on_Startup 3750      // optische Warnung beim Einschalten : 3 x rot blinken
String LoggerName = "TMP117_logger_1";
#define OFF_mV_threshold 3500  // this is for LiION
#define wakePin 0         //use interrupt 2 (pin 0) and run function wakeUp when pin 0 gets LOW
#define ledRedPin 13         //red LED
#define ledGreenPin 8         //green LED
const int chipSelect = 4; // CS SD Karte
#define LEDredOn    digitalWrite(ledRedPin, HIGH);
#define LEDgreenOn  digitalWrite(ledGreenPin, HIGH);
#define LEDredOff    digitalWrite(ledRedPin, LOW);
#define LEDgreenOff  digitalWrite(ledGreenPin, LOW);
int BatteriePin = 9;
#if defined __AVR_ATmega32U4__
#else
    #error "Please select Adafruit Feather logger m32u4 in IDE."
#endif
#define GOTO_SLEEP LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);           //arduino enters sleep mode here
#include <LowPower.h>
#endif



#ifdef OPENLOG
#warning "Openlog m328p selected"
#define mv_Batt_thresh_RedFlash_on_Startup 3730      // optische Warnung beim Einschalten : 3 x rot blinken
String LoggerName = "Openlog_1";
#define wakePin 2         //use interrupt 0 (INT0, port PD2) and run function wakeUp when pin gets LOW. rem: A3 did NOT work
#define ledRedPin  9        //actully blue LED in original design, rewire PCB! 13->9, originally pin 13, which is SPI clk, not good for low power
#define ledGreenPin 5         //green LED
OneWire  ds(4);  //  PortPin für 1Wire BUS 1Wire-BUS, a 4.7K resistor to Vcc is necessary
const int chipSelect = 10; // CS SD Karte
#define LEDredOn    digitalWrite(ledRedPin, HIGH);
#define LEDgreenOn  digitalWrite(ledGreenPin, HIGH);
#define LEDredOff    digitalWrite(ledRedPin, LOW);
#define LEDgreenOff  digitalWrite(ledGreenPin, LOW);
int BatteriePin = 15;
#define OFF_mV_threshold 0   // this is for NiMH battery, use 3500 for LiPO or LiION
#define GOTO_SLEEP LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);           //arduino enters sleep mode here
#if defined __AVR_ATmega328P__
#else
    #error "Please select a m328p board in IDE."
#endif
#include <LowPower.h>
#endif

//----End of --o-f---Board Definitions

// ----- help mode for debugging can be un-commented if required for further development -----------
// #define es_debug_mode
// #define RedLedOn_for_measurement
// #define RedLedOn_for_low_power
// #define no_Auto_power_off_on_bat_low

//--E-n-d----o-f-----U-s-e-r---S-e-t-t-i-n-g-s----------<<<<<<<<<<<<<<<<<<<<<<<<<<<<< USER

#include <Wire.h>
#include <RTClibExtended.h>
#include <SPI.h>          // Required for SD card
#include <SD.h>           // SD Lib


RTC_DS3231 RTC;           //we are using the DS3231 RTC

int Sekunde = 0;           // variable for alarm time calculation
int Minute = 0;           // variable for alarm time calculation



// ----------------------- TMP117 functions start ---------------------------
// Hexadecimal addresses for various TMP116 registers 
const int Temp_Reg = 0x00;           // Temperature register
const int Config_Reg = 0x01;         // Configuration register

char TMP117_present[] = { 0, 0, 0, 0 }; // boolean for presence of sensor

/*********************** Read Temperature Sensor Function **************************/
/*
 *  https://training.ti.com/how-interface-tmp116-tmp117-temperature-sensors-arduino 
 *  .ino code discussed in video at 11:15
 *  https://training.ti.com/node/1136406
 *  TMP117_Example.ino
 *  Developed by Megan Anderson, THS Applications Engineering Intern
 *  May 2018
 */

void StartReadTempSensorTMP117(int TMP117_Addressl){   // separate to allow to start for sensors in parallel and then to wait for conversion
  // Points to device & begins transmission
  Wire.beginTransmission(TMP117_Addressl); 
  // Points to temperature register to read/write data
  Wire.write(Temp_Reg); 
  // Ends data transfer and transmits data from register
  Wire.endTransmission(); 
  // Delay to allow sufficient conversion time, is done after the calling loop
  delay(10);
}


double ReadTempSensorTMP117(int TMP117_Addressl){
  // Data array to store 2-bytes from I2C line
  uint8_t data[2]; 
  // Combination of 2-byte data into 16-bit data
  int16_t datac;   

  // Requests 2-byte temperature data from device
  Wire.requestFrom(TMP117_Addressl,2); 

  // Checks if data received matches the requested 2-bytes
  if(Wire.available() <= 2){  
    // Stores each byte of data from temperature register
    data[0] = Wire.read(); 
    data[1] = Wire.read(); 

    // Combines data to make 16-bit binary number
    datac = ((data[0] << 8) | data[1]); 

    // Convert to Celcius (7.8125 mC resolution) and return
    return datac*0.0078125; 
  }
}

// ----------------------- TMP117 functions end ---------------------------


/**************************** I2C Write Function ********************************/
double I2Cwrite(int dev, int reg, int H, int L){
  // Takes in 4 variables:
  // device address, register addres
  // high and low bytes of data to transmit 
  Wire.beginTransmission(dev); 
  Wire.write(reg);
  Wire.write(H);
  Wire.write(L);
  Wire.endTransmission();
  delay(10);
}







#ifdef read_config_file 
//----------------------------------------------------------------------------
// https://jurgen.gaeremyn.be/hardware-en/33-arduino/18-reading-configuration-file-from-an-sd-card

#include <ctype.h>

struct parameters {
  int interval;
} settings;


// Setting come from a SD-card file "settings.txt"

void getSettings()
{
File myFile;    
 // Open the settings file for reading:
  myFile = SD.open("settings.txt");
  char character;
  String description = "";
  String value = "";
  boolean valid = true;
    // read from the file until there's nothing else in it:
    settings.interval = Intervall_seconds; // default
    while (myFile.available()) {
      character = myFile.read();
             if(character == '/')         {
               // Comment - ignore this line
               while(character != '\n'){
                 character = myFile.read();
               };
      } else if(isalnum(character))      {  // Add a character to the description
        description.concat(character);
      } else if(character =='=')         {  // start checking the value for possible results
      // First going to trim out all trailing white spaces
      do {
        character = myFile.read();
      } while(character == ' ');
        if(description == "interval") {
          value = "";
          while(character != '\n') {
            if(isdigit(character)) {
              value.concat(character);
            } else if(character != '\n') {
              // Use of invalid values
              valid = false;
            }
            character = myFile.read();            
          };
          if (valid) { 
            // Convert string to array of chars
            char charBuf[value.length()+1];
            value.toCharArray(charBuf,value.length()+1);
            // Convert chars to integer
            settings.interval = atoi(charBuf);
          } else {
            // revert to default value for invalid entry in settings
            settings.interval = Intervall_seconds;  // default
          }
        } 

            else { // unknown parameter
          while(character != '\n')
          character = myFile.read();
        }
        description = "";
      } else {
        // Ignore this character (could be space, tab, newline, carriage return or something else)
      }
   
    }
    // close the file:
    myFile.close();
    Intervall_seconds= settings.interval;   // set global variable
}
//-----------------------------------------------------------
#endif


void dateTime(uint16_t* date, uint16_t* time) {
  DateTime now = RTC.now();

  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(now.year(), now.month(), now.day());

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
}



//-----s-t-a-r-t---o-f---s-e-t-u-p-----------------------------------------
void setup() {
  int sensorcount = 0;   
  #if defined (es_debug_mode) || defined (serial_verbose)
  Serial.begin(9600);
  #endif  

  #if defined (serial_verbose)
  Serial.println("Start in Serial Verbose mode"); 
  #endif 
  #ifdef es_debug_mode
  Serial.println("es_debug_mode is on");
  #endif  
  pinMode(wakePin, INPUT_PULLUP);               //Set pin D0 as INPUT for accepting the interrupt signal
                                                // from DS3231 + Interner Pullup setzen
  pinMode(ledRedPin, OUTPUT);                      //rote LED
  LEDredOff
  pinMode(ledGreenPin, OUTPUT);                     //grüne LED
  LEDgreenOff

  batCheck();                                   // Checks if the battery is sufficently charged. Above threshold, it flashes green; below flashes red.
  
    // SD Check                   Warning: If this is moved to the end of setup, no data will be logged.
  if (!SD.begin(chipSelect)) {
    LEDgreenOn
    return;
  }
  delay(1300);
  
#ifdef read_config_file  
  getSettings(); // Reads settings.txt and extracts the interval, which is the logging interval in seconds. 
                 // If that doesn't work, the compiled default from the beginning of this code applies
#endif

  SdFile::dateTimeCallback(dateTime); // for right time stamp on file

  String headerstr="Logger-id";
    headerstr+=Separator+"Vbatt";
    headerstr+=Separator+"Date_time";

  RTC.begin();
  RTC.armAlarm(1, false);                       //clear any pending alarms
  RTC.clearAlarm(1);
  RTC.alarmInterrupt(1, false);
  RTC.armAlarm(2, false);
  RTC.clearAlarm(2);
  RTC.alarmInterrupt(2, false);
  RTC.writeSqwPinMode(DS3231_OFF);              //Set SQW pin to OFF
  setWakeupTime();




// TMP117: Test if up to 4 TMP117 sensors are installed

for(int i = 0; i<4; i++)
{
Wire.beginTransmission(0x48+i); // 0x48 is first adress of sensor
  if (Wire.endTransmission() == 0) {
    I2Cwrite(0x48+i, Config_Reg,0b00000110, 0b01100000);  // MOD=0b01 ; Conv = 0b100 ;  AVG0 = 0b11      This  means: shutdown; 1 second conversion,  64 averaged conversions      
    TMP117_present[i]= true;
    headerstr+=Separator+"TMP"+ String (i+1); sensorcount++;
    // Serial.println("TMP117 found at 0x48");
  }
}


diagBlinkGreen(sensorcount, 700);



  File dataFile = SD.open(FILENAME, FILE_WRITE);
  if (dataFile) {
    dataFile.println(headerstr);
    dataFile.close();
  }
  
#ifdef serial_verbose
Serial.println(headerstr);
#endif

  delay (250);  
}
//---e-n-d---of---s-e-t-u-p----------------------------------------------




void loop()
{
  go2sleep();                                  // go to sleep 
  get_measurements_and_write_to_SD_card();         // call 
  setWakeupTime();                             // set wakeup time
}
//--e-n-d---o-f---l-o-o-p-----------------------------------------------


//-------------------------------------------------
void wakeUp()                                   // here the interrupt is handled after wakeup <--- place no furhter code here
{
}
//-------------------------------------------------
void setWakeupTime() {
  // Calculate and set the next wake-up time to the next full n seconds.
  DateTime now = RTC.now();
  Sekunde = (((now.second()+(60*now.minute())) / Intervall_seconds) * Intervall_seconds) + Intervall_seconds;
  if (Sekunde >= 3600) Sekunde = 0;
  Minute = Sekunde/60; Sekunde = Sekunde % 60;
  // setAlarm(ALARM_TYPES_t alarmType, byte seconds, byte minutes, byte hours, byte daydate) ALM1_MATCH_MINUTES matches minutes and seconds
  RTC.setAlarm(ALM1_MATCH_MINUTES, Sekunde, Minute, 0, 0);
  RTC.alarmInterrupt(1, true);
}


//--s-t-a-r-t---o-f-- get_measurements_and_write_to_SD_card ------------------------------------------------
void get_measurements_and_write_to_SD_card() {
  // Capture data string and write to SD card

    if (VBat() >= OFF_mV_threshold) {                                  //Cutoff threshold: Stop measurement when battery is too low; value is in board definitions
#ifdef RedLedOn_for_measurement
    LEDredOn                          // Turn on the LED as a debug aid - for demo use only!!
#endif
    String secString = "";
    String minString = "";
    String hourString = "";
    String dayString = "";
    String monthString = "";    
    

    String dataString = LoggerName;                      // Write logger name to identify the data record
    DateTime now = RTC.now();                            // Read current time and format data string

    dataString += Separator + String(VBat());    //  Battery voltage

    if (now.month() < 10) { // left pad with leading zero
      monthString = "0" + String(now.month());
    } else {
      monthString = String(now.month());
    }

    if (now.day() < 10) { // left pad with leading zero
      dayString = "0" + String(now.day());
    } else {
      dayString = String(now.day());
    }

    if (now.second() < 10) { // left pad with leading zero
      secString = "0" + String(now.second());
    } else {
      secString = String(now.second());
    }

    if (now.minute() < 10) { //  left pad with leading zero
      minString = "0" + String(now.minute());
    } else {
      minString = String(now.minute());
    }

    if (now.hour() < 10) {
      hourString = "0" + String(now.hour());
    } else {
      hourString = String(now.hour());
    }
    
    // we use iso Date_time format e.g.  2025-12-01T11:34:51  https://en.wikipedia.org/wiki/ISO_8601
    dataString += Separator + String(now.year()) + "-"+ monthString + "-" + dayString + "T"+ \
    hourString + ":" + minString + ":" + secString; 
    
#ifdef CLOCK_TIME_IS_UTC
    dataString += "Z";
#endif    
      

#ifdef APPEND_UTC_OFFSET_TO_ISO8601_DATE
    dataString += UTC_OFFSET_APPENDIX;
#endif  
    
   
// read up to 4 TMP117 sensors and put sensors to sleep again
for(int i = 0; i<4; i++)
    {
    if (TMP117_present[i]) { 
          I2Cwrite(0x48+i, Config_Reg,0b00000010, 0b01100000);  // MOD=0b00 ; Conv = 0b100 ;  AVG0 = 0b11  This  means: Continous conversion; 1 second conversion intervall,  64 averaged conversions    
       }
    }
delay(2500); // to wait for TWO conversion in order to dispose of first conversion, 
             // because possibly the first cobversion is less accurate.
for(int i = 0; i<4; i++)
    {
    if (TMP117_present[i]) { 
          StartReadTempSensorTMP117(0x48+i);
          dataString += Separator + String(ReadTempSensorTMP117(0x48+i));
          I2Cwrite(0x48+i, Config_Reg,0b00000110, 0b01100000);  // MOD=0b01 ; Conv = 0b100 ;  AVG0 = 0b11      This  means: shutdown; 1 second conversion,  64 averaged conversions
       }
    }


    SdFile::dateTimeCallback(dateTime); // time stamp on file for card file system
    File dataFile = SD.open(FILENAME, FILE_WRITE);              // write data frame to card
    if (dataFile) {
      dataFile.println(dataString);
#ifdef serial_verbose
Serial.println(dataString);
delay(200); // without this delay serial data get scrambled by premature sleep. 
            // 200 ms should be sufficient for 240 characters at 9600 baud.
#endif
      dataFile.close();
    }
    LEDredOff
  } else {
#ifdef no_Auto_power_off_on_bat_low
// for debug purposes do not go to sleep if battery low
#else
    cli();   // diable interrupts
    GOTO_SLEEP
#endif
#ifdef RedLedOn_for_low_power
    LEDredOn                          // intended just for debugging, not for production usage 
#endif                                // due to higher power consumption

  }
}
//--e-n-d---o-f-get_measurements_and_write_to_SD_card-----------------------------------------------


void go2sleep() {
  attachInterrupt(digitalPinToInterrupt(wakePin), wakeUp, LOW);  //use interrupt 0 (pin 2) and run function wakeUp when pin 2 gets LOW

GOTO_SLEEP
  detachInterrupt(digitalPinToInterrupt(wakePin));               //execution resumes from here after wake-up
  RTC.armAlarm(1, false);                                        //When exiting the sleep mode we clear the alarm
  RTC.clearAlarm(1);
  RTC.alarmInterrupt(1, false);

}


//-------------------------------------------------
int VBat() {
  // *2 (resistor voltage devider ) *3.3 (RefVoltage) / 1.024 (converting to mV)
  float measuredvbat = analogRead(BatteriePin) * 6.4453;
  return measuredvbat;
}
//-------------------------------------------------


void diagBlinkGreen (int n, int pause) {
  int i = 0;
  for (int i = 0; i < n; i++) {
    LEDgreenOn
    delay (pause);
    LEDgreenOff
    delay (pause);
  }
}

void diagBlinkRed (int n, int pause) {
  int i = 0;
  for (int i = 0; i < n; i++) {
    LEDredOn
    delay (pause);
    LEDredOff
    delay (pause);
  }
}

//-------------------------------------------------

void batCheck() {
  // test if battery voltage is above threshold defined in Batt_thresh_RedFlash_on_Startup
  if (VBat() >  mv_Batt_thresh_RedFlash_on_Startup) {  
    diagBlinkGreen (3, 300);                         // green for voltage above threshold
	}
  else {
    diagBlinkRed (3, 300);                          // red for voltage below threshold
  }

}
