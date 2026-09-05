Highly accurate temperature logger for laboratory use
========================================================
Logs data and timestamps directly to an SD card. The package includes hardware design files, firmware, and a data analysis script. Our loggers have been reliably used in diverse laboratory applications for years. 

Features
--------
- Runs for months on a single 18650 Li-ion cell without recharging

- Up to four TMP117A sensors provide NIST-traceable 
accuracy not worse than ±0.1&nbsp;°C from –20&nbsp;°C to +50&nbsp;°C.

- The firmware automatically detects sensors and supports all combinations of the four dedicated I2C bus addresses.

- Writes a tab separated value formatted text file with 
ISO 8601 date and time to a micro-SD card. 
This format is compatible with python's pandas 
and plotly packages as well as with spreadsheet processing. 

- The user can specify the logging interval in settings.txt on the SD card. 

- A custom printed circuit board integrates an 18650 lithium-ion 
 battery, an accurate DS3231SN clock, the 'Adafruit Feather 32u4 Adalogger #2795' 
 and up to two on-board TMP117A sensors for environmental 
temperature monitoring in incubators or lab rooms.


- Four 7 x 5 mm sized custom printed breakout circuit boards for wire-attached 
TMP117A sensors. These act as probes in laboratory applications, 
for example, for usage in agar plates or liquid samples.

- An alternative firmware supports sensor types for an extended temperature range, for logging
 humidity, atmospheric pressure, infrared non-contact temperature and illuminance, see below. 

Notes
-----

1. For measurements in wet, salty or corrosive environments you
must cover all exposed sensor board conductive surfaces with a high 
quality polyepoxide (cured epoxy resin).

2. The main printed circuit board must be coated for water resistance if used in humid conditions. 
We applied the acrylic protective lacquer "PLASTIK 70" from https://www.kontaktchemie.com to operate the device in a 2&nbsp;°C incubator. 

2. The logger can be operated at temperatures between -20&nbsp;°C to +50&nbsp;°C.

2. Charging must be done between 0&nbsp;°C to 45&nbsp;°C, best at room temperature.

2. Replace R0 on the Adafruit Feather 32u4 Adalogger 2795 with a 2.2k SMD resistor for a 400 mA charging current; otherwise, it defaults to 100 mA, taking four times longer to charge. 

2. TMP117 product versions TMP117N and TMP117M are slightly 
less accurate than TMP117A.

2. The DS3231SN real-time clock may drift up to 63 seconds per year.

2. The alternative DS3231M real-time clock version is less accurate but more vibration-resistant. It may drift 
up to 158 seconds per year.

2. SD card types vary widely in their standby current. For a longer battery life, 
select an SD card with a low standby current.

