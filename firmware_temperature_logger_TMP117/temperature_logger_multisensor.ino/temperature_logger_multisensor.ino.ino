/*
  Precision temperature logger using up to four Texas Instruments sensors TMP117 with 
  Adafruit Feather 32u4 Adalogger and DS3231 real time clock 
  
  Experimental multi-sensor multi-board firware version, supporting additionally:
  --------------------------------------------------------------------------------
  ADT7420 
  Onewire senors DS18B20, DS18S20, DS1822, MAX31850 (for type K thermocouple) 
  BME280 (temperature, humidity, pressure)
  MLX90614 (IR contactless temperature)
  TSL2561 (for light intensity measurement)

  Sensors are auto-detected on startup, when actvated via #define options in the code below,
  however, the flash memory of ATmega32u4 allows only to activate about two of these
  
  Moreover, boards with ATmega 1284 or ATmega 644 allow all sensor options activated simultanously,
  however, these require to install the 'Mightycore' board option to show up in the 
  board selection menu of the Arduino IDE.

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

#define VERSION "21"
#define SUBVERSION "1"

// Attention: select board in Arduino-IDE and additionally select board in the board selector section below in this code
//            ------------------             ===============   (pre-selected default is Adafruit Feather 32u4 Adalogger)

   
//--S-t-a-r-t--o-f-----U-s-e-r---S-e-t-t-i-n-g-s----------<<<<<<<<<<<<<<<<<<<<<<<<<<<<< USER

int Intervall_seconds = 300;   // Measurement interval in seconds. 3600 (= 1 hour) must be an integer divisible by the interval! 
                       // e.g., 1, 2, 3, 4, 5, 6, 10, 12, 15, 20, 30, 300, 600 seconds
                       // the interval can be overruled by config file "settings.txt" from the SD card

String Separator = "\t";                              // .tsv .csv table separator char

#define FILENAME "uni_log.tsv" // ONLY 8 chars + dot + 3 char !!!

#define read_config_file  // read config file "settings.txt" (UNIX(LF) line end) from SD card. Relevant line:interval=300 . 
                          // Comment lines in "settings.txt"  start with / (single slash)

#define serial_verbose // serial print datastring


//#define CLOCK_TIME_IS_UTC    // un-comment this is for iso 8601 UTC date-time style e.g. '2025-12-16T15:19:01Z'

//#define APPEND_UTC_OFFSET_TO_ISO8601_DATE  // un-comment this is for iso 8601 UTC date-time with UTC offset, e.g. '2025-12-16T15:19:01+09:00'
//#define UTC_OFFSET_APPENDIX  +09:00       // un-comment this is for iso 8601 UTC date-time with UTC offset, e.g. '2025-12-16T15:19:01+09:00'

//-------S-e-n-s-o-r--I-n-c-l-u-s-i-o-n-s------------------------------------------------------------------------------------
#define include_TMP117           // NIST traceble silicon temperature 0.1 °C accurate, 0x48 - 0x4B i2c adresses are probed, can be combined with ADT7420, however just one per address, 4 in total
//#define include_ADT7420         // NIST traceble silicon temperature 0.25 °C accurate,  0x48 - 0x4B i2c adresses are probed, can be combined with TMP117, however just one per address, 4 in total
#define include_1wire            // DS1820, DS18B20, DS18S20, MAX31850(silicon temperature+type K thermocouple,mixed multiple sensors work)
//#define include_TypeK_linearization  // NIST linearization table for Type K Thermoelements for 1Wire MAX31850
//#define include_MLX90614        // IR-thermometer
//#define include_BME280_Sensor   // temperature, pressure, humidity
//#define include_TSL2561         // light (Lux) sensor
//----------- below are options that Ekkehard regularly does NOT use ------------------------------
//#define log_DS3231_temperature       // log temperature data from DS3231 clock (which is not very accurate)
//#define include_MCP3424              // code inclusion for  ADCs Attention!!!!: this blocks the startup if no MC3224 is connected, no probing implemented yet


// ------------ Board selector: uncomment the one single board you now compile for. 
// Make sure you select this board additionally also in the Arduino IDE!   
//================---------------++++++++++------------------_PCB_PCB_PCB_PCB_PCB_PCB_PCB_PCB_PCB_PCB_PCB_PCB_PCB_PCB_PCB_PCB_PC
#define ADAFRUIT_FEATHER_LOGGER
//#define ARDUINO_UNO_3Volt_logger
//#define HELDT_Logger       // m324p or m644p or m1284p 8 MHz internal
//#define ARDUINO_UNO_5Volt_testbed
//#define GREIF_BOARD_m328p
//#define GREIF_BOARD_m1284    
//#define ARDUINO_NANO
//#define OPENLOG
//---------------------------------------------------



#include <OneWire.h>  // required before board definitions


//----Start of --o-f---Board Definitions------------------------------------------------------------

#ifdef ADAFRUIT_FEATHER_LOGGER
#warning "Adafruit Feather Logger m32u4 selected"
#define mv_Batt_thresh_RedFlash_on_Startup 3750      // optische Warnung beim Einschalten : 3 x rot blinken
String LoggerName = "Feather_1";
#define OFF_mV_threshold 3500
#define wakePin 0         //use interrupt 2 (pin 0) and run function wakeUp when pin 0 gets LOW
#define ledRedPin 13         //red LED
#define ledGreenPin 8         //green LED
OneWire  ds(5);   //  PortPin für 1Wire BUS 1Wire-BUS, a 4.7K resistor to Vcc is necessary
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
#define wakePin 2         //use interrupt 2 (pin 0) and run function wakeUp when pin 0 gets LOW
#define ledRedPin  13        //red LED
#define ledGreenPin 7         //green LED
OneWire  ds(5);  //  PortPin für 1Wire BUS 1Wire-BUS, a 4.7K resistor to Vcc is necessary
const int chipSelect = 10; // CS SD Karte
#define LEDredOn    digitalWrite(ledRedPin, HIGH);
#define LEDgreenOn  digitalWrite(ledGreenPin, HIGH);
#define LEDredOff    digitalWrite(ledRedPin, LOW);
#define LEDgreenOff  digitalWrite(ledGreenPin, LOW);
int BatteriePin = 9;
#define OFF_mV_threshold 0
#define GOTO_SLEEP LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);           //arduino enters sleep mode here
#if defined __AVR_ATmega328P__
#else
    #error "Please select a m328p board in IDE."
#endif
#include <LowPower.h>
#endif


#ifdef HELDT_Logger
// Mightycore settings: 
// Board: ATmega1284p
// Bootloader if YES choose UART1
// Pinout: Standard
// Clock: internal 8 MHz
// LTO disables
// Variant 1284P
// BOD 2.7V
#warning "Heldt m1284p Logger selected"     //  mightycore bootloader could be tested using reset button. Select UART2?
#define mv_Batt_thresh_RedFlash_on_Startup 3750      // optische Warnung beim Einschalten : 3 x rot blinken
String LoggerName = "Heldt_Log";
#define OFF_mV_threshold 3550
#define wakePin 10         // D2         use interrupt 0 (pin 0) and run function wakeUp when pin 0 gets LOW
#define ledRedPin  22        // doppelLED
#define ledGreenPin 23         // doppelLED
#define LEDredOn    digitalWrite(ledGreenPin, LOW); digitalWrite(ledRedPin, HIGH);
#define LEDgreenOn  digitalWrite(ledGreenPin, HIGH); digitalWrite(ledRedPin, LOW);
#define LEDredOff      digitalWrite(ledGreenPin, LOW); digitalWrite(ledRedPin, LOW);
#define LEDgreenOff      digitalWrite(ledGreenPin, LOW); digitalWrite(ledRedPin, LOW);
OneWire  ds(3);  // B3  PortPin für 1Wire BUS 1Wire-BUS, a 4.7K resistor to Vcc is necessary
const int chipSelect = 4; // B4 CS SD Karte
int BatteriePin = 24;      // PA0 = Pin24 = ADC0
#define GOTO_SLEEP LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);           //arduino enters sleep mode here
#if defined (__AVR_ATmega324P__) ||  defined (__AVR_ATmega644P__) || defined (__AVR_ATmega1284P__)
#else
    #error "Please select MCU/Board Mightycore m324p or m644p or m1284p in IDE."
#endif
#include <LowPower.h>
#endif



#ifdef ARDUINO_NANO
#warning "Arduino Nano m328p selected"
#define mv_Batt_thresh_RedFlash_on_Startup 3750      // optische Warnung beim Einschalten : 3 x rot blinken
String LoggerName = "ArduNano";
#define OFF_mV_threshold 3550
#define wakePin 2         // PD2 use interrupt 2 (pin 0) and run function wakeUp when pin 0 gets LOW
#define ledRedPin  13        //rote LED
#define ledGreenPin 7         //grüne LED
OneWire  ds(5);  //  PortPin für 1Wire BUS 1Wire-BUS, a 4.7K resistor to Vcc is necessary
const int chipSelect = 4; // CS SD Karte
#define LEDredOn    digitalWrite(ledRedPin, HIGH);
#define LEDgreenOn  digitalWrite(ledGreenPin, HIGH);
#define LEDredOff    digitalWrite(ledRedPin, LOW);
#define LEDgreenOff  digitalWrite(ledGreenPin, LOW);
int BatteriePin = 9;
#define GOTO_SLEEP LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);           //arduino enters sleep mode here
#if defined __AVR_ATmega328P__
#else
    #error "Please select a m328p board in IDE."
#endif
#include <LowPower.h>
#endif


#ifdef ARDUINO_UNO_5Volt_testbed
#warning "Arduino Uno  m328p selected"
#define mv_Batt_thresh_RedFlash_on_Startup 3750      // optische Warnung beim Einschalten : 3 x rot blinken
String LoggerName = "Arduino_5_Volt_Testbed";
#define OFF_mV_threshold 0
#define wakePin 2         // PD2 use interrupt 2 (pin 0) and run function wakeUp when pin 0 gets LOW
#define ledRedPin  16        //rote LED
#define ledGreenPin 3         //grüne LED
OneWire  ds(17);  //  PortPin für 1Wire BUS 1Wire-BUS, a 4.7K resistor to Vcc is necessary
const int chipSelect = 10; // CS SD Karte
#define LEDredOn    digitalWrite(ledRedPin, HIGH);
#define LEDgreenOn  digitalWrite(ledGreenPin, HIGH);
#define LEDredOff    digitalWrite(ledRedPin, LOW);
#define LEDgreenOff  digitalWrite(ledGreenPin, LOW);
int BatteriePin = 15;
#define GOTO_SLEEP LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);           //arduino enters sleep mode here
#if defined __AVR_ATmega328P__
#else
    #error "Please select a m328p board in IDE."
#endif
#include <LowPower.h>
#endif


#ifdef  ARDUINO_UNO_3Volt_logger
#warning "Arduino Uno 3.3 Volt mod  m328p selected"
#define mv_Batt_thresh_RedFlash_on_Startup 3591      // optische Warnung beim Einschalten : 3 x rot blinken
String LoggerName = "Arduino_Log";
#define OFF_mV_threshold 3500
#define wakePin 2         // PD2 use interrupt 2 (pin 0) and run function wakeUp when pin 0 gets LOW
#define ledRedPin  16        //rote LED
#define ledGreenPin 3         //grüne LED
OneWire  ds(17);  //  PortPin für 1Wire BUS 1Wire-BUS, a 4.7K resistor to Vcc is necessary
const int chipSelect = 10; // CS SD Karte
#define LEDredOn    digitalWrite(ledRedPin, HIGH);
#define LEDgreenOn  digitalWrite(ledGreenPin, HIGH);
#define LEDredOff    digitalWrite(ledRedPin, LOW);
#define LEDgreenOff  digitalWrite(ledGreenPin, LOW);
int BatteriePin = 15;
#define GOTO_SLEEP LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);           //arduino enters sleep mode here
#if defined __AVR_ATmega328P__
#else
    #error "Please select a m328p board in IDE."
#endif
#include <LowPower.h>
#endif


#ifdef GREIF_BOARD_m328p
#warning "Greif board m328p selected"
#define mv_Batt_thresh_RedFlash_on_Startup 3750      // optische Warnung beim Einschalten : 3 x rot blinken
String LoggerName = "Greif_board_m328p_test_bed";
#define OFF_mV_threshold 0
#define wakePin 2         // PD2 int0 use interrupt run function wakeUp when pin 2 gets LOW. esrem: A3 did NOT work.
#define ledRedPin  6         //rote LED
#define ledGreenPin 7         //grüne LED
OneWire  ds(5);  //  PortPin für 1Wire BUS 1Wire-BUS, a 4.7K resistor to Vcc is necessary
const int chipSelect = 10; // CS SD Karte
#define LEDredOn    digitalWrite(ledRedPin, LOW);
#define LEDgreenOn  digitalWrite(ledGreenPin, LOW);
#define LEDredOff    digitalWrite(ledRedPin, HIGH);
#define LEDgreenOff  digitalWrite(ledGreenPin, HIGH);
int BatteriePin = 9;
#define GOTO_SLEEP LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);           //arduino enters sleep mode here
#if defined __AVR_ATmega328P__
#else
    #error "Please select a m328p board in IDE."
#endif
#include <LowPower.h>
#endif


#ifdef GREIF_BOARD_m1284      // if you choose mightycore bootloader select UART2
// Mightycore settings: 
// Board: ATmega1284p
// Bootloader if YES choose UART1
// Pinout: Standard
// Clock: internal 8 MHz
// LTO disables
// Variant 1284P
// BOD 2.7V
#warning "Greif board m1284p selected"
#define mv_Batt_thresh_RedFlash_on_Startup 3750      // optische Warnung beim Einschalten : 3 x rot blinken
String LoggerName = "Greif_board_m1284_test_bed";
#define OFF_mV_threshold 0
#define wakePin 10         // D2         use interrupt 0 (pin 0) and run function wakeUp when pin 0 gets LOW
#define ledRedPin  2         //rote LED     B2
#define ledGreenPin 3         //grüne LED   B3
OneWire  ds(1);  // B1  PortPin für 1Wire BUS 1Wire-BUS, a 4.7K resistor to Vcc is necessary
const int chipSelect = 4; // B4 CS SD Karte
#define LEDredOn    digitalWrite(ledRedPin, LOW);
#define LEDgreenOn  digitalWrite(ledGreenPin, LOW);
#define LEDredOff    digitalWrite(ledRedPin, HIGH);
#define LEDgreenOff  digitalWrite(ledGreenPin, HIGH);
int BatteriePin = 24;      // PA0 = Pin24 = ADC0
#define GOTO_SLEEP LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);           //arduino enters sleep mode here
#if defined (__AVR_ATmega324P__) ||  defined (__AVR_ATmega644P__) || defined (__AVR_ATmega1284P__)
#else
    #error "Please select MCU/Board Mightycore m324p or m644p or m1284p in IDE."
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


#ifdef include_MLX90614
#include <SparkFunMLX90614.h> // SparkFunMLX90614 Arduino library
IRTherm therm; // Create an IRTherm object to interact with throughout
char MLX90614_is_present = 0;
#endif


#if defined (include_BME280_Sensor) || defined (include_TSL2561)
#include <Adafruit_Sensor.h>
#endif

#ifdef include_BME280_Sensor
#include <Adafruit_BME280.h>
char bme280_is_present = 0;    
Adafruit_BME280 bme; // I2C    
#endif


#ifdef include_TSL2561
#include <Adafruit_TSL2561_U.h>
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
char TSL2561_is_present = 0;
#endif


#ifdef include_MCP3424
long ADValue; //  MCP3424
byte gain = 2;            //1,2,4, oder 8 = Verstärkung für das Messsignal 
byte resADC = 16;         // Auflösung Kanal: 18,16,14 oder 12 Bit 
int adressADC[] = {0X6A, 0X6C, 0X6E}; // Adressen der ADCc (K1,K2,K3)
#endif 





#ifdef include_TMP117



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


#endif


#if defined(include_TMP117) &&  defined(include_ADT7420)  

// test if Sensor is TMP117 using the ID register
   
const int TMP117_Device_ID = 0x0F;         // ID register

   
void SelectTMP117Register(int TMP117_Addressl, int TMP117_Register){   // select register for write
  // Points to device & begins transmission
  Wire.beginTransmission(TMP117_Addressl); 
  // Points to register to read/write data
  Wire.write(TMP117_Register); 
  // Ends data transfer and transmits data from register
  Wire.endTransmission(); 
}

char isTMP117(int TMP117_Addressl)  {
   SelectTMP117Register(TMP117_Addressl, TMP117_Device_ID);
   uint8_t data[2]; 
     // Combination of 2-byte data into 16-bit data
  int16_t datac;   

  // Requests 2-byte temperature data from device
  Wire.requestFrom(TMP117_Addressl,2); 

  // Checks if data received matches the requested 2-bytes
  if(Wire.available() <= 2){  
    // Stores each byte of data from ID register
    data[0] = Wire.read(); 
    data[1] = Wire.read();  
    // Combines data to make 16-bit binary number
    datac = ((data[0] << 8) | data[1]);     
    // delete revision number and compare type ID
    if ((datac & 0b0000111111111111) == 0x0117) {return TRUE;}
    else {return FALSE;}
  }
  } 


#endif


#ifdef include_ADT7420
/********************************************************************************************************************************************
 * Global Variables ADT7420
 *****************************************/
