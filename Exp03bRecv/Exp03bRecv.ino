#include <esp_now.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "ESPAsyncWebSrv.h"
#include <Arduino_JSON.h>


const char* wifi_network_ssid     = "Team MIKE";   /*Replace with Your own network SSID*/
const char* wifi_network_password =  "invincibles"; /*Replace with Your own network PASSWORD*/



// Replace with your network credentials (STATION)
const char* ssid = "mikemighty";
const char* password = "wewillrockyou019";

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int board_id;
  float per_unit_weight;
  int reading_id;
  char rfid_id[9];
  
} struct_message;

struct_message incomingReadings;

JSONVar board;


AsyncWebServer server(80);
AsyncEventSource events("/events");

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) { 
  // Copies the sender mac address to a string
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  
  board["board_id"] = incomingReadings.board_id;
  board["per_unit_weight"] = incomingReadings.per_unit_weight;
  board["reading_id"] = incomingReadings.reading_id;
  board["rfid_id"] = incomingReadings.rfid_id;
  String jsonString = JSON.stringify(board);
  events.send(jsonString.c_str(), "new_readings", millis());
  
  Serial.printf("Board ID %i: %u bytes\n", incomingReadings.board_id, len);
  Serial.printf("per_unit_weight value: %4.2f \n", incomingReadings.per_unit_weight);
  Serial.println("rfid_id value: ");
  //Serial.println(incomingReadings.rfid_id.);
  for(byte i=0;i< 4 ;i++){
    Serial.printf("%x",incomingReadings.rfid_id[i]);
  }
  Serial.printf("\nreading_id value: %d \n", incomingReadings.reading_id);
  Serial.println();
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP-NOW DASHBOARD</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p {  font-size: 1.2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #2f4468; color: white; font-size: 1.7rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 700px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); }
    .reading { font-size: 2.8rem; }
    .packet { color: #bebebe; }
    .card.per_unit_weight { color: #fd7e14; }
    .card.rfid_id { color: #1b78e2; }
  </style>
</head>
<body>
  <div class="topnav">
    <h3>ESP-NOW DASHBOARD</h3>
  </div>
  <div class="content">
    <div class="cards">
      <div class="card per_unit_weight">
        <h4>  <i class="fa-regular fa-weight-scale"></i>   BOARD #1</h4>
        <p><span class="reading">PER UNIT WEIGHT<span id="b1"></span> </span></p>
        <p class="packet">Reading ID: <span id="rb1"></span></p>
      </div>
      <div class="card rfid_id"> 
        <h4><i class="fa-solid fa-nfc-signal"></i> BOARD #1</h4>
        <p><span class="reading">RFID ID:<span id="rfid1"></span></span></p>
        <p class="packet">Reading ID: <span id="rb1"></span></p>
      </div>



      <div class="card per_unit_weight">
        <h4><i class="fa-regular fa-weight-scale"></i> BOARD #2</h4>
        <p><span class="reading">PER UNIT WEIGHT<span id="b2"></span> </span></p>
        <p class="packet">Reading ID: <span id="rb2"></span></p>
      </div>
      
      <div class="card rfid_id">
        <h4><i class="fa-solid fa-nfc-signal"></i> BOARD #2</h4>
        <p><span class="reading">RFID ID:<span id="rfid2"></span></span></p>
        <p class="packet">Reading ID: <span id="rb2"></span></p>
      </div>



    </div>
  </div>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);
 
 source.addEventListener('new_readings', function(e) {
  console.log("new_readings", e.data);
  var obj = JSON.parse(e.data);
  document.getElementById("b"+obj.id).innerHTML = obj.per_unit_weight.toFixed(2);
  document.getElementById("rb"+obj.id).innerHTML = obj.reading_id;
  document.getElementById("rfid"+obj.id).innerHTML = obj.rfid_id;
  
 }, false);
}
</script>
</body>
</html>)rawliteral";



void setup() {
  // Initialize Serial Monitor
  
  Serial.begin(9600);
  
  // Set the device as a Station and Soft Access Point simultaneously
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid,password);  /*Configuring ESP32 access point SSID and password*/
  // Set device as a Wi-Fi Station
  
  WiFi.begin(wifi_network_ssid, wifi_network_password);
    
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());
  
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
   
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.begin();
}
 
void loop() {
  
  
  static unsigned long lastEventTime = millis();
  static const unsigned long EVENT_INTERVAL_MS = 5000;
  if ((millis() - lastEventTime) > EVENT_INTERVAL_MS) {
    events.send("ping",NULL,millis());
    lastEventTime = millis();
  }
}

