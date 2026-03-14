Arduino firmware to set the time of the DS3231 real time clock
============================================================

Choose one of three alternative methods
-------

1. Compile and upload the code in the Arduino IDE; the microcontroller then restarts and updates the DS3231 RTC
to the compile time of the script. 
Using this method, the RTC will be a little (e.g. 1 second)
behind the real time. Make sure the microcontroller is not started for a second time, this 
would set a time which is far more behind the real time. After setting the DS3231 RTC time, upload the 
application firmware to the microcontroller. 

2. To set a different time than the PC's system time, temporarily change
the PC's system time to the intended time, compile and run the firmware, and then change the PC's
system time back to the original. This approach can for example be used to set the clock to UTC.

3. Alternatively, you can uncomment line 33 and edit it to set the clock for a fixed time. 
Compile and upload the software, then use an accurate reference clock to reset or startup
the microcontroller manually at this specific time.