float adt7420_float;
short adt7420_short;

#define ARRAY_SIZE_ADT7420 2 // Maximum size of the buffer
#define REQUEST_ADT7420 2 //number of bytes to REQUEST_ADT7420 at a time from I2C


char adt7420_present[] = { 0, 0, 0, 0 }; // boolean for presence of sensor


short buffer_adt7420[ARRAY_SIZE_ADT7420]; // Create a buffer to hold values passed through I2C

/*****************************************
 * Functions ADT7420
 *****************************************/

short getTemp_ADT7420(int adr ){

  Wire.requestFrom(adr, REQUEST_ADT7420); // Request two bytes of data from the temperature sensor

  while(Wire.available()){ //While there is still data to be transfered
    for (int i =0; i < REQUEST_ADT7420; i++){ // Put REQUEST bytes
      buffer_adt7420[i] = Wire.read();
    }
  }

  short retTemp_ADT7420 = 0; // define a short value that will be set and returned by the function

  for (int i = 0; i < REQUEST_ADT7420; i++){ // A loop to handle each byte contained in the buffer. This loop recombines the bytes sent over I2C into a single short
    retTemp_ADT7420 = retTemp_ADT7420<<8; // shift all bits to the right 8 spaces
    retTemp_ADT7420 |= buffer_adt7420[i]; // This is equivalent to retTemp_ADT7420 = retTemp_ADT7420 | buffer_adt7420[i]. This effectively concatenates the retrieved bytes into a single short
  }

  // return retTemp_ADT7420*.0078; // Multiply by a scaling factor (found on the data sheet) to convert to Celsius
  return retTemp_ADT7420;

}  // /********************************************************************************************************************************************

