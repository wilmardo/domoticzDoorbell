#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <IPAddress.h>
#include <PString.h>

/* Setting up the wifi
 * SSID is your network SSID (name)
 * PASS is your network password (use for WPA, or use as key for WEP)
 * IP is the ipaddress of the arduino
 * DOMOTICZIP is the ipaddress of the Domoticz 
 */
const char* SSID = "SSID";          //replace with own SSID
const char* PASS = "password";      //replace with own wifipassword

IPAddress DOMOTICZIP(192,168,1,0); //insert Domoticz IP address here
WiFiClient CLIENT;

/* Init the pins
 * BELPIN for gpio doorbell input
 */
const byte PROGMEM BELPIN = 0;  //change if optocoupler not on GPIO 0

/* IDX's of the domoticz devices
 * IDX_B is the idx of the doorbell
 */
const byte PROGMEM IDX_B = 48;  //change to idx of your domoticz doorbell

/*
 * Timer variables
 */
const long PROGMEM interval = 5000;
unsigned long previousMillis = 0;

/*
 * Global JSON variable for the PString buffers
 */
const int PROGMEM JSONURL_MAX = 140;
 
void setup() {
  Serial.begin(9600);
  WiFi.begin(SSID, PASS);
  
  //following line can me commented out dor DHCP mode. 
  WiFi.config(IPAddress(192,168,1,16), IPAddress(192,168,1,1), IPAddress(255,255,255,0)); // WiFi.config(ip, dns, gateway, subnet) see https://www.arduino.cc/en/Reference/WiFiConfig

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
  }

  pinMode(BELPIN, INPUT_PULLUP);

  //send startup message to domoticz log
  sendLog("Startup_succesfull");
}

/* Function to add message to the Domoticz log */
void sendLog(const char* message) {
  char buffer[JSONURL_MAX];
  PString logUrl(buffer, sizeof(buffer), "/json.htm?type=command&param=addlogmessage&message=Doorbell:%20"); //%20 is a space
  logUrl +=  message;
  sendRequest(logUrl);
}

/*
 * Function to send request to Domoticz by URL
 * For authorization see https://www.domoticz.com/wiki/Domoticz_API/JSON_URL's#Authorization
 */
void sendRequest(PString &url) {
  char buffer[16];
  PString ipaddress(buffer, sizeof(buffer));
  ipaddress += DOMOTICZIP[0];
  ipaddress += ".";
  ipaddress += DOMOTICZIP[1]; 
  ipaddress += ".";
  ipaddress += DOMOTICZIP[2];
  ipaddress += ".";
  ipaddress += DOMOTICZIP[3];
   
  if (CLIENT.connect(DOMOTICZIP, 8080)) {
    // Make a HTTP request:
    CLIENT.print("GET ");
    CLIENT.print(url);
    CLIENT.print(" HTTP/1.1\r\n");
    CLIENT.print("Host: ");
    CLIENT.print(ipaddress);
    CLIENT.print("\r\n");
    CLIENT.print("Accept: */*\r\n");
    CLIENT.print("User-Agent: Mozilla/4.0 (compatible; esp8266 Lua; Windows NT 5.1)\r\n");
    CLIENT.print("\r\n");
    Serial.println(F("Data send succesfully"));
  } else {
    // if you didn't get a connection to the server:
    Serial.println(F("Connection to server failed"));
  }
}

void loop() {
  unsigned long currentMillis = millis();
  
  if(digitalRead(BELPIN) == LOW) { // when doorbell rings BELPIN is low
    if(currentMillis - previousMillis >= interval) { //wait interval (default 5 seconds) before setting the domoticz switch on again
      previousMillis = currentMillis;
      char urlBuffer[90];
      PString url(urlBuffer, sizeof(urlBuffer), "/json.htm?type=command&param=switchlight&idx=");
      url += IDX_B;
      url += "&switchcmd=On";
      sendRequest(url);
    }  
  }
}
    
