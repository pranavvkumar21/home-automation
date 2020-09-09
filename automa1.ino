
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// defining required relays. You can add more relays by defining relay3, relay4 etc.,
#define Relay1            D1
#define Relay2            D2

#define errlight          D4

// defining switches you can add more switches if you want after reading readme.md
#define sw1               D6
#define sw2               D7
#define WLAN_SSID       "enter ssid here"             // Your SSID
#define WLAN_PASS       "enter wlan password here"        // Your password

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com" //Adafruit Server
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "enter adafruit username"            // Username
#define AIO_KEY         "aio_BLYX03rchLidBrhehFVqUXHf1LHm"   // Auth Key
int sw1v = 0;
int sw2v = 0;
int sw1state = 0;
int sw2state = 0;
int relay1state =1;
int relay2state=1;
//WIFI CLIENT
WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
// add feeds according to the requirement
Adafruit_MQTT_Subscribe Light1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/feedname1");
Adafruit_MQTT_Subscribe Light2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/feednam2");

void MQTT_connect();

void setup() {
  Serial.begin(115200);

  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(sw1, INPUT);
  pinMode(sw2, INPUT);
  pinMode(errlight, OUTPUT);

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  mqtt.subscribe(&Light1);
  mqtt.subscribe(&Light2);

}

void loop() {

  if(WiFi.status() == WL_CONNECTED){
  MQTT_connect();
  digitalWrite(errlight, 1);

  Adafruit_MQTT_Subscribe *subscription;
  (subscription = mqtt.readSubscription(500));
  if (subscription){
    if (subscription == &Light1) {
      int Light1_State = atoi((char *)Light1.lastread);

      if (Light1_State==1){

        relay1state= !relay1state;
        digitalWrite(Relay1, relay1state);
        Serial.print("relay1state: ");
        Serial.println(relay1state);
        }


    }
    if (subscription == &Light2) {                        // after adding more relay and switches copy this portion of code
      int Light2_State = atoi((char *)Light2.lastread);   //paste it right after the end of the if loop
                                                          //make necessary changes to the vasriable names
      if (Light2_State==1){                               //and you're ready to run
        relay2state= !relay2state;                        //
        digitalWrite(Relay2, relay2state);                //
        Serial.print("relay2state: ");                    //
        Serial.println(relay2state);                      //
        }
    }

  }}
  else{digitalWrite(errlight, 0); }
  sw1state = digitalRead(sw1);
    sw2state = digitalRead(sw2);
    if (sw1state != sw1v){
      relay1state= !relay1state;
      digitalWrite(Relay1, relay1state);
      Serial.print("relay1state: ");
      Serial.println(relay1state);
      sw1v = sw1state;
    }
    if (sw2state != sw2v){
      relay2state= !relay2state;
      digitalWrite(Relay2, relay2state);
      Serial.print("relay2state: ");
      Serial.println(relay2state);
      sw2v = sw2state;
    }

}


void MQTT_connect() {
  int8_t ret;

  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 30;

  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    digitalWrite(errlight, 0);
    mqtt.disconnect();
    delay(5000);
    //retries--;
    if (retries == 0) {
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
  digitalWrite(errlight, 1);

}