void adt7420_control (uint8_t i2caddr, uint8_t cvalue)
    {
    	Wire.beginTransmission(i2caddr);
    	Wire.write((uint8_t)0x03); //control register address
    	Wire.write((uint8_t)cvalue); // value
    	Wire.endTransmission();
    }
#endif









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


#if defined (include_TypeK_linearization)  // MAX31850
float Klookup(float fin)
{
const float Klookupt[][2] = { // interpolation table type K Thermocouple
// [temperature],[correction value]
// printf(" 0,0: %f  0,1: %f  1,0 %f    \n",Klookupt[0][0],Klookupt[0][1],Klookupt[1][0]);
// result: 0,0: -210.000000  0,1: -63.790001  1,0 -200.000000
{ -210 , -63.79},
{ -200 , -57.28},
{ -190 , -51.18},
{ -180 , -45.54},
{ -170 , -40.29},
{ -160 , -35.45},
{ -150 , -30.97},
{ -140 , -26.88},
{ -130 , -23.13},
{ -120 , -19.75},
{ -110 , -16.68},
{ -100 , -13.90},
{ -90 , -11.43},
{ -80 , -9.26},
{ -70 , -7.32},
{ -60 , -5.66},
{ -50 , -4.23},
{ -40 , -3.01},
{ -30 , -1.99},
{ -20 , -1.15},
{ -10 , -0.50},
{ 0 , 0.00},
{ 10 , 0.38},
{ 20 , 0.67},
{ 30 , 0.85},
{ 40 , 0.95},
{ 50 , 0.99},
{ 60 , 0.98},
{ 70 , 0.93},
{ 80 , 0.85},
{ 90 , 0.80},
{ 100 , 0.77},
{ 110 , 0.76},
{ 120 , 0.80},
{ 130 , 0.92},
{ 140 , 1.06},
{ 150 , 1.29},
{ 160 , 1.55},
{ 170 , 1.84},
{ 180 , 2.17},
{ 190 , 2.51},
{ 200 , 2.84},
{ 210 , 3.12},
{ 220 , 3.41},
{ 230 , 3.65},
{ 240 , 3.86},
{ 250 , 4.02},
{ 260 , 4.14},
{ 270 , 4.20},
{ 280 , 4.25},
{ 290 , 4.24},
{ 300 , 4.21},
{ 310 , 4.16},
{ 320 , 4.08},
{ 330 , 3.98},
{ 340 , 3.87},
{ 350 , 3.72},
{ 360 , 3.55},
{ 370 , 3.37},
{ 380 , 3.17},
{ 390 , 2.97},
{ 400 , 2.75},
{ 410 , 2.50},
{ 420 , 2.25},
{ 430 , 1.98},
{ 440 , 1.71},
{ 450 , 1.41},
{ 460 , 1.11},
{ 470 , 0.82},
{ 480 , 0.50},
{ 490 , 0.18},
{ 500 , -0.15},
{ 510 , -0.49},
{ 520 , -0.81},
{ 530 , -1.16},
{ 540 , -1.48},
{ 550 , -1.80},
{ 560 , -2.14},
{ 570 , -2.46},
{ 580 , -2.78},
{ 590 , -3.08},
{ 600 , -3.38},
{ 610 , -3.67},
{ 620 , -3.97},
{ 630 , -4.24},
{ 640 , -4.49},
{ 650 , -4.74},
{ 660 , -4.96},
{ 670 , -5.19},
{ 680 , -5.36},
{ 690 , -5.56},
{ 700 , -5.71},
{ 710 , -5.86},
{ 720 , -5.97},
{ 730 , -6.07},
{ 740 , -6.15},
{ 750 , -6.20},
{ 760 , -6.26},
{ 770 , -6.26},
{ 780 , -6.24},
{ 790 , -6.23},
{ 800 , -6.16},
{ 810 , -6.09},
{ 820 , -5.98},
{ 830 , -5.86},
{ 840 , -5.72},
{ 850 , -5.53},
{ 860 , -5.35},
{ 870 , -5.11},
{ 880 , -4.87},
{ 890 , -4.59},
{ 900 , -4.30},
{ 910 , -3.97},
{ 920 , -3.64},
{ 930 , -3.28},
{ 940 , -2.87},
{ 950 , -2.47},
{ 960 , -2.01},
{ 970 , -1.53},
{ 980 , -1.05},
{ 990 , -0.53},
{ 1000 , 0.01},
{ 1010 , 0.58},
{ 1020 , 1.17},
{ 1030 , 1.79},
{ 1040 , 2.44},
{ 1050 , 3.11},
{ 1060 , 3.82},
{ 1070 , 4.54},
{ 1080 , 5.30},
{ 1090 , 6.09},
{ 1100 , 6.90},
{ 1110 , 7.75},
{ 1120 , 8.62},
{ 1130 , 9.53},
{ 1140 , 10.46},
{ 1150 , 11.43},
{ 1160 , 12.44},
{ 1170 , 13.47},
{ 1180 , 14.54},
{ 1190 , 15.65},
{ 1200 , 16.79},
{ 1210 , 17.97},
{ 1220 , 19.18},
{ 1230 , 20.43},
{ 1240 , 21.72},
{ 1250 , 23.04},
{ 1260 , 24.41},
{ 1270 , 25.81},
{ 1280 , 27.25},
{ 1290 , 28.73},
{ 1300 , 30.25},
{ 1310 , 31.80},
{ 1320 , 33.40},
{ 1330 , 35.03},
{ 1340 , 36.70},
{ 1350 , 38.40},
{ 1360 , 40.13},
{ 1370 , 41.90},
{ 2000 , 41.90}                                         // as a limit handler
};
const int LastKlookup = 159;
int i; float  fout,Scorr_diff,Fraction;
i=0;
while ((fin >= Klookupt[i][0]) && (i<LastKlookup)) {i++;}i--; if (i<0) {i=0;}
Scorr_diff = Klookupt[i+1][1]-Klookupt[i][1];
Fraction = (fin - Klookupt[i][0]) / (Klookupt[i+1][0]-(Klookupt[i][0]));
fout = fin+(Fraction*Scorr_diff)+Klookupt[i][1];
return fout;
}
#endif  



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

  Wire.begin();
  RTC.begin();
  RTC.armAlarm(1, false);                       //clear any pending alarms
  RTC.clearAlarm(1);
  RTC.alarmInterrupt(1, false);
  RTC.armAlarm(2, false);
  RTC.clearAlarm(2);
  RTC.alarmInterrupt(2, false);
  RTC.writeSqwPinMode(DS3231_OFF);              //Set SQW pin to OFF
  setWakeupTime();



