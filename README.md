Precision temperature SD-card logger for laboratory use
-------------------------------------------------------

The device is low power and runs for month from a single 

18650 LiIon cell without re-charging. We provide hardware 

design files, firmware, and a script for interactive 

graphical data analyzis and statistics. Multiple loggers 

have served various laboratory applications for years.

Up to four TMP117A sensors provide NIST-traceable 

accuracy not worse than ±0.1°C from –20°C to 50°C.

The firmware supports all combinations of the four 

possible sensor addesses.

The logger writes a 'tab or comma separated value'

formated text file with ISO 8601 date and time strings 

to a micro-SD card. This format is compatible with python's pandas 

and plotly packages as well as with standard spreadsheet

processing. The user can specify the logging interval 

in a settings.txt file on the SD-card.

We use 'Adafruit Feather 32u4 Adalogger #2795' as the controller 

board in combination with a precision real time clock DS3231.

We provide a PCB to integrate a 18650 lithium ion battery and the 

DS3231 real time clock with the 'Adafruit Feather 32u4 Adalogger'. 

Moreover, we provide four miniature 7 x 5 mm sized breakout boards for wire-

attached TMP117A sensors. These serve as probes in laboratory applications, 

for example for usage in agar plates, liquid samples and laboratory devices.

The main board has two optional places for on-board TMP117A sensors, 

which serve as environmental temperature sensors, for example for usage in 

incubators or climatized lab rooms.


Notes
-----

1.) For measurements in wet, salty or corrosive environments you

must cover all exposed sensor board conductive surfaces with a high 

quality polyepoxide (cured epoxy resin).


2.) For a qick start instead of using our project's printed circuit 

board you can use the 'Adafruit Feather 32u4 Adalogger #2795' with an 

'Adafruit DS3231 Precision RTC FeatherWing #3028' and power it with a 

pluggable LiPo or LiIon battery. Some TMP117 breakout boards are commercially 

available (e. g. Adafruit #4821 or Sparkfun #15805), but these have a much 

larger size than our sensor boards and are more difficult to seal for use

in wet and corrosive environments.


3.) Our firmware will also run on the 'SparkFun OpenLog #DEV-13712' board, but the 

user will have to breakout the I2C bus, provide pull-up resistors for the I2C bus, 

a voltage divider for battery voltage sensing, a DS3231 clock, and TMP117 sensors.


4.) Take note that TMP117 product versions TMP117N and TMP117M are slightly 

less accurate than TMP117A.


5.) You may like to try the software compatible sensor TMP119.

TMP119 has a higher accuracy (±0.03°C (typical) from 0°C to 45°C

and ±0.08°C (maximum) from 0°C to 45°C), but is only avaible in the 

YBG0006 'Die Size Ball Grid Array' package, whereas our boards use the 

slightly larger WSON package. Remarkably, the die size of TMP119 

is just 1.5 mm x 1.0 mm. This may allow laboratory aplications,

for which the 2.1 mm x 2.1 mm sized WSON package size we used is 

too large.


Contributions
-------------

This device was developed by the department of Bioinformatics and Molecular Genetics

('The Baumeister Lab'), Institute for Biology III, Faculty of Biology, 

University of Freiburg, Schänzlestraße 1, 79104 Freiburg, Germany

in collaboration with the institute's electronics workshop 'AT-Technik'.

The hardware was designed by Jürgen Schmidt and constructed by 

Joshua Schwarber. Jürgen Schmidt wrote an initial low-power voltage 

logging firmware, which then was extended to the present temperature 

logging firmware by Ekkehard Schulze. Ekkehard Schulze tested 

multiple different temperature sensors typs and identified the 

Texas Instruments sensor TMP117A as the only type satisfying 

our experimental requirements in the temperature range close to 0°C.


Images
-------

![Sensor before and after coating with epoxy resin](https://github.com/Ekkehard-Schulze/precision-temperature-logger/blob/main/hardware_sensor_breakout_boards/TMP117-WSON-package-Sensor-PCBs/populated%20TMP117%20sensor%20breakout%20PCB.jpg)

Board with TMP117A sensor before and after coating with epoxy resin

![Sensor before and after coating with epoxy resin](https://github.com/Ekkehard-Schulze/precision-temperature-logger/blob/main/hardware_sensor_breakout_boards/TMP117-WSON-package-Sensor-PCBs/Sensor_with_wires_and_plug.jpg)

Sensor board with polytetrafluoroethylene isolated wires and plug

![Sensor before and after coating with epoxy resin](https://github.com/Ekkehard-Schulze/precision-temperature-logger/blob/main/hardware_sensor_breakout_boards/TMP117-WSON-package-Sensor-PCBs/Logger_with_Sensor.jpg)

Logger with sensor attached

![Sensor before and after coating with epoxy resin](https://github.com/Ekkehard-Schulze/precision-temperature-logger/blob/main/hardware_sensor_breakout_boards/TMP117_in_CyA.JPG)

TMP117A sensor soldered to wires and coated with cyanoacrylate after years of lab use. 
Avoid outdoor use of cyanoacrylate polymer (CyA) coated sensors; UV light deteriorates CyA.