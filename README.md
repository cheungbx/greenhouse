# greenhouse

/*
 Author: Billy Cheung 
 
 

 
 Function:  This project is to create a mini IoT  green house for growing plants using either the hydroponics system or the aeroponics system.
            The ESP32 collect data form sensors and display them on the I2C driven OLED SSD1306
            Following is the list of sensors used.
            H1750FVI -Lux sensor  - lghting level of grow light using sensor 
   
            homemade two pone sensor - EC (electric connectivity to assess neutrient concentration and water level), using a  with algorthims provded by 
            MR. Michael Ratcliffe  Mike@MichaelRatcliffe.com. All credits of this EC sensor should be given to Mr. Michael Ratchliffe. Please reference his blog for details.
            https://www.element14.com/community/community/design-challenges/vertical-farming/blog/2015/09/03/automated-green-house-blog9--three-dollar-ec-ppm-meter-for-mcu
            DS18B20 Waterproof Temperature Sensor used to calculste the equivalet EC at 25 Celius form the measured EC.
            
            DHT22 - for Air Temp and Humidity.
            
            The ESP32 controls the LED grow light, and neutrient supply pump through a relay module.
            The LED and pump are powered by 5V Dc and inserted to the two USB sockets driven by the relay module that is in turn controlled by the ESP32.
            
            Growlight on and off hours and minute  within a day  can be defined in the variables : LedOnHour, LedOnMin, LedOffHour, LedOffMin.
            Pump on and off hours and minutes within a day can be defined in the variables: PumpOnHour, PumpOnMinm, PumpOffHour, PumpOffMin.
            With in the Pump on and off times, the pump will cycle through an on perid, and off period, say turn on the pump once an hour for 1 min.
            For aeroponics, this feature can be used to control how often the neutrient pump is turned on to supply neutrients to the roots of the plant.
            For hydroponics , this feature may not be required. As you may want to turn the Air pump on all the time for Aeration.

            There are two buttons to for manual control of grow light and Pump.
            Switch your LED grow lights on or off either by pressing a physical button, or toggle the virtual button on MQTT Dashboard on 
            the Adafruit.IO web page or other equivalent cell phone MQTT client software.
            The Adafruit.IO dashboard collects the sensor eepromValues and display them overtime on the Dashboard.
            Adafruit has free of charge services for max. 10 feeds and not more than 30 data points per minute form all your sensors in a single account.
            To avoid overrruning the threshold, only changed eepromValues are publised every 10 seconds.
            
 
*/



