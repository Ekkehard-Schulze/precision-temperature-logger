Notes
-----
**This Arduino firmware sets the time of the DS3231 real time clock to the compile time of the script**

Compile and upload the code in the Arduino IDE; the microcontroller then restarts and updates the DS3231 RTC. 
Using this method, the RTC will be a little (e.g. 1 second)
behind the real time. Make sure the microcontroller is not started for a second time, this 
would set a time which is far more behind the real time. After setting the DS3231 RTC time, upload the 
application firmware to the microcontroller. 

 To set a different time than the PC's system time, temporarily change
the PC's system time to the intended time, compile and run the firmware, and then change the PC's
system time back to the original. This approach can for example be used to set the clock to UTC.
