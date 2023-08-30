#include "ThingSpeak.h"
#include <WiFi.h>
#include "DHT.h"

#define DHTPIN 4     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

#define SECRET_SSID "MySSID"    // replace MySSID with your WiFi network name
#define SECRET_PASS "MyPassword"  // replace MyPassword with your WiFi password

#define SECRET_CH_ID 00000000     // replace 0000000 with your channel number
#define SECRET_READ_APIKEY "XYZ"   // replace XYZ with your channel write API Key

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myReadAPIKey = SECRET_READ_APIKEY;

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);



void setup()
{
Serial.begin(115200);  //Initialize serial
dht.begin();//Initialize DHT sensor
WiFi.mode(WIFI_STA);//set ESP32 in station mode   
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

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float t=dht.readTemperature();//read temperature value from sensor
  int t1=ThingSpeak.writeField(myChannelNumber,1,t,myReadAPIKey);
  // write temperature value (as Celsius) to the ThingSpeak channel
  float h = dht.readHumidity();//read humidity value from sensor
  int h1=ThingSpeak.writeField(myChannelNumber,2 , h, myReadAPIKey);
  // write humidity value to the ThingSpeak channel
  // Check if any write failed and exit early (to try again).
  if (isnan(h1) || isnan(t1)){
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  
  
  //printing values onto console
  
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print("\n");
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print("\n");

  delay(2000); // Wait 2 seconds to update the channel again




}