/*
 Detailed instructions for beginners - Billy Cheung 2019-07-16
======================================================================
Hardware set up
----------------

The ESP32 Board
----------------
Get the ESP32 NodeMCU D1 mini with the pins pre-soldered. Otherwise, solder the pins on first.
Insert the NodeMCon onto the breadboard.
Then make the following connections


Humidity and air temperature sensor
------------------------------------
Connect the DHT22 humidity and air temperature sensor  according to the following pin layout.
*** WARNING *** Do not swap VCC (+3V) and Ground (0V), otherwised, the chip will be burnt very quickly.
    ___________
    |  DHT22  |
    |  front  |
    |  with   |
    |  holes  |
    ___________
      1 2 3 4
      V D N G
      C A C N
      C T   D
        A

Pin 1-VCC connect to the 3V/VCC of the ESP32 board
pin 2-Data connect to GPIO 19
Pin 4-GND connect to GND (ground) of the ESP32 board.
Connect your USB cable from your computer to the ESP32 board. Make sure you use a good cable. 
Many charging only cable do not have the data pins. 
Long USB cables > 1.5M  may  not work as the data signals dies out due to the long distance.

Push buttons
------------
Connect one leg of the push buttons to GND of ESP32.
Connect the other leg of the push button for the LED switch to GPIO 27
Connect the other leg of the push button to the pump switch to GPIO 25

Water Temperature Sensor
-------------------------
Connect the water temp sensor to pin 18. and connect a pull up resistor of 1K from pin 18 to +3V of the ESP 32.
Connect the +ve terminal of the water temp sensor to +3V of the ESP 32.
Connect the -ve terminal of the water temp sensor to GND of the ESP 32.

EC (electrical conductivity) sensor
-----------------------------------
Connect one leg of the self made two ponge plug for the EC (electrical conductivity) sensor to GPIO 36.
Connect the otherleg of the self made two ponge plug for the EC (electrical conductivity) sensor to GPIO 23.
connect a 470 ohm resistor between GPIO36 and GPIO32.


I2C OLED SSD1306  and I2C light sensor (lux meter) BH1750 
----------------------------------------------------------
  Connect VCC of the OLED to 3V of ESP32
  Connect GND of the OLED to GND of ESP32
  Connect SDA of the OLED to GPIO21 of ESP32
  Connect SCK of the OLED to GPIO22 of ESP32
  
  Repeat the same connections for the I2C LIght Sensor BH1750
  

Relay switch module (for two switches)
---------------------------------------
Connect 5V terminal of the Relay switch module to the +5V of the ESP32
Connect GND terminal of the Relay switch module to the GND of the ESP32
Connect relay one control pin of the relay module (that controls the LED) to GPIO 16 of the ESP32
Connect relay two control pin of the relay module (that controls the Pump) to GPIO 17 of the ESP32

Summary of all the GPIO pins used
-----------------------------------
#define         LedPin          16        // to control the LED grow light
#define         PumpPin         17        // to control the Nutrient water pump or Air Pump or Fogger/mist maker
#define         DHTPIN          19        // humidity and air temperature sensor
#define         ONE_WIRE_BUS    18        // water temperature sensor
#define         EcPin           36        // EC Meter, GPIO 36 SVP
#define         EcPwrPin        32        // +Ve power for EC meter, only turned on during measurement to avoid corrsision of probe. Measures > 5 sec apart.
#define         EcGndPin        23        // -Ve power for EC meter, only turned on during measurement to avoid corrsision of probe. Measures > 5 sec apart.
#define         Btn2Pin         25        // manual push button to switch the pump on/off
#define         Btn1Pin         27        // manual push button to switch the LED growlight  on/off
#define         i2cSDA          21        // I2C interface shared by both OLED and light meter
#define         i2cSCL          22        // I2C interface shared by  both OLED and light meter


 


Optionally, if you want to display the status on an SSD1306 I2C OLED.
Connect the the Vcc of the OLED to +3V (VCC) of the ESP32
Connect the GND of the OLED  to the GND of the ESP32
Connect the SDA of the OLED to GPIO GPI21  of the ESP32
Connect the SCL of the OLED to GPIO GPI22  of the ESP32


Software set up
---------------
This source can be found in  this github location.

https://github.com/cheungbx/greenhouse

Download the zip file into your computer.
Then unzip the file to the Arduino folder of your computer.
If you are using a MAC computer, then this will be your Documents\Arduino folder.
A folder called fridgemon will be created.
Then you will see two files
fridgemon.ino - this file
Credentials.h - the file that holds all WIFI and MQTT ids and passwords.

For Adruino IDE set up
-----------------------
If this is your first time using Arduino IDE, download the Arduino software from this url:  https://www.arduino.cc/en/main/software
Windows, Mac and Linux client are supported.


Start Adruino IDE
Then click the menu.

Adruino->Preference->"Additional Boards Manager Url:", then input http://arduino.ESP32.com/stable/package_ESP32com_index.json
Then exit Arduino and restart to take in the preference.

Reopen Arduino IDE
Tools->Borad:Atmega... ->Boards Manager
Input ESP32 to search and find the matching board drivers for ESP32
Click Install to install.

Tools->Borad->Wemos LOLIN 32 (Appear under the section for ESP32, will not show up unless you did the preference setting above)
Tools->Port:->"/dev/cu.SLAB_USBtoUART"  - select your serail port used to connect to the ESP32, if nothing shows up, check your cable or your driver for that USB-Serail port.
                                          Most ESP32 boards used the SIL 2104 driver for USB driver.

File->Open-> open this sketch file (main program source).
Update the credentials.h with your home wifi ssid and passwords
If you already have an account on adafruit.io, put in the id and passwords you used to login to Adafruit.io
and the AIO key.

Otherwise, you need to first register at Adafruit.io to get a free account.
Mr. Andreas Spiess has created a very good youtube video about how to set up your own MQTT account on Adafruit.io.
Please view this youtube video.  #48 Connect ESP32 with the world (and IFTT) through MQTT and Adafruit.io (Tutorial)
https://www.youtube.com/watch?v=9G-nMGcELG8

Once you have the MQTT account on adafruit.io
Set up your dashboards and add the follwoing feeds.
lux
ec
airtemp
humidity
led
ledonhour
ledoffhour
pump
pupmonmin
pupoffmin

Then create the dashboard
and add blocks to dashboard to reprsent the charts or buttons.
You can put multiple relevant feeds onto the same chart if the values are not too far apart,
e.g. humidity, air temp, water temp on one chart.
lux on it's own chart (values can be 10,000)
ec on it's own chart from 0 to 3.0.

Then add buttons for 
led - ON  / OFF
pump - ON / OFF
ledonhour - slide bar 00 - 23 hour
ledoffhour - slide bar 00 - 23 hour
pupmonmin - slide bar 0 - 720 minutes
pumpoffmin - slide bar 0 - 720 minutes


Then update the  "credential.h" files with your home wifi SSID and passwords as well as your adafruit.io account userid and AIO key , and the full path of the feeds to the credential.h file.

e,g, 

#define  WIFIssid        "mywifissid"
#define  WIFIpassword     "mywifipassword"

#define MQTT_USERNAME   "userid"                              - userid is your id to login to adafruit.io account.  
#define MQTT_KEY        "9a795532c0874a43a5486e0c8a3919c2"    - can be obtained by logging into your adafruit.io account, click View AIO Key.
#define SERVER          "io.adafruit.com"    
#define SERVERPORT      1883 
#define MQTT_LUX         "userid/feeds/lux"                   - replace all "userid" with your id to login to adafruit.io account.
#define MQTT_EC          "userid/feeds/ec"
#define MQTT_AirTemp     "userid/feeds/airtemp"
#define MQTT_Humidity    "userid/feeds/humidity"
#define MQTT_Led         "userid/feeds/led"
#define MQTT_LedOnHour   "userid/feeds/ledonhour"
#define MQTT_LedOffHour  "userid/feeds/ledoffhour"
#define MQTT_Pump        "userid/feeds/pump"
#define MQTT_PumpOnInterval   "userid/feeds/pumponmin"
#define MQTT_PumpOffInterval  "userid/feeds/pumpoffmin"
#define MQTT_Upgrade     "userid/feeds/upgrade"


Click tools->Serial Monitor. Set the serial speed to "115200 baud" to  match with this program.
This will allow you to see all the diagnosis messages from the program once the compile and upload is done.
Resize the serial monitor window size and the Arduino IDE size so you can see both on your screen.
On the to right of the Arduino IDE window where the source code is displayed, click "->" to compile and upload.
If you get file not found on files like "??????.h" e.g. NTPClient.h do the following:
Sketch->Include Library->Manage Libraries
at the search box type in the name of the file without the extension, e.g. NTPClient
Select to install the most matching library.
Sometimes you cannot find these missing libraries within the Adruino IDE.
Then you need to search in google and download them fro Github as zip files.
Then click Sketch->Include Library->Add .Zip libraries   and open these zip files to add to the Aruindo IDE.
Then recompile by clicking "->" to compile and upload.

Repeat this process until all missing library files have been instlaled.

Once the program is uploaded, the board will be reset, and you will see diagnosis messages on the serail monitor.
Check that WIFI is connected successfully. If no, check the SSID nad password you put into credentials.h.
You can click the reset button on the ESP32 board to reboot any time.
Or you can adjust the source code and clieck "->" to recompile and upload again.

Once wifi is successfully connected, you will see the message like the one below
Connecting to: yourwifissid
...........WiFi connected at: 192.168.1.113


Then you can login to your MQTT account in adafruit.io to view the dashboard and the feeds.
And create charts for the feeds and buttons for the light switch and upgrade.
Then test the other feature

*/
