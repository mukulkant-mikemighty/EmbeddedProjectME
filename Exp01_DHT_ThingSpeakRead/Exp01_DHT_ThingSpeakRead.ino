#include "ThingSpeak.h"
#include <WiFi.h>

#define SECRET_SSID "MySSID"    // replace MySSID with your WiFi network name
#define SECRET_PASS "MyPassword"  // replace MyPassword with your WiFi password

#define SECRET_CH_ID 0000000     // replace 0000000 with your channel number
#define SECRET_READ_APIKEY "XYZ"   // replace XYZ with your channel Read API Key

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myReadAPIKey = SECRET_READ_APIKEY;

void setup()
{
Serial.begin(115200);  //Initialize serial
WiFi.mode(WIFI_STA);//set ESP32 as a station   
ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop()
{
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  int x = ThingSpeak.readMultipleFields(myChannelNumber, myReadAPIKey);
  if(x == 200){
    Serial.println("Channel read successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float t = ThingSpeak.readFloatField(myChannelNumber,1 , myReadAPIKey);
  // Read temperature as Celsius (the default)
  float h = ThingSpeak.readFloatField(myChannelNumber,2 , myReadAPIKey);
  //Read humidity as percentage

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)){
    Serial.println(F("Failed to read from channel / DHT sensor!"));
    return;
  }
 //write values to console 
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print("\n");
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print("\n");
  delay(2000); // Wait 2 seconds to update the channel again
}