#ifdef log_DS3231_temperature
    headerstr+=Separator+"DS3231_temp";     // Temperatur von der RTC
    sensorcount++;
#endif



#ifdef include_BME280_Sensor   
// Achtung 0x76 und 0x77 sind möglich
  if (bme.begin(&Wire)) {
      bme280_is_present = 1;
    bme.setSampling(Adafruit_BME280::MODE_FORCED, // got to powerdown after measurement, from adafruit  weather monitoring mode
                    Adafruit_BME280::SAMPLING_X1, // temperature
                    Adafruit_BME280::SAMPLING_X1, // pressure
                    Adafruit_BME280::SAMPLING_X1, // humidity
                    Adafruit_BME280::FILTER_OFF   );
                  headerstr+=Separator+"BME_temp"+Separator+"BME_pressure"+Separator+"BME_humidity";  
                  sensorcount++;
#ifdef es_debug_mode
Serial.println("bme280 found by Adafruit library");
#endif                    

        } else {
#ifdef es_debug_mode
Serial.println("No bme280 found by Adafruit library");
#endif            
       }
#endif       // BME280 Sensor




#if defined(include_TMP117) && !defined(include_ADT7420)  // only TM117 
// Test if up to 4 TMP117 sensors are installed

for(int i = 0; i<4; i++)
{
Wire.beginTransmission(0x48+i); // 0x48 is first adress of sensor
  if (Wire.endTransmission() == 0) {
    I2Cwrite(0x48+i, Config_Reg,0b00000110, 0b01100000);  // MOD=0b01 ; Conv = 0b100 ;  AVG0 = 0b11      This  means: shutdown; 1 second conversion,  64 averaged conversions      
    TMP117_present[i]= TRUE;
    headerstr+=Separator+"TMP"+ String (i+1); sensorcount++;
    // Serial.println("TMP117 found at 0x48");
  }
}
#endif



