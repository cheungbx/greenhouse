/*

 Save your Credentials in a secret file so it will not accidentially reviewed with your source code.
 your Credentials.h file should be in the same folder as your source code for the arduino project and have the following content:
 
#define ssid          "your home wifi ssid"
#define password      "your home wifi password"
#define OTA_Password  "the password to upgrade firmware through WIFI"

// I am useing adafruit.io as the MQTT broker and dashboard service. You can use other MQTT services on the internet.

#define IO_USERNAME   "username to login to adafruit.io", 
#define IO_KEY        "AIO Key - you can get from adafruit.io - click View AIO Key"
#define SERVER          "io.adafruit.com"     server dns name for Adafruit.IO service.
#define SERVERPORT      1883
#define IO_Temperature "username/feeds/temperature"
#define IO_Humidity    "usedrname/feeds/humidity"
#define IO_Switch      "username/feeds/lightsw"
#define IO_Upgrade     "username/feeds/upgrade"

 */

#define ssid          "BILLYWIFI"
#define password      "Xolmem13"
#define OTA_Password  "Xolmem18"
#define SERVER          "io.adafruit.com"    
#define SERVERPORT      1883
#define IO_USERNAME  "cheungbx2"
#define IO_KEY       "0b62c232419448319c5469e2a26b8407"
#define IO_Temperature "cheungbx2/feeds/temperature"
#define IO_Humidity    "cheungbx2/feeds/humidity"
#define IO_Switch      "cheungbx2/feeds/lightsw"
#define IO_Upgrade     "cheungbx2/feeds/upgrade"


#define SMTP2goUSER    "cheungbx@hotmail.com"
#define SMTP2goPW      "Xolmem18"
