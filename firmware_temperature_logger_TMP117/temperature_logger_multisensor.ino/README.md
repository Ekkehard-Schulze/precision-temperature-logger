Notes
-----
**Alternative firmware, which supports these sensors:**

* TMP117 or TMP119 (precision temperature, up to 4 sensors)
* OneWire temperature, this allows for a larger number of sensors on a several meters long two-conductor cable
  * DS18B20 
  * DS18S20
  * DS1822
  * MAX31850  for type K thermocouples

* BME280 (temperature, humidity, pressure)

* MLX90614 (infrared non-contact temperature)

* TSL2561 (illuminance)

* ADT7420 (precision temperature, up to 4 sensors, no address overlap with TMP117 allowed, inferior to TMP117A)	

This firmware is more complicated and requires selecting sensor code by uncommenting 
'#define' statements in the C source code. The resulting firmware then detects the selected 
sensors on each power-up. Selected sensor types and numbers can be mixed in any combination. 
This approach is required, because it is not possible to include all sensors options together 
in one firmware due to memory limitations of the 'Adafruit Feather 32u4 Adalogger #2795'. The 
compiler will tell you, if the combination of the sensors you selected fits into the memory.


1. The file 'temperature_logger_multisensor.ino.ino' is the source code for the Arduino IDE 1.8.19.
***You need to install the Arduino IDE and the required libraries to compile this***, see the ../libraries folder here.
You find more information here: https://learn.adafruit.com/adafruit-feather-32u4-adalogger/using-with-arduino-ide.
If you require differnt sensor combinations than TMP117+1Wire you need to edit the user 
settings in the code. Due to memory limitations of Atmega 32U4 it is not possible to select all sensors at the same time.

2. The file 'temperature_logger_TMP117+ADT7420+1Wire_multisensor.with_bootloader.feather32u4.hex' is a 
pre-compiled example binary for
the 'Adafruit Feather 32u4 Adalogger #2795'. It should be possible to flash the controller with the command
    
    avrdude -p m32u4 -P <YOUR_PORT> -c avr109 -U flash:w:<HEX_FILE>:i
    
    with replacing your <YOUR_PORT> with something like COM1 ... COM99 (the name of your virtual serial port of the device) 
and replacing
    <HEX_FILE> with temperature_logger_TMP117.ino.ino.with_bootloader.feather32u4.hex.
You may have to **double-click** the RST button to get into the bootloader. 
***You need to install avrdude for this.***
If you choose this option, you do not need to install and operate the\
&emsp;&emsp;Arduino IDE and libraries.