#if defined(include_ADT7420) && !defined(include_TMP117) // only ADT7420
// Test if up to 4 ADT7420 sensors are installed

for(int i = 0; i<4; i++)
{
Wire.beginTransmission(0x48+i); // 0x48 is first adress of sensor
  if (Wire.endTransmission() == 0) {
  adt7420_present[i]= TRUE;
      headerstr+=Separator+"ADT"+ String (i+1); sensorcount++;
   // Serial.println("ADT7420 found at 0x48");
  }
}
#endif



#if defined(include_TMP117) && defined(include_ADT7420)  // TMP117 and ADT7420 co-usage is allowed, if i2c addresses are different
// Test if up to 4 TMP117 sensors are installed
// Test if up to 4 ADT7420 sensors are installed
String TMP_header = "";
String ADT_header = "";  
for(int i = 0; i<4; i++)
{
Wire.beginTransmission(0x48+i); // 0x48 is first adress of sensor

  if (Wire.endTransmission() == 0) {
    if  (isTMP117(0x48+i)) { 
        I2Cwrite(0x48+i, Config_Reg,0b00000110, 0b01100000);  // MOD=0b01 ; Conv = 0b100 ;  AVG0 = 0b11      This  means: shutdown; 1 second conversion,  64 averaged conversions      
        TMP117_present[i]= TRUE;
        TMP_header+=Separator+"TMP"+ String (i+1); sensorcount++;
        // Serial.println("TMP117 found at 0x48");
    }
    else {     
  adt7420_present[i]= TRUE;
      ADT_header+=Separator+"ADT"+ String (i+1); sensorcount++;
   // Serial.println("ADT7420 found at 0x48");
    }
  }
}
headerstr+=TMP_header+ADT_header;
#endif




