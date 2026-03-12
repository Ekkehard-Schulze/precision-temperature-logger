Notes
-----
This firmware is kept simple and supports four TMP117/TMP119 sensors, one for each of the dedicated I2C-bus addresses.

1. The file 'temperature_logger_TMP117.ino.ino' is the source code for the Arduino IDE 1.8.19.
You need to install the required libraries to compile this, see the ../libraries folder here.
You find more information here: https://learn.adafruit.com/adafruit-feather-32u4-adalogger/using-with-arduino-ide

2. The file 'temperature_logger_TMP117.ino.ino.with_bootloader.feather32u4.hex' is a pre-compiled binary for
the 'Adafruit Feather 32u4 Adalogger #2795'. It should be possible to flash the controller with the command

&emsp;&emsp;avrdude -p m32u4 -P <YOUR_PORT> -c avr109 -U flash:w:<HEX_FILE>:i

&emsp;&emsp;with replacing your <YOUR_PORT> with something like COM1 ... COM20 (the name of your virtual serial port of the device) 
and replacing <HEX_FILE> with temperature_logger_TMP117.ino.ino.with_bootloader.feather32u4.hex.\
You may have to **double-click** the RST button to get into the bootloader. You need to install avrdude for this. If you choose this option, you do not need to install and operate the
Arduino IDE and libraries.

