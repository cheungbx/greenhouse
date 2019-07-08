
#include <PubSubClient.h>
// for temp and humidity sensor DHT11 or DHT22
#include "DHT.h"
#include "Math.h"
#define DHTTYPE DHT22  // DHT11 or DHT22 - more accuracy and wider range
#define DHTPIN      13    // data pin for the humidity sensor



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

bool sendAdafruit () {

  
char valueStr[5];

String s1, hString, tString;
DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266
float t1 = 99.0, humidity = 99.0;

WiFiClient WiFiClient;
// create MQTT object
PubSubClient client(WiFiClient);

 // set up temp. and humidity sensor
dht.begin();
  
Serial.print("Connecting to "); Serial.print(SERVER);
client.setServer(SERVER, SERVERPORT);

yield();  // this command passes control to the WIFI handler to receive any new IP packets received by the WIFI cobntroller, without this line, you cannot receive any new ip packets.

if (!client.connected())   // check if the MQTT server is still connected, if not, reinitiate the connection.
   {
#ifdef DEBUG 
 Serial.println("Attempting MQTT connection...");
#endif
// Attempt to connect
if (client.connect("", IO_USERNAME, IO_KEY)) {
#ifdef DEBUG 
      Serial.print("connected using account name: ");
      Serial.println (IO_USERNAME); 
#endif
     } else {
#ifdef DEBUG 
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("Failed to connect, going back to sleep");
#endif    
      return false;
  
    }
   }


   
// take new measurement
humidity = dht.readHumidity();       // Read humidity (percent)
t1 = dht.readTemperature(false);     // Read temperature as Celsius

if (isnan(humidity)) {
#ifdef DEBUG  
     Serial.println("Failed to read humidity from DHT sensor!");
#endif    
     }
else {
#ifdef DEBUG  
     Serial.print("Published Humidity: ");
     Serial.println(humidity);
#endif   
     hString = (String) humidity;
     hString.toCharArray(valueStr, 5);
     client.publish(IO_Humidity, valueStr);
     }
      
if (isnan(t1)) {
#ifdef DEBUG  
     Serial.println("Failed to read temp from DHT sensor!");
#endif
      }
else  {
#ifdef DEBUG    
     Serial.print("Published Temperature: ");
     Serial.println(t1);
#endif
     tString = (String) t1;
     tString.toCharArray(valueStr, 5);
     client.publish(IO_Temperature, valueStr);
     }

 
}