#ifdef include_MLX90614
  Wire.beginTransmission(0x5A);
  if (Wire.endTransmission() == 0) {
      MLX90614_is_present = 1;
      headerstr+=Separator+"MLX"; sensorcount++;
     // Serial.println("MLX90614 found at 0x5A");
     therm.begin(); // Initialize thermal IR sensor
     therm.setUnit(TEMP_C); // Set the library's units to Ceslisus
   }
#endif   


#ifdef include_1wire
{         // start of 1Wire Sensors header writing
  byte i = 1;
  byte addr[8];

 while (ds.search(addr)) {
  if (OneWire::crc8(addr, 7) == addr[7]) { // CRC is valid
//  headerstr+=Separator+"DS"+String(i++); sensorcount++;  // obsolete:  print DS followed by identification count number i 
headerstr+=Separator+"DS"+String(addr[7]); sensorcount++;  // print DS followed by CRC-code of each sensor ROM
  }                                                        // I use this to get a fingerprint ID of each specific sensor
}  // finished  with all 1Wire Sensors                     // so that for mixed multi-sensor arrangements the number 
    ds.reset_search();                                     // stays with this specific sensor
    delay(250);
} // end of 1Wire Sensors header writing
#endif


#ifdef include_TSL2561
if(tsl.begin())
{
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  //tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  //tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
   tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */
  TSL2561_is_present = 1;
  headerstr+=Separator+"Lux"; sensorcount++;  // print headers
  }
#endif


#ifdef include_MCP3424
  adc(adressADC[0], 1, 1, 12);                  //Die drei ADCs schlafen legen
  adc(adressADC[1], 1, 1, 12);                  //Der OneShot Mode legt den Baustein nach einer Konvertierung in Sleepmode
  adc(adressADC[2], 1, 1, 12);
#endif

