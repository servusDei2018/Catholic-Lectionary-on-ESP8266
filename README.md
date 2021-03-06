# Catholic-Lectionary-on-ESP8266
The Roman Catholic Lectionary, Liturgical Calendar and Feast Days on ESP8266 with Dalian/Waveshare 2.7" or 4.2" red/black SPI display, SPI-mode SD card, DS3231 clock IC and Nanjing Top Power TP5410 PMIC/boost IC, with support for 8 grey/red levels + antialiased text

<h2>Catholic Lectionary</h2>
<p>
This project implements a Calendar, Liturgical calendar, Lectionary and Feast Day (Sanctorale) calendar on the ESP8266. It also shows an indicator for Holy Days of Obligation.
It is based on the Ruby program calendarium-romanum (https://github.com/igneus/calendarium-romanum) which implements just the Liturgical calendar and Sanctorale days. As a result, it uses the some of the same configuration files and file formats as this program, with some modifications to support month names etc.
</p>

![Preview1](./lectionary5.jpg)
![Preview2](./lectionary3.jpg)
![Preview3](./lectionary7.png)

<h3>Localization</h3>
<p>
The project is localized for English, French, Czech and Latin, and other languages, Bible translations, Sanctorale days and Liturgical days can be added over time as all configuration data resides on the SD card.
Arabic ligaturization is supported, and in the SD card archive an Arabic Bible is available (which I was using to test Arabic ligaturization). However, the Sanctorale and Liturgical days are still shown in English with this Bible (but this can be modified if anyone would like to translate the underlying files into Arabic).
</p>

![Preview4](./lectionary6.jpg)

<h3>ToolChain</h3>
<p>
Several Toolchain projects are provided, written in C# and C++, one of which - TheDotFactory (https://github.com/pavius/the-dot-factory/tree/master/TheDotFactory) - is not mine, but I have modified it to output "disk fonts", which are bitmap fonts which can be used as an alternative to the built-in rom font used by default. (They may be useful for other character sets needing more detail and larger characters, such as Chinese or Arabic).
<p>

<h3>Debug Output</h3>
<p>
In order to maximize available pins on the ESP8266, the TX and RX pins are used as an I2C bus to control the DS3231 clock chip. So, to retain the ability to output backchannel debug text therefore an additional program is provided which can be compiled and uploaded to a separate 3.3v Arduino Leonardo, which can then be attached to the ESP8266 by connecting TX to SDA and RX to SCL. Debug output can then be enabled using ?debug=1 as one of the parameters to the setconf webpage served by the lectionary, eg. http://lectionary.local/setconf.htm?debug=1.
</p>

<h3>Hardware</h3>
The current printed circuit board schematic and design can be found at https://easyeda.com/plishman/lectionary_2-3

The current case design is at https://www.tinkercad.com/things/h8xbfqkVT7G

As well as the other Arduino libraries, this project uses the following libraries:
<ul>
  <li>GxEPD - https://github.com/ZinggJM/GxEPD (modified for 3bpp - 7 grey/red levels, plus white)</li>
  <li>Adafruit GFX - https://travis-ci.org/adafruit/Adafruit-GFX-Library</li>
  <li>brzo_i2c - https://github.com/pasko-zh/brzo_i2c</li>
  <li>LinkedList - https://github.com/ivanseidel/LinkedList</li>
</ul>

Modified versions of the Time and SPI arduino libraries are also used: The time library has been changed to use a 64-bit value to store the time in seconds since 1970 so that the 2038 bug will not occur. The SPI library has had an extra function added which permits 9-bit SPI data to be written, so that the WaveShare/Dalian display used can be operated on 3 wires instead of 4 (no need for a data/command wire, the d/c value is stored in the 9th bit of each byte transmitted to the screen, which saves a pin on the ESP8266).

Many thanks to the contributors of Calendarium-Romanum, TheDotFactory and all of the other libraries on which this project depends.
<p>
There is a lot to this project, and it has taken me over a year to develop the software and a board to run it on, so bear with me - I will update the readme as I go.
</p>
[This readme is still in the process of being updated]
