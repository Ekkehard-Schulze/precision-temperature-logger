Precision temperature SD-card logger for laboratory usage
---------------------------------------------------------

We provide hardware design files, firmware, and a script

for interactive data plotting and statistics.

The logger is low power and intended for battery usage.

It runs for month from a single 18650 LiIon cell

without re-charging. Multiple devices have served

various laboratory applications for years.

Up to four TMP117 sensors provide NIST-traceable 

accuracy not worse than ±0.1°C from –20°C to 50°C.

Logging intervals can be set in a settings.txt file on the 

SD-card by the user. The sensors are auto-detected, 

and all combinations of the four possible sensor addesses 

can be used with the same firmware.

The logger writes a 'tab or comma separated value'

format text file with ISO 8601 formatted date and time strings 

to a micro-SD card. The format is compatible with python's pandas 

and plotly packages as well as with standard spreadsheet

processing. 

We provide a python script for plotting, interactive

analysis and descriptive statistics. 

The target board is 'Adafruit Feather 32u4 Adalogger #2795'.

It works in combination with a precision real time clock DS3231.

We proved a PCB to integrate a 18650 lithium ion battery and a 

DS3231 real time clock with the 'Adafruit Feather 32u4 Adalogger' and 

morevoer, four miniature 7 x 5 mm sized breakout boards for wire-

attached TMP117 sensors. These serve as probes for laboratory usage, 

and can be inserted into agar plates or liquids. The main board has 

two optional places for on-board TMP117

sensors to log the environmental temperature, for example for usage 

in incubators.


Notes
-----

1.) For usage in wet, salty or corrosive environments you

must cover all exposed sensor board conductive surfaces with a high 

quality polyepoxide (cured epoxy resin).


2.) For a qick start instead of using our project's custom printed circuit 

board the 'Adafruit Feather 32u4 Adalogger #2795' can be combined with an 

'Adafruit DS3231 Precision RTC FeatherWing #3028' and powered with a 

commercially available pluggable LiPo or LiIon battery. Some TMP117 breakout 

boards are commercially available (e. g. Adafruit #4821 or Sparkfun #15805), 

but these have a much larger size than our sensor boards, which are optimized

for watertight sealing and laboratory usage, e. g. inside the salty wet agar of

a Petri dish.


3.) Our firmware will also run on the 'SparkFun OpenLog #DEV-13712' board, but the 

user will have to breakout the I2C bus, provide pull-up resistors for the I2C bus, 

a voltage devider for battery voltage sensing, a DS3231 clock, and TMP117 sensors.


4.) You may like to try the software compatible sensor TMP119.

TMP119 has higher a accuracy (±0.03°C (typical) from 0°C to 45°C

and ±0.08°C (maximum) from 0°C to 45°C), but is only avaible in the 

YBG0006 Die Size Ball Grid Array package, whereas our boards use the 

WSON package.


Contributions
-------------

This device was developed by the department of Bioinformatics and Molecular Genetics

('The Baumeister Lab'), Institute for Biology III, Faculty of Biology, 

University of Freiburg, Schänzlestraße 1, 79104 Freiburg, Germany

in collaboration with the institute's electronics workshop 'AT-Technik'.

The hardware was designed by Jürgen Schmidt and constructed by 

Joshua Schwarber. Jürgen Schmidt wrote an initial low-power voltage 

logging firmware, which then was extended to the present temperature 

logging firmware by Ekkehard Schulze. Ekkehard Schulze also tested 

many alternative temperature sensors and identified the TMP117 as the 

only type satisfying our specific requirements in the temperature range

close to 0°C.


