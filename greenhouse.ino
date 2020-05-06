
/*
 Author: Billy Cheung\
 
 

 
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


 `  `


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


// uncomment this line for debug mode to print lots of debug messages using Serial.print
#define DEBUG 1

// for i2c OLED 128 x 64

#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
// Include the UI li
#include "OLEDDisplayUi.h"

#include <OneWire.h>
#include <DallasTemperature.h>

#include <EEPROM.h> //Standard EEPROM Library
#define eepromSize 12
#define addrToken 0
#define addrLedOnHour 2
#define addrLedOffHour 4
#define addrPumpOnInterval 6
#define addrPumpOffInterval 8
#define magicToken 123
int eepromValue;
bool eepromCommit = false;

//for MQTT messaging
#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "credentials.h"
#include <ArduinoOTA.h>

WiFiClient WiFiClient;
// create MQTT object
PubSubClient client(WiFiClient);

bool OTA_mode = false;
unsigned long OTA_timer;


#include <BH1750FVI.h>

#include "time.h"

char dateTimeText[20];


#include <Adafruit_Sensor.h>


#include <DHT.h>
float airTemp, humidity, lastAirTemp, lastHumidity;

    
char valueStr[9];

String s1, h1;

const char* ntpServer = "hk.pool.ntp.org";   // if you do not know your local NT server, use "pool.ntp.org"
const long  gmtOffset_sec = 8 * 3600;        // mini sec
const int   daylightOffset_sec = 0;          // mini sec


// Summary of all the GPIO pins used
// ------------------------------------
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

// Initialize the OLED display using Wire library


SSD1306Wire  display(0x3c, i2cSDA, i2cSCL);  // OLED's I2c device address, sda pin, scl pin

OLEDDisplayUi ui ( &display );


#define DHTTYPE    DHT11
// #define DHTTYPE    DHT22

//    Connect the DHT11 or DHT22  according to the following pin layout.
//    *** WARNING *** Do not swap VCC (+3V) and Ground (0V), otherwised, the chip will be burnt very quickly.
//    ___________
//    |  DHT11  |
//    |    or   |
//    |  DHT22  |
//    |  front  |
//    |  with   |
//    |  holes  |
//    ___________
//      1 2 3 4
//      V D N G
//      C A C N
//      C T   D
//        A

DHT dht(DHTPIN, DHTTYPE);

uint16_t lux, lastLux;



//##################################################################################
//-----------  EC 
//##################################################################################


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
 
 
float waterTemp, lastWaterTemp;
float ECraw=0;
float EC =0, lastEC;

 
 
float raw= 0;
float Vin= 3.3;
float Vdrop= 0;
float Rc= 0;

float CalibrationEC=1.33; 
uint16_t Ec1;
float waterTempCoef = 0.019;
float K=2.8;   // calibration factor for EC meter
int R1= 470;
int Ra=25; //Resitance of Digital Pin, 25 ohms for ESP32


uint16_t water1; 

int Readings=0;
 
 
  // Create the Lightsensor instance
BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);
 
struct tm* timeNow;

uint16_t  LedOnHour = 07;
uint16_t  LedOnMin  = 00;
uint16_t  LedOnMinutes;

uint16_t  LedOffHour= 23;
uint16_t  LedOffMin = 00;
uint16_t  LedOffMinutes;

uint16_t  PumpOnHour  = 0;
uint16_t  PumpOnMin  =  0;
uint16_t  PumpOnMinutes;


uint16_t  PumpOffHour = 23;
uint16_t  PumpOffMin = 59;
uint16_t  PumpOffMinutes;


int currMin;


bool LedOn = true, lastLedOn = false; 
bool PumpOn = true, lastPumpOn = false;
int8_t LedProg = 0, LastLedProg = -1, PumpProg = 0, lastPumpProg = -1;

const long timingFactor = 60000; // unit of interval 1000 = seconds 60000 = minutes
uint16_t  PumpOffInterval  = 60;  
uint16_t  PumpOnInterval  = 1; 
uint16_t  pumpRemainingInterval = 0; 
long PumpTimeNow;
long PumpActionTime = 0;  


// These variables are for Btn1 debouncing
int reading1, reading2;
int Btn1State = 1, LastBtn1State = 1;             // the current reading from the input pin
int Btn2State = 1, LastBtn2State = 1;             // the current reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastBtn1Debounce = 0;  // the last time the output pin was toggled
unsigned long lastBtn2Debounce = 0;  // the last time the output pin was toggled
const unsigned long debounceDelay = 30;    // the debounce time; increase if the output flickers

unsigned long lastDisplayTime = 0;  // the last time the LCD display happened.
const unsigned long displayDelay = 1000;    // the display delay time; increase if the output flickers

unsigned long lastECTime = 0;  // the last time the LCD display happened.
const unsigned long ECDelay = 10000;    // the display delay time;

unsigned long lastLuxTime = 0;  // the last time the LCD display happened.
const unsigned long LuxDelay = 1000;    // the display delay time;

unsigned long lastHumidityTime = 0;  // the last time the LCD display happened.
const unsigned long HumidityDelay = 10000;    // the display delay time;


bool  reconnect_MQTT()
{

   
   if (!client.connected()) 
   {
#ifdef DEBUG 
    Serial.println("Attempting MQTT connection...");
#endif
    // Attempt to connect
    if (client.connect("", MQTT_USERNAME, MQTT_KEY)) {
#ifdef DEBUG 
      Serial.print("connected using account name: ");
      Serial.println (MQTT_USERNAME); 
#endif
      // ... and resubscribe

      client.subscribe(MQTT_Led, 1);
      delay (3000);
      client.subscribe(MQTT_Pump, 1);
      delay (1000);
      client.subscribe(MQTT_LedOnHour, 1);
      delay (1000);
      client.subscribe(MQTT_LedOffHour, 1);
      delay (1000);
      client.subscribe(MQTT_PumpOnInterval, 1);
      delay (1000);
      client.subscribe(MQTT_PumpOffInterval, 1);
      delay (1000);

      return true;


     } else {
#ifdef DEBUG 
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
#endif      
      return false;
    }
  }
}

String mmdd (int digits)
{
  if (digits < 10)  return ("0" + String (digits)); else return String (digits);
}  


void scan_i2c()
{
 int j;

int error,  nDevices = 0;
int i2c_addr[] = {0,0,0,0,0,0,0,0};

Serial.println("Scanning...");
 
  for (j = 1; j < 127; j++)
  {
    // The i2c scanner uses the return eepromValue of Write.endTransmisstion to see if a device did acknowledge to the address.
    Wire.beginTransmission(j);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
        if (j < 16) {
        Serial.print("0");
      }
      Serial.print(j, HEX);
      Serial.println(" !");
      i2c_addr[nDevices]=j;
      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (j < 16) {
        Serial.print("0");
      }
      Serial.println(j, HEX);
    }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.print (nDevices);
    Serial.println (" I2C devices found");   
  }
}

void setup()
{
    Serial.begin(115200);

    EEPROM.begin(eepromSize);
    if (EEPROM.read(addrToken) != magicToken) {
      // Record default eepromValues to EEPORM.
      Serial.println ("Token not found in EEporm, recreating defaults in EEPORM");
      EEPROM.put(addrToken, magicToken);
      EEPROM.put(addrLedOnHour, LedOnHour);
      EEPROM.put(addrLedOffHour, LedOffHour);
      EEPROM.put(addrPumpOnInterval, PumpOnInterval);
      EEPROM.put(addrPumpOffInterval, PumpOffInterval);
      EEPROM.commit();
    
      }
    else {
      EEPROM.get(addrLedOnHour,LedOnHour);  
      EEPROM.get(addrLedOffHour,LedOffHour); 
      EEPROM.get(addrPumpOnInterval,PumpOnInterval); 
      EEPROM.get(addrPumpOffInterval, PumpOffInterval); 
      }

    Serial.println("AutoPonics v1.0");
    Serial.println( "LED " + mmdd(LedOnHour)+":"+mmdd(LedOnMin) + "-" + mmdd(LedOffHour) + ":" + mmdd(LedOffMin));
    Serial.println( "Pump cycle " + String(PumpOnInterval)+"/"+String(PumpOffInterval));
    Serial.println(  mmdd(PumpOnHour)+":"+mmdd(PumpOnMin)+"-"+mmdd(PumpOffHour)+":"+mmdd(PumpOffMin));
 
    
    display.init();
    display.clear();
    display.flipScreenVertically();
 
    display.setTextAlignment(TEXT_ALIGN_LEFT);

    display.clear();
//  display.setFont(ArialMT_Plain_10);
    display.setFont(ArialMT_Plain_16);

    display.drawString(0, 0, "AutoPonics v1.0");

    display.drawString(0, 16, "LED " + mmdd(LedOnHour)+":"+mmdd(LedOnMin) + "-" + mmdd(LedOffHour) + ":" + mmdd(LedOffMin));
    display.drawString(0, 32, "Pump cycle " + String(PumpOnInterval)+"/"+String(PumpOffInterval));
    display.drawString(0, 48, "    " + mmdd(PumpOnHour)+":"+mmdd(PumpOnMin)+"-"+mmdd(PumpOffHour)+":"+mmdd(PumpOffMin));
 
    display.display();

   LedOnMinutes =  LedOnHour * 60 + LedOnMin;
   LedOffMinutes = LedOffHour * 60 + LedOffMin;
   PumpOnMinutes =  PumpOnHour * 60 + PumpOnMin;
   PumpOffMinutes = PumpOffHour * 60 + PumpOffMin;
  
  

  sensors.begin(); // under water temp sensor.
  
  LightSensor.begin();  // Lux Meter

  dht.begin(); // Air Temperature and Humidity Sensor
  

  pinMode (Btn1Pin, INPUT_PULLUP); 
  pinMode (Btn2Pin,INPUT_PULLUP); 

  pinMode (LedPin, OUTPUT);
  pinMode (PumpPin, OUTPUT);
  pinMode (EcPwrPin, OUTPUT);
  pinMode (EcGndPin, OUTPUT);   
  digitalWrite(EcGndPin, LOW);
  digitalWrite(LedPin, HIGH);
  digitalWrite(PumpPin, HIGH);

  delay(5000);

  //connect to WiFi
  Serial.printf("Connecting to %s ", WIFIssid);
  WiFi.begin(WIFIssid, WIFIpassword);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");    
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


    display.clear();
    Serial.println("WIFI on");
    display.drawString(0, 0, "AutoPonics v1.0");

    display.drawString(0, 16, "WIFI on");
    display.display();


//init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

   
//original setup code starts here
  client.setServer(SERVER, SERVERPORT);
  
  client.setCallback(callback);
  Serial.println();



if (reconnect_MQTT()) {
    Serial.println ("MQTT on");    
    display.drawString(0, 48, "MQTT on");
    display.display();  
} 
else {
  
    Serial.println ("MQTT Err!");    
    display.drawString(0, 48, "MQTT Err!");
    display.display();  }



  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to ESP32-[ChipID]
  // ArduinoOTA.setHostname("myESP32");

  // No authentication by default
   ArduinoOTA.setPassword(OTA_password);

  // Password can be set with it's md5 eepromValue as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
#ifdef DEBUG  
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
#endif 
  });
  
  ArduinoOTA.onEnd([]() {
#ifdef DEBUG  
    Serial.println("\nEnd");
#endif   
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
#ifdef DEBUG
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
#endif
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
 #ifdef DEBUG  
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");

    }
#endif
  });
  
  ArduinoOTA.begin();
#ifdef DEBUG  
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
#endif


delay (3000);

}



void callback(char* topic, byte * data, unsigned int length) {
  // handle message arrived {

  String s1 = (String)topic;
  String s2 = "";
#ifdef DEBUG  
  Serial.print("MQTT Received - ");
  Serial.print(topic);
  Serial.print(":");
  for (int i = 0; i < length; i++) {
   s2 = s2 + ((char)data[i]);
  }
  Serial.println(s2);
#endif

 
  if  (s1 == MQTT_Led)
   {

// Test if  the data is "OFF"
    if (data[1] == 'F')  {     
      LedOn= false;
 
     } else {
      LedOn= true;


    }
   }
else if  (s1 == MQTT_Pump)
   {

// Test if  the data is "OFF"
    if (data[1] == 'F')  {     
      PumpOn= false;

     } else {
      PumpOn= true;


    }
   }

  else if (s1 == MQTT_Upgrade) {
    if (data[1] == 'N')  {    
     OTA_mode = true; 

#ifdef DEBUG      
    Serial.println ("Turned ON code upgrade mode.");
#endif
    }
   else if (data[1] == 'F')  {   
         OTA_mode = false; 
#ifdef DEBUG      
    Serial.println ("Turned OFF code upgrade mode..");
#endif
   }

  }
  else if (s1 == MQTT_LedOnHour) {
   LedOnHour = s2.toInt(); 
   Serial.println ("Led On Hour " + String (LedOnHour));
   EEPROM.put(addrLedOnHour, LedOnHour); 
   EEPROM.commit();
   LedOnMinutes =  LedOnHour * 60 + LedOnMin;

  
   }
  else if (s1 == MQTT_LedOffHour) {
   LedOffHour = s2.toInt(); 
   Serial.println ("Led Off Hour " + String (LedOnHour));
   EEPROM.put(addrLedOffHour, LedOffHour); 
   EEPROM.commit();
   LedOffMinutes = LedOffHour * 60 + LedOffMin;

   }

   else if (s1 == MQTT_PumpOnInterval) {
   PumpOnInterval = s2.toInt(); 
   Serial.println ("Pump On Interval " + String (PumpOnInterval));
   EEPROM.put(addrPumpOnInterval, PumpOnInterval); 
   EEPROM.commit();
   PumpActionTime = 0; // clear wait time so new interval is effective immediately
   }
   else if (s1 == MQTT_PumpOffInterval) {
   PumpOffInterval = s2.toInt(); 
   Serial.println ("Pump Off Interval " + String (PumpOffInterval));
   EEPROM.put(addrPumpOffInterval, PumpOffInterval); 
   EEPROM.commit();
   PumpActionTime = 0; // clear wait time so new interval is effective immediately
   }

}


void readBtn1 ()
{
     // read the state of the Btn1 into a local variable:
   reading1 = digitalRead(Btn1Pin);
    
  // check to see if you just pressed the Btn1
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:


  if (reading1 != LastBtn1State) {
    // reset the debouncing timer
    lastBtn1Debounce = millis();
  }
  
  if (abs((millis() - lastBtn1Debounce) > debounceDelay)) 
   {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the Btn1 state has changed:
    if (reading1 != Btn1State) 
     {
      Btn1State = reading1;
 
     Serial.println(" Btn1State:" + String (Btn1State));
     
    //  Btn1 pressed = 1 as the input pin it pulled high when not pressed and pulled low when pressed.
    // check if Btn1 has been toggled,  i.e. from off (1) to on (0) to off (1).
      if (Btn1State == 1)    // remove this if statement if you are using a toggle switch instead of a press Btn1.
     { 
        if (lastLedOn)  LedOn=false; else LedOn=true;
     }
    }
   }

LastBtn1State = reading1; 
}


void readBtn2 ()
{
     // read the state of the Btn1 into a local variable:
   reading2 = digitalRead(Btn2Pin);
    
  // check to see if you just pressed the Btn1
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  if (reading2 != LastBtn2State) {
    // reset the debouncing timer
    lastBtn2Debounce = millis();
  }

 
  if (abs((millis() - lastBtn2Debounce) > debounceDelay)) 
   {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
 
    // if the Btn1 state has changed:
    if (reading2 != Btn2State) 
     {

      Btn2State = reading2;


      
    //  Btn1 pressed = 1 as the input pin it pulled high when not pressed and pulled low when pressed.
    // check if Btn1 has been toggled,  i.e. from off (1) to on (0) to off (1).
      if (Btn2State == 1)    // remove this if statement if you are using a toggle switch instead of a press Btn1.
     { 
        if (lastPumpOn)  PumpOn=false; else PumpOn=true;
     }
    }
   }

LastBtn2State = reading2; 
}


void loop()
{

// check if somoene switched on on the air firmware upgrade switch on the MQTT dashboard on the IO.adafruit web site.
// if so, perform on the air WIFI download if new firmware. The user will need to use the Adruino IDE and select the port as the IP address of the ESP32
// then click upload to program the ESO 8266 with new firmware codes within 1 minute. Otherwise, the program will resume the regular funciton and ignore the upgrade.
if (OTA_mode) {
   OTA_timer = 60;

   // handle WIFI code upload requests once at setup
   while (OTA_mode) {
    ArduinoOTA.handle();
    delay (1000); 
    OTA_timer --;
    s1 = String (OTA_timer) + " sec";

#ifdef lcdDisplay 
    display.clear();
    display.drawString(0, 0,  "OTA Upgrade");
    display.drawString(0, 24, "Count Down");
    display.drawString(0, 48, s1);
  display.display();   
#endif

#ifdef DEBUG 
    Serial.println ("Over the Air Upgrade countdown : " + s1);
#endif

   if (OTA_timer <= 0)  OTA_mode = false;
  
   }
}



  yield();

  if (! reconnect_MQTT()) {
   delay(5000);  // if failed to connect to MQTT, wait 5 seconds before retrying.
  }
  
  time_t now = time (nullptr);
  timeNow = localtime(&now);
  currMin = timeNow->tm_hour * 60  + timeNow->tm_min;
  
  

 
if (abs((millis() - lastLuxTime) > LuxDelay))  { 
  lastLuxTime =   millis();


  lux = LightSensor.GetLightIntensity() * 8;


  if (lastLux != lux) {
     lastLux = lux;
     h1 = (String) lux;
     h1.toCharArray(valueStr, 5);
     client.publish(MQTT_LUX, valueStr); 
     }
}
 
if (abs((millis() - lastECTime) > ECDelay))  { 
  lastECTime =   millis();
  sensors.requestTemperatures();// Send the command to get waterTemps
  waterTemp=sensors.getTempCByIndex(0); //Stores eepromValue in Variable

  digitalWrite(EcPwrPin, HIGH);
  raw = analogRead(EcPin);
  digitalWrite(EcPwrPin, LOW);
 
  //***************** Converts to EC **************************
  Vdrop= (Vin*raw)/4096.0;
  
  
  Rc=(Vdrop*(R1+Ra))/(Vin-Vdrop) - Ra;


  ECraw = 1000/(Rc*K);
 
 
  //*************Compensating For Temperaure********************
  EC   =  ECraw/ (1+ waterTempCoef*(waterTemp-25.0));
  

  
  if (lastEC != EC) {
     lastEC = EC;
     h1 = (String) EC;
     h1.toCharArray(valueStr, 5);
     client.publish(MQTT_EC, valueStr); 
     }

 
  if (lastWaterTemp != waterTemp) {
     lastWaterTemp = waterTemp;
     h1 = (String) waterTemp;
     h1.toCharArray(valueStr, 5);
 //    client.publish(MQTT_WaterTemp, valueStr); 
     }
  }
   
if (abs((millis() - lastHumidityTime) > HumidityDelay))  { 
  lastHumidityTime =   millis();
  airTemp = dht.readTemperature();  
  humidity = dht.readHumidity();
  
  // Read waterTemp as Fahrenheit (isFahrenheit = true)
  //float t = dht.readwaterTemp(true);

  
  if (lastAirTemp != airTemp) {
     lastAirTemp = airTemp;
     h1 = (String) airTemp;
     h1.toCharArray(valueStr, 5);
     client.publish(MQTT_AirTemp, valueStr); 
     }


  if (lastHumidity != humidity) {
     lastHumidity = humidity;
     h1 = (String) humidity;
     h1.toCharArray(valueStr, 5);
     client.publish(MQTT_Humidity, valueStr); 
     }
  }

  // only change LED status once

    if (currMin >= LedOnMinutes and currMin < LedOffMinutes) LedProg = 1; else LedProg = 0;
    
    if (LastLedProg != LedProg) {
       if (LedProg == 1) LedOn = true; else LedOn = false;
       LastLedProg = LedProg;
    }

  
  



  
  if (currMin  >= PumpOnMinutes  and currMin < PumpOffMinutes)
  {
    PumpTimeNow =   millis() / timingFactor;
    
    if (PumpTimeNow >= PumpActionTime) {
    
      if (lastPumpProg == 1) {     
          PumpActionTime = PumpTimeNow + PumpOffInterval;
          PumpProg = 0;
          PumpOn = false;
       }
      else {
         PumpActionTime = PumpTimeNow + PumpOnInterval;
  
         PumpProg = 1;
         PumpOn = true;
         }
      lastPumpProg = PumpProg;

    }
    pumpRemainingInterval = PumpActionTime - PumpTimeNow; 
   }   
      
  else
     pumpRemainingInterval = 0;
  

 if (abs((millis() - lastDisplayTime)) > displayDelay)  { 
  lastDisplayTime =   millis();
  
  display.clear();
  display.setFont(ArialMT_Plain_16);

  sprintf(dateTimeText, "%02d:%02d:%02d", timeNow->tm_hour,timeNow->tm_min,timeNow->tm_sec);
  display.drawString (0,0, dateTimeText);

  Serial.print ( dateTimeText);
  
  Serial.print ( " Left:" + String(digitalRead(Btn1Pin)) + " Right:"+ String(digitalRead(Btn2Pin))+" ");
  Serial.print("  EC: " + String(EC)+" ");
 
 
  if (EC == 0) {

       display.drawString(0, 32, "No Water!!" );

    
  }
  else {
  display.drawString(0, 16, "Lux:" + String(lux));
    
  Serial.print ("Lux:" + String(lux)+" ");  


  Serial.print ("Water Temp:"+String(waterTemp)+" ");
  display.drawString(80, 32, "C:"+String(waterTemp));
  Serial.print("  EC: " + String(EC)+" ");
  display.drawString(0, 32, "EC:" + String(EC));
   Serial.print ("Temp:"+String(airTemp)+" ");
   display.drawString(80, 16, "T:" + String(airTemp));
   Serial.println ("Humidity:"+String(humidity)+" ");
   display.drawString(80, 0, "H:" + String(humidity));


  if (LedOn) {
        Serial.print("  Light: ON  "); 
        display.drawString(0, 48, "Led on");

        }
  else {
  
        Serial.print("  Light: OFF ");
        display.drawString(0, 48, "Led off");
  }
  
  if (PumpOn) {
      display.drawString(68, 48, "P on"); 
      Serial.print(" Pump On");
   }
  else {  display.drawString(68, 48, "P off"); 
          Serial.print(" Pump Off");  

    }
    
  display.drawString(110, 48, String(pumpRemainingInterval));
  Serial.print (" " + String(pumpRemainingInterval));
  Serial.println();
  
    


  }
  display.display();
 }


  
 if (LedOn != lastLedOn) {
     if (LedOn) {
      digitalWrite(LedPin, LOW); 
      client.publish(MQTT_Led, "ON");
#ifdef DEBUG 
      Serial.print(MQTT_Led);
      Serial.println (" ON");
#endif     

      } 
    else {
     digitalWrite(LedPin, HIGH);
     client.publish(MQTT_Led, "OFF");
#ifdef DEBUG 
      Serial.print(MQTT_Led);
      Serial.println (" OFF");
#endif     
     }
   lastLedOn = LedOn;
 }

if (PumpOn != lastPumpOn) {
   if (PumpOn) {
    digitalWrite(PumpPin, LOW); 
    client.publish(MQTT_Pump, "ON");
#ifdef DEBUG 
      Serial.print(MQTT_Pump);
      Serial.println (" ON");
#endif
   }else  {
    digitalWrite(PumpPin, HIGH);
    client.publish(MQTT_Pump, "OFF");
#ifdef DEBUG 
      Serial.print(MQTT_Pump);
      Serial.println (" OFF");
#endif

   }
   lastPumpOn = PumpOn; 
}

  readBtn1();

  readBtn2();

  
client.loop();
}
