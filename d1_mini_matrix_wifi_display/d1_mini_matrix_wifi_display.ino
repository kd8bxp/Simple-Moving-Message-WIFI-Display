/* WEMOS D1 Mini Matrix WIFI Display. By LeRoy Miller 2017
 *  
 *  Uses a Wemos D1, and MatrixLED Shield.
 *   Required Library: https://github.com/thomasfredericks/wemos_matrix_gfx
 *   and Adafruit_GFX library: https://github.com/adafruit/Adafruit-GFX-Library
 *   PubSubClient library: https://github.com/knolleary/pubsubclient
 *   
 *  Useage: Connect to your wifi, 
 *  change your subscription topic (If wanted)
 *  change the MQTT broker if wanted.
 *  
 *  Publish a short message to your topic on the broker.
 *  Limited to about 20 or 21 characters. 
 *  
 *  If you find this or any of my projects useful or enjoyable please support me.  
Anything I do get goes to buy more parts and make more/better projects.  
https://www.patreon.com/kd8bxp  
https://ko-fi.com/lfmiller  
 *  
 *  License: 
 *  This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses>
 */

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include <WEMOS_Matrix_GFX.h>

const char* ssid = "homeinternet";
const char* password = "trixie4me";
const char* mqtt_server = "test.mosquitto.org"; //MQTT broker
//const char* mqtt_server = "rpi-speak.local";
const char* subTopic = "matrixDisplay10"; //subscribe topic

WiFiClient espClient;
PubSubClient client(espClient);

String msg;
int value = 0;

MLED matrix(7); //set intensity=7 (maximum)


void setup() {
   Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
   reconnect();
    matrix.clear();
    matrix.writeDisplay();
}

void loop() {
   client.loop();
   matrix.setTextSize(1);
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextColor(LED_ON);
    int l = (msg.length() * 7);
  for (int8_t x=7; x>=-l; x--) {
    matrix.clear();
    matrix.setCursor(x,0);
    matrix.print(msg);
    matrix.writeDisplay();
  client.loop();
    delay(100);
  }
}

void scroll(String MSG) {
  //client.loop();
  matrix.setTextSize(1);
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextColor(LED_ON);
    int l = (MSG.length() * 7);
  for (int8_t x=7; x>=-l; x--) {
    matrix.clear();
    matrix.setCursor(x,0);
    matrix.print(MSG);
    matrix.writeDisplay();
  //client.loop();
    delay(100);
  }
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
    msg = "";
  for (int i = 0; i < length; i++) {
     Serial.print((char)payload[i]);
    msg += (char)payload[i];
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    uint32_t chipid=ESP.getChipId();
    char clientid[25];
    snprintf(clientid,25,"WIFI-Display-%08X",chipid); //this adds the mac address to the client for a unique id
    Serial.print("Client ID: ");
    Serial.println(clientid);
    if (client.connect(clientid)) {
      Serial.println("connected");
      client.subscribe(subTopic);
      scroll("Connected...");
      scroll(subTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