diagBlinkGreen(sensorcount, 700);


  File dataFile = SD.open(FILENAME, FILE_WRITE);
  if (dataFile) {
    dataFile.println(headerstr);
   //  dataFile.println("// gain: " + String(gain) + " res: " + String(resADC));
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
    
   
#ifdef log_DS3231_temperature
    dataString +=  Separator + String(RTC.getTemp());     // Temperatur von der RTC
#endif
#ifdef include_BME280_Sensor
if (bme280_is_present) {
    bme.takeForcedMeasurement(); // forced mode to have sensor in auto-powerdown afterwards
    delay(750);
    dataString += Separator;   
    dataString += String(bme.readTemperature()); 
    dataString += Separator;   
    dataString += String(bme.readPressure()); 
    dataString += Separator;       
    dataString += String(bme.readHumidity()); 

}    // end bme280
#endif    


#ifdef include_TMP117
// read up to 4 TMP117 sensors and put sensors to sleep again
for(int i = 0; i<4; i++)
    {
    if (TMP117_present[i]) { 
          I2Cwrite(0x48+i, Config_Reg,0b00000010, 0b01100000);  // MOD=0b00 ; Conv = 0b100 ;  AVG0 = 0b11  This  means: Continous conversion; 1 second conversion intervall,  64 averaged conversions    
       }
    }
delay(2500); // to wait for TWO conversion in order to dispose of first conversion. Perhaps it is less accurate
for(int i = 0; i<4; i++)
    {
    if (TMP117_present[i]) { 
          StartReadTempSensorTMP117(0x48+i);
          dataString += Separator;          
          dataString += String(ReadTempSensorTMP117(0x48+i));
          I2Cwrite(0x48+i, Config_Reg,0b00000110, 0b01100000);  // MOD=0b01 ; Conv = 0b100 ;  AVG0 = 0b11      This  means: shutdown; 1 second conversion,  64 averaged conversions
       }
    }
#endif



#ifdef include_ADT7420
// read up to 4 ADT7420 sensors and put sensors to sleep again
for(int i = 0; i<4; i++)
    {
    if (adt7420_present[i]) { 
          adt7420_control(0x48+i,0xb10000000);  // continous vonversion 16 bit // 0x48 is first adress of sensor    
          delay(250);
          adt7420_float = 0; 
          for(int averages = 1; averages<9; averages++)  {    // make 8 averages  for one measurement, takes 2.25 seconds
              adt7420_float += getTemp_ADT7420(0x48+i); // 0x48 is first adress of sensor
              delay(250);
              }
          adt7420_float = adt7420_float  * 0.0078125 / 8;  // actuelly 0.0078 is the datasheet value for ADT7420, 0.0078125 is the datasheet value for ADT7422 (diff: 25°C -> 25.04°C)
          // Serial.print(adt7420_float, 2); // Print the temperature through the serial port
          // Serial.print(Separator);
          dataString += Separator;
          dataString += String(adt7420_float); //adc Funktion
          adt7420_control(0x48+i,0xb11100000); // shutdown
       }
    }
#endif

#ifdef include_MLX90614
 if (MLX90614_is_present) {
  // therm.wake();    // this fails !!!!!!!!!!!!
  delay(1000);
  if (therm.read()) // On success, read() will return 1, on fail 0.
  {
    // Use the object() and ambient() functions to grab the object and ambient
	// temperatures.
	// They'll be floats, calculated out to the unit you set with setUnit().
    //Serial.print("Object: " + String(therm.object(), 2));
    //Serial.write('°'); // Degree Symbol
    //Serial.println("C");
    //Serial.print("Ambient: " + String(therm.ambient(), 2));
    //Serial.write('°'); // Degree Symbol
    //Serial.println("C");
    //Serial.println();
    dataString += Separator;    
    dataString += String(therm.object(), 2); //adc Funktion
	}
  // therm.sleep(); // disabled because wake fails
 }  // finished with single MLX90614 Thermopile
 #endif

#ifdef include_1wire
{         // start  of 1Wire Sensors
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius;


 while (ds.search(addr)) {

 // Serial.print("ROM =");
 // for( i = 0; i < 8; i++) {
 //   Serial.write(' ');
 //   Serial.print(addr[i], HEX);
 //}

  if (OneWire::crc8(addr, 7) == addr[7]) { // CRC is valid

  //Serial.println();

  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x3b:
      //Serial.println("  Chip = MAX31850");
      type_s = 3;
      break;
    case 0x10:
      //Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      //Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      //Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
	  default:
      //Serial.println("Device is not a DS18x20 family device and also not a MAX31850.");
      return;
  }
do {
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end

  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);         // Read Scratchpad

  //Serial.print("  Data = ");
  //Serial.print(present, HEX);
  //Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    //Serial.print(data[i], HEX);
    //Serial.print(" ");
  }
  //Serial.print(" CRC=");
  //Serial.print(OneWire::crc8(data, 8), HEX);
  //Serial.println();
}while (OneWire::crc8(data, 8) !=  data[8]);

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.

  int16_t raw = (data[1] << 8) | data[0];

byte cfg;
  switch (type_s) {
    case 0:
      cfg = (data[4] & 0x60); // DS18B20
      // at lower res, the low bits are undefined, so let's zero them
      if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
      else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
      else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
      //// default is 12 bit resolution, 750 ms conversion time
      break;
    case 1:
      raw = raw << 3; // 9 bit resolution default  //DS18S20  DS1820
      if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
	  }
      break;
    case 3:
	  raw = raw & 0b1111111111111100;  // MAX31850 Achtung: das ist nur für positive Werte korrekt, für negative ist der Fehler 3/16°C
      break;
     default: ;
  }

  celsius = (float)raw / 16.0;

#if defined (include_TypeK_linearization)  // MAX31850
    if (type_s == 3) {
    celsius = Klookup(celsius);
    }
#endif  

  //Serial.print(celsius);
  //Serial.print(Separator);
  dataString += Separator;  
  dataString += String(celsius); //adc Funktion
  } // block for valid CRC

}  // finished  with all 1Wire Sensors
    // Serial.println("No more addresses.");
    // Serial.println();            //Zeilenende
    ds.reset_search();
    delay(250);
} // 1-Wire very end
#endif


#ifdef include_TSL2561
 if (TSL2561_is_present) {
  /* Get a new sensor event */ 
  sensors_event_t event;
  tsl.getEvent(&event);
 
  /* Display the results (light is measured in lux) */
  if (event.light)
  {
  dataString += Separator;
  dataString += String(event.light); 
  }
  else
  {
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
    //Serial.println("Sensor overload");   //es: however; there are alos real zeroes
      dataString += Separator;
      dataString += "0"; 
  }
 }
#endif

#ifdef include_MCP3424
    // ADCs auslesen und Datenstring erstellen
     for (int k = 0; k < sizeof(adressADC) / 2; k++)                 //Bei M0 geteilt durch 4, da int 4Byte hat. Beim Arduino durch 2 teilen!
    {
      for (int i = 1; i < 5 ; i++)                                  // CH1-4 abfragen
      {
        dataString += Separator;
        dataString += String(digit2uV((adc(adressADC[k], i, gain, resADC)), gain, resADC)); //adc Funktion
      }
      // dataString += Separator;
    } 
