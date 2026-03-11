
The file 'temperature_logger_TMP117.ino.ino.with_bootloader.feather32u4.hex' is a pre-compiled binary for
the Adafruit feather logger.


It should be possible to flash the controller with the command

avrdude&nbsp;-p&nbsp;m32u4&nbsp;-P&nbsp;<YOUR_PORT>&nbsp;-c&nbsp;avr109&nbsp;-U&nbsp;flash:w:temperature_logger_TMP117.ino.ino.with_bootloader.feather32u4.hex:i

with replacing your port with something like COM1 ... COM20 (the name of your virtual serial port of the device).

The file 'temperature_logger_TMP117.ino.ino' is the source code for the Arduino IDE 1.8.6.
You need to install the required libraies to comile this, see the ../lib folder here.