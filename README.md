Precision temperature SD-card logger for laboratory usage.

It is low power and intended for battery usage.

It runs for many month from a single LiIon 18850 cell

withour re-charging.

Up to four TMP117 sensors provide ±0.1°C NIST-

tracable accuracy. Logging intervals can be

set in a settings.txt file on the SD-card by the user.

The sensors are auto-detected, and all combinations of

the four possible sensor addesses can be used 

without adapting the software.

The logger writes a 'tab or comma separated value'

format text file with ISO 8601 date strings.

The format is compatible with the python's pandas and 

plotly packages as well as with standard spreadsheet

processing. We provide a python script for plotting, interactive

analysis and descriptive statistics. 

The target board is 'Adafruit Feather 32u4 Adalogger #2795' 

in combination with a precision real time clock DS3231.

The firmware is Arduino 1.8.11 based.

We proved a PCB to integrate a lithium ion battery and a DS3231 real time clock

with the 'Adafruit feather logger' and morevoer, also miniature 7 x 5 mm 

sized breakout boards for the TMP117 sensors for usage e. g. in agar plates or liquids.

Notes:

1.) You may like to try the improved, but software compatible sensor type TMP119.

2.) For a qick start instead of using our project's custom printed circuit 

board the 'Adafruit Feather 32u4 Adalogger #2795' can be combined with an 

'Adafruit DS3231 Precision RTC FeatherWing #3028'and powered with a 

commercially available pluggable LiPo or LiIon battery. TMP117 breakout 

boards are commercially available, but typically have a larger size than

our laboratory optimizes sensor boards.

3.) Our firmware will also run on the 'SparkFun OpenLog #DEV-13712'  board, but the user will have

to breakout the I2C bus, provide pull-up resistors for the I2C bus, 

a voltage devider for battery voltage sensing, TMP117 sensors and a DS3231 clock.

