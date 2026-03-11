
it should be possible to flash the controller with the command

avrdude -p m32u4 -P <YOUR_PORT> -c avr109 -U flash:w:temperature_logger_TMP117.ino.ino.with_bootloader.feather32u4.hex:i

with replacing your port with something like COM1 ... COM20 (the name of your virtual serial port of the device)