3. For a quick start instead of using our project's printed circuit 
board and case you can use the 'Adafruit Feather 32u4 Adalogger #2795' 
with an 'Adafruit DS3231 Precision RTC FeatherWing #3028' and power it 
with a pluggable battery. Some TMP117 breakout boards are commercially 
available (e. g. Adafruit #4821 or SparkFun #15805), but these have a much 
larger size than our sensor boards and are more difficult to seal for use
in wet and corrosive environments. You may also consider Adafruit #6482, which is 
a breakout board for TMP119 (see below).

4. Our firmware also supports the 'SparkFun OpenLog DEV-13712' board with modifications.
 An electronic technician must breakout the I2C-bus and INT0, re-wire one LED, add I2C and 1-Wire bus pull-up resistors, a voltage divider, a DS3231 clock, and TMP117 sensors. These modifications require trace cutting and soldering under a microscope.

5. To meet alternative research requirements, we offer a specialized firmware version that supports a wider range of sensor types:
    * TMP117 and TMP119 (accuracy not worse than ±0.1&nbsp;°C from –20&nbsp;°C to +50&nbsp;°C)
    * ADT7420 (±0.20 °C accuracy from −10°C to +85 °C)		
    * 1-Wire temperature, this allows for a larger number of sensors on a several meters long two-conductor cable
      * DS18B20 (±0.5 °C accuracy from -10°C to +85 °C)
      * DS18S20 (±0.5 °C accuracy from -10°C to +85 °C, obsolete)
      * MAX31850 for type K thermocouples allows measurements from -30 °C to +600 °C. The optional correction according to the ITS-90 standard extends this range from -200 °C to +1200 °C.

    * BME280 (humidity, atmospheric pressure, less accurate temperature)

    * MLX90614 (infrared non-contact temperature)

    * TSL2561 (illuminance)
	


    This firmware is more complex and requires you to select sensors by uncommenting "#define" statements in the C source code. While the chosen sensors are then automatically detected and different sensor types can be mixed, memory limitations mean that only certain subsets of options can be implemented together.

13. You may like to try the newer and software compatible sensor type TMP119.
TMP119 has a higher accuracy (±0.03&nbsp;°C (typical) from 0&nbsp;°C to 45&nbsp;°C
and ±0.08&nbsp;°C (maximum) from 0&nbsp;°C to 45&nbsp;°C), but is only available in the 
YBG0006 'Die Size Ball Grid Array' package, which doesn't match our boards layout.
Remarkably, the die size of TMP119 is just 1.5&nbsp;mm x 1.0&nbsp;mm. This allows laboratory applications,
for which the 2.1&nbsp;mm x 2.1&nbsp;mm sized WSON package size is too large. A breakout board is available as 
Adafruit #6482, or, as https://www.pcbway.com/project/shareproject/[FPCB_version]_Precision_thermometer_humidity_sensor_with_digital_I2C_interfac_736a557b.html.




Contributions
-------------

This device was developed by the department of Bioinformatics and Molecular Genetics
('The Baumeister Lab'), Institute for Biology III, Faculty of Biology, 
University of Freiburg, Schänzlestraße 1, 79104 Freiburg, Germany
in collaboration with the institute's electronics workshop 'AT-Technik'.
The hardware was designed by Jürgen Schmidt and constructed by 
Joshua Schwarber. Jürgen Schmidt wrote an initial low-power voltage 
and temperature logging firmware, which was extended to the present highly accurate temperature 
logging firmware by Ekkehard Schulze. Ekkehard Schulze tested 
various temperature sensors types and identified the 
Texas Instruments sensor TMP117A as the only type satisfying 
our experimental requirements in the temperature range close to 0&nbsp;°C.


Images
-------

![Sensor before and after coating with epoxy resin](https://github.com/Ekkehard-Schulze/precision-temperature-logger/blob/main/hardware_sensor_breakout_boards/TMP117-WSON-package-Sensor-PCBs/populated%20TMP117%20sensor%20breakout%20PCB.jpg)

TMP117A sensor board before and after coating with epoxy resin.
\
\
\
![Sensor before and after coating with epoxy resin](https://github.com/Ekkehard-Schulze/precision-temperature-logger/blob/main/hardware_sensor_breakout_boards/TMP117-WSON-package-Sensor-PCBs/Sensor_with_wires_and_plug.jpg)

Sensor board with polytetrafluoroethylene isolated wires and plug.
\
\
\
![Sensor before and after coating with epoxy resin](https://github.com/Ekkehard-Schulze/precision-temperature-logger/blob/main/hardware_sensor_breakout_boards/TMP117-WSON-package-Sensor-PCBs/Logger_with_Sensor.jpg)

Logger with external sensor attached via cable. A further sensor is present on the main board. The scale is cm/mm.
\
\
\
![Sensor before and after coating with epoxy resin](https://github.com/Ekkehard-Schulze/precision-temperature-logger/blob/main/hardware_sensor_breakout_boards/TMP117_in_CyA.JPG)

TMP117A sensor soldered to wires and coated with polymerized cyanoacrylate after years of lab use. This mounting
was performed to minimize the size of the probe. Avoid outdoor use of cyanoacrylate polymer coated sensors; enduring UV light 
deteriorates it. The scale is mm.
\
\
\
![Openlog adaption](https://github.com/Ekkehard-Schulze/precision-temperature-logger/blob/main/images/Openlog_adaption.jpg)

Adaption of an Openlog board (SparkFun DEV-13712) for highly accurate temperature logging using an improvised  I2C-bus board and a DS3231 clock module (ZS-042). LED, EEPROM and the battery charging circuit were removed from the clock board for power optimization. This setup represents the most budget-friendly hardware configuration supported by the firmware. 
 To build the binary for this hardware, ensure `#define OPENLOG` is active and `#define ADAFRUIT_FEATHER_LOGGER` is commented out in the C source file.
\
\
\
![Openlog adaption](https://github.com/Ekkehard-Schulze/precision-temperature-logger/blob/main/images/Openlog_adaption_2.jpg)

However, adaption of an Openlog board for for this firmware requires microscope soldering and trace cutting. See point 11 in the __Notes__ section above.
\
\
\
![plots_and_statistics_of_time_series](https://github.com/Ekkehard-Schulze/precision-temperature-logger/blob/main/images/plots_and_statistics_of_time_series.py_screenshot.webp)

![plots_and_statistics_of_time_series](https://github.com/Ekkehard-Schulze/precision-temperature-logger/blob/main/images/plots_and_statistics_of_time_series.py_screenshot_2.webp)

Screenshots of interactive data analysis using the script "plots_and_statistics_of_time_series.py".
