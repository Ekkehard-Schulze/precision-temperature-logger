Notes
-----
**This Arduino firmware sets the time of the DS3231 real time clock to the compile time of the script**

So, compile and upload the code using the Arduino IDE. Typically, after the upload the microcontroller restarts
and transfers the compile time to the DS3231 RTC. Using this method, the RTC will be a little
behind the real time. Make sure the microcontroller is not startet a second time, this 
would give a time which is far behind the real time. Instead, after setting the DS3231 RTC
flash the application firmware to the microcontroller.