#endif    

    SdFile::dateTimeCallback(dateTime); // for right time stamp on file
    File dataFile = SD.open(FILENAME, FILE_WRITE);              // Datensatz auf SD Karte schreiben
    if (dataFile) {
      dataFile.println(dataString);
#ifdef serial_verbose
Serial.println(dataString);
delay(200); // without this delay serial data get scrambled by premature sleep. 200 ms should be sufficient for 240 characters at 9600 baud.
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
    LEDredOn                          // Als Debughilfe LED einschalten - nur für Demo!!
#endif

  }
}
//--e-n-d---o-f--m-e-s-s-e-n---u-n-d---s-c-h-r-e-i-b-e-n-----------------------------------------------



void go2sleep() {
  attachInterrupt(digitalPinToInterrupt(wakePin), wakeUp, LOW);  //use interrupt 0 (pin 2) and run function wakeUp when pin 2 gets LOW

GOTO_SLEEP
  detachInterrupt(digitalPinToInterrupt(wakePin));               //execution resumes from here after wake-up
  RTC.armAlarm(1, false);                                        //When exiting the sleep mode we clear the alarm
  RTC.clearAlarm(1);
  RTC.alarmInterrupt(1, false);

}


#ifdef include_MCP3424
//-------------------------------------------------
//-------------------------------------------------
// MCP3424 ADC
// config register 0x8C = CH0, one shot, 18 Bit, gain 0, %1000 1100
// config register 0x9c = CH0, continous, 18 Bit, gain 0, %1001 1100
// config register 0x88 = CH0, one shot, 16 Bit, gain 0, %1000 1000
// config register 0x98 = CH0, continous, 16 Bit, gain 0, %1001 1000
// config register 0x84 = CH0, one shot, 14 Bit, gain 0, %1000 1000
// config register 0x94 = CH0, continous, 14 Bit, gain 0, %1001 1000
// config register 0x80 = CH0, one shot, 12 Bit, gain 0, %1000 1000
// config register 0x90 = CH0, continous, 12 Bit, gain 0, %1001 1000
long adc(byte adr, byte channel, byte gain, byte resolution)
{
  long ADVal, byte1 = 0, byte2 = 0, byte3 = 0;
  byte Config, sign;


  if (resolution == 18)  Config = 0x8c;               // Auflösung einstellen
  if (resolution == 16)  Config = 0x88;
  if (resolution == 14)  Config = 0x84;
  if (resolution == 12)  Config = 0x80;
  if (channel == 2 ) Config = Config | 0x20;          // Kanal einstellen
  if (channel == 3 ) Config = Config | 0x40;
  if (channel == 4 ) Config = Config | 0x60;
  if (gain == 2) Config = Config | 0x1;               //Vorverstärker einstellen
  if (gain == 4) Config = Config | 0x2;
  if (gain == 8) Config = Config | 0x3;
  //mySerial.println(Config,BIN);
  //TinyWireM.beginTransmission(adr);
  Wire.beginTransmission(adr);
  Wire.write(Config);                               // sends value byte
  Wire.endTransmission();                           // stop transmitting


  do {                                              // ADC Werte auslesen
    if (resolution == 18) {
      //delay (270);

      Wire.requestFrom((int)adr, (int) 4);
      byte1 = Wire.read();;
      byte2 = Wire.read();
      byte3 = Wire.read();
      Config = Wire.read();
    }
    else
    {
      Wire.requestFrom((int)adr, (int) 3);
      byte1 = Wire.read();
      byte2 = Wire.read();
      Config = Wire.read();
    }
  } while ((Config & 0x80) == 0x80);             //Prüfen ob Wandlung schon ein neuer Wert anliegt


  if (resolution == 18) {                        // Vorzeichen prüfen und Digits berechnen bei 18 Bit
    sign = byte1 >> 7;
    byte1 = (byte1 & 3);

    ADVal = (byte1 * 65536) + (byte2 * 256) + byte3;
    if (sign == 1) ADVal = ADVal - 262144; // wenn Ergebnis negativ
  }

  if (resolution == 16) {
    ADVal = (byte1 * 256) + byte2;               // Vorzeichen prüfen und Digits berechnen bei 16 Bit
    if (ADVal > 32767) ADVal = ADVal - 65536;
  }
  if (resolution == 14) {                         // Vorzeichen prüfen und Digits berechnen bei 14 Bit
    sign = byte1 >> 5;
    sign = sign & 1;
    byte1 = (byte1 & 0x1F);
    ADVal = (byte1 * 256) + byte2;
    if (sign == 1) ADVal = ADVal - 8192; // wenn Ergebnis negativ
  }

  if (resolution == 12) {                         // Vorzeichen prüfen und Digits berechnen bei 12 Bit
    sign = byte1 >> 3;
    sign = sign & 1;
    byte1 = (byte1 & 0x1F);
    ADVal = (byte1 * 256) + byte2;
    if (sign == 1) ADVal = ADVal - 2048; // wenn Ergebnis negativ
  }

  return (ADVal);
}


 long digit2uV(long digit, byte gain, byte resolution)
// Spannung (in nV) wird aus Digitanzahl, Verstärkung und der Auflösung berechnet
// Zurückgegeben werden uV
{
  long nV;
  if (resolution == 18) nV = 15625 * digit / gain;
  if (resolution == 16) nV = 62500 * digit / gain;
  if (resolution == 14) nV = 250000 * digit / gain;
  if (resolution == 12) nV = 1000000 * digit / gain;

  return nV / 1000;
} 
#endif


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
