Precision temperature SD-card logger for laboratory usage
---------------------------------------------------------

We provide hardware design files, firmware, and a script

for interactive data plotting and statistics.

The logger is low power and intended for battery usage.

It runs for month from a single 18650 LiIon cell

without re-charging. Multiple devices have served

various laboratory applications for years.

Up to four TMP117 sensors provide NIST-traceable 

accuracy not worse than ±0.1°C from –20 °C to 50°C.

Logging intervals can be set in a settings.txt file on the 

SD-card by the user. The sensors are auto-detected, 

and all combinations of the four possible sensor addesses 

can be used with the same firmware.

The logger writes a 'tab or comma separated value'

format text file with ISO 8601 formatted datetime strings 

to a micro-SD card. The format is compatible with python's pandas 

and plotly packages as well as with standard spreadsheet

processing. 

We provide a python script for plotting, interactive

analysis and descriptive statistics. 

The target board is 'Adafruit Feather 32u4 Adalogger #2795' 

in combination with a precision real time clock DS3231.

We proved a PCB to integrate a 18650 lithium ion battery and a 

DS3231 real time clock with the 'Adafruit feather logger' and 

morevoer, also miniature 7 x 5 mm sized breakout boards for 

the TMP117 sensors for laboratory usage e. g. in agar plates or 

liquids.


Notes
-----

1.) You may like to try the improved, but software compatible sensor type TMP119.

2.) For a qick start instead of using our project's custom printed circuit 

board the 'Adafruit Feather 32u4 Adalogger #2795' can be combined with an 

'Adafruit DS3231 Precision RTC FeatherWing #3028' and powered with a 

commercially available pluggable LiPo or LiIon battery. A few TMP117 breakout 

boards are commercially available, but have a larger size than

our laboratory optimizes sensor boards.

3.) Our firmware will also run on the 'SparkFun OpenLog #DEV-13712' board, but the 

user will have to breakout the I2C bus, provide pull-up resistors for the I2C bus, 

a voltage devider for battery voltage sensing, a DS3231 clock, and TMP117 sensors.


Contributions
-------------

This device was developed by the department of Bioinformatics and Molecular Genetics, 

Faculty of Biology ('The Baumeister Lab'), University of Freiburg, 79104 Freiburg, 

Germany in collaboration with the intitute's electronics workshop 'AT-Technik'.

The hardware was designed by Jürgen Schmidt and constructed by 

Joshua Schwarber. Jürgen Schmidt wrote an initial low-power voltage 

logging firmware, which then was extended to the present temperature 

logging firmware by Ekkehard Schulze. Ekkehard Schulze also tested 

many alternative temperature sensors and identified the TMP117 as the 

only type satifying our specific requirements in the temperature range

close to 0°C.

