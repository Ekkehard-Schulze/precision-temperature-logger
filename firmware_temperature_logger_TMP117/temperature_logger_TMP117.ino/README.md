
The file 'temperature_logger_TMP117.ino.ino.with_bootloader.feather32u4.hex' is a pre-compiled binary for
the Adafruit feather logger.


It should be possible to flash the controller with the command

avrdude -p m32u4 -P <YOUR_PORT> -c avr109 -U flash:w:<HEX_FILE>:i

with replacing your <YOUR_PORT> with something like COM1 ... COM20 (the name of your virtual serial port of the device) 
and replacing <HEX_FILE> with temperature_logger_TMP117.ino.ino.with_bootloader.feather32u4.hex.



The file 'temperature_logger_TMP117.ino.ino' is the source code for the Arduino IDE 1.8.6.
You need to install the required libraies to comile this, see the ../lib folder here.