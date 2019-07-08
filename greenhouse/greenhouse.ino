
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
            The Adafruit.IO dashboard collects the sensor values and display them overtime on the Dashboard.
            Adafruit has free of charge services for max. 10 feeds and not more than 30 data points per minute form all your sensors in a single account.
            To avoid overrruning the threshold, only changed values are publised every 10 seconds.
            
 
*/


// uncomment this line for debug mode to print lots of debug messages using Serial.print
#define DEBUG 1

// for i2c OLED 128 x 64

#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
// Include the UI lib
#include "OLEDDisplayUi.h"

#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h> //Standard EEPROM Library

//for MQTT messaging
#include <PubSubClient.h>

// Initialize the OLED display using Wire library
// ESP32
// 21 -> SDA
// 22 -> SCL

SSD1306Wire  display(0x3c, 21, 22);  // OLED's I2c device address, sda pin, scl pin

OLEDDisplayUi ui ( &display );

#include <WiFi.h>
#include <WiFiUdp.h>
#include "wificredential.h"
#include <ArduinoOTA.h>

WiFiClient WiFiClient;
// create MQTT object
PubSubClient client(WiFiClient);

bool OTA_mode = false;
unsigned long OTA_timer;

/*
uses only 4 wires to test the BH1750 Light sensor
  
  Connecting the sensor to a NodeMCU ESP32
  VCC  <-> 3V3
  GND  <-> GND
  SDA  <-> D21
  SCL  <-> D22
*/


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


const uint8_t   LedPin        = 16 ;      // Grow Light
const uint8_t   PumpPin       = 17;       // Nutrient or Air Pump or Fogger
#define         DHTPIN          19
#define         ONE_WIRE_BUS    18
#define         EcPin           36        // EC Meter, GPIO 36 SVP
#define         EcPwrPin        32        // +Ve power for EC meter, only turned on during measurement to avoid corrsision of probe. Measures > 5 sec apart.
#define         EcGndPin        23        // -Ve power for EC meter, only turned on during measurement to avoid corrsision of probe. Measures > 5 sec apart.
#define         Btn2Pin         25
#define         Btn1Pin         27
  
 

// #define DHTTYPE    DHT11
#define DHTTYPE    DHT22

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
    // The i2c scanner uses the return value of Write.endTransmisstion to see if a device did acknowledge to the address.
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

#define addrToken 0
#define addrLedOnHour 1
#define addrLedOffHour 2
#define addrPumpOnInterval 3
#define addrPumpOffInterval 4

#define magicToken 123

byte value;
EEPROM.begin(12);
if (EEPROM.read(addrToken) != magicToken) {
  // Record default values to EEPORM.
  Serial.println ("Token not found in EEporm, recreating defaults in EEPORM");
  EEPROM.write(addrToken, magicToken);
  EEPROM.write(addrLedOnHour, LedOnHour);
  EEPROM.write(addrLedOffHour, LedOffHour);
  EEPROM.write(addrPumpOnInterval, PumpOnInterval);
  EEPROM.write(addrPumpOffInterval, PumpOffInterval);

}
else {
  value = EEPROM.read(addrLedOnHour);  if (value < 0 or value > 23)  EEPROM.write(addrLedOnHour, LedOnHour);   else LedOnHour = value;
  value = EEPROM.read(addrLedOffHour); if (value < 0 or value > 23)  EEPROM.write(addrLedOffHour, LedOffHour); else LedOffHour= value;
  value = EEPROM.read(addrPumpOnInterval);  if (value < 0 or value > 240) EEPROM.write(addrPumpOnInterval, PumpOnInterval);   else PumpOnInterval = value;
  value = EEPROM.read(addrPumpOffInterval); if (value < 0 or value > 240) EEPROM.write(addrPumpOffInterval, PumpOffInterval); else PumpOffInterval = value;
}

    
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
  

  pinMode (Btn1Pin, INPUT); 
  pinMode (Btn2Pin,INPUT); 

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

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
   ArduinoOTA.setPassword(OTA_password);

  // Password can be set with it's md5 value as well
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
   Serial.println ("Led On Hour" + String (LedOnHour));
   EEPROM.write(addrLedOnHour, LedOnHour); 
   LedOnMinutes =  LedOnHour * 60 + LedOnMin;

  
   }
  else if (s1 == MQTT_LedOffHour) {
   LedOffHour = s2.toInt(); 
   Serial.println ("Led Off Hour" + String (LedOnHour));
   EEPROM.write(addrLedOffHour, LedOffHour); 
   LedOffMinutes = LedOffHour * 60 + LedOffMin;

   }

   else if (s1 == MQTT_PumpOnInterval) {
   PumpOnInterval = s2.toInt(); 
   Serial.println ("Pump On Interval " + String (PumpOnInterval));
   EEPROM.write(addrPumpOnInterval, PumpOnInterval); 


   }
   else if (s1 == MQTT_PumpOffInterval) {
   PumpOffInterval = s2.toInt(); 
   Serial.println ("Pump Off Interval " + String (PumpOffInterval));
   EEPROM.write(addrPumpOffInterval, PumpOffInterval); 

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
    Serial.println("Last Debounce:" + String (lastBtn1Debounce));
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
// if so, perform on the air WIFI download if new firmware. The user will need to use the Adruino IDE and select the port as the IP address of the ESP8266
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
  
  

   

  
 if (abs((millis() - lastDisplayTime) > displayDelay)) 
{ 
  lastDisplayTime =   millis();
  display.clear();
  display.setFont(ArialMT_Plain_16);

  sprintf(dateTimeText, "%02d:%02d:%02d", timeNow->tm_hour,timeNow->tm_min,timeNow->tm_sec);
  display.drawString (0,0, dateTimeText);

  Serial.print ( dateTimeText);
  

  Serial.print ( " Left:" + String(digitalRead(Btn1Pin)) + " Right:"+ String(digitalRead(Btn2Pin))+" ");
 
 

  lux = LightSensor.GetLightIntensity() * 8;


  display.drawString(0, 16, "Lux:" + String(lux));
    
  Serial.print ("Lux:" + String(lux)+" ");  

  if (lastLux != lux) {
     lastLux = lux;
     h1 = (String) lux;
     h1.toCharArray(valueStr, 5);
     client.publish(MQTT_LUX, valueStr); 
     }
  
 
if (abs((millis() - lastECTime) > ECDelay))  { 
  lastECTime =   millis();
  sensors.requestTemperatures();// Send the command to get waterTemps
  waterTemp=sensors.getTempCByIndex(0); //Stores Value in Variable

  digitalWrite(EcPwrPin, HIGH);
  raw = analogRead(EcPin);
  digitalWrite(EcPwrPin, LOW);
 
  //***************** Converts to EC **************************
  Vdrop= (Vin*raw)/4096.0;
  
  
  Rc=(Vdrop*(R1+Ra))/(Vin-Vdrop) - Ra;


  ECraw = 1000/(Rc*K);
 
 
  //*************Compensating For Temperaure********************
  EC   =  ECraw/ (1+ waterTempCoef*(waterTemp-25.0));
  
  }


  
  Serial.print ("Water Temp:"+String(waterTemp)+" ");
  display.drawString(80, 32, "C:"+String(waterTemp));
  Serial.print("  EC: " + String(EC)+" ");
  display.drawString(0, 32, "EC:" + String(EC));

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
  
  airTemp = dht.readTemperature();  
  humidity = dht.readHumidity();
  
  // Read waterTemp as Fahrenheit (isFahrenheit = true)
  //float t = dht.readwaterTemp(true);

   Serial.print ("Temp:"+String(airTemp)+" ");
   display.drawString(80, 16, "T:" + String(airTemp));
   Serial.println ("Humidity:"+String(humidity)+" ");
   display.drawString(80, 0, "H:" + String(humidity));

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

  // only change LED status once

    if (currMin >= LedOnMinutes and currMin < LedOffMinutes) LedProg = 1; else LedProg = 0;
    if (LastLedProg != LedProg) {
       if (LedProg == 1) LedOn = true; else LedOn = false;
       LastLedProg = LedProg;
    }

  
  if (LedOn) {
        Serial.print("  Light: ON  "); 
        display.drawString(0, 48, "Led on");

        }
  else {
  
        Serial.print("  Light: OFF ");
        display.drawString(0, 48, "Led off");

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
