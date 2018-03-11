/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "";
const char* password = "";
const char* mqtt_server = "192.168.1.225";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(13, LOW); 
    delay(500);
    Serial.print(".");
    digitalWrite(13, HIGH); 
    delay(500);

  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if(strcmp(topic,"liv/set/LivingRoomCornerLamp/STATE")==0){
      if ((char)payload[0] == '1') {
        digitalWrite(12, HIGH);   // Turn the LED on (Note that LOW is the voltage level
        client.publish("liv/status/LivingRoomCornerLamp/STATE", "1");
        // but actually the LED is on; this is because
        // it is active low on the ESP-01)
      } else {
        digitalWrite(12, LOW);  // Turn the LED off by making the voltage HIGH
        client.publish("liv/status/LivingRoomCornerLamp/STATE", "0");
      }
  }

  if(strcmp(topic,"liv/identify/LivingRoomCornerLamp")==0){
      if ((char)payload[0] == '1') {
        for(int i=0;i++;i>=5){
          digitalWrite(13, LOW); 
          delay(500);
          digitalWrite(13, HIGH); 
          delay(500);
        }

        digitalWrite(13, HIGH); 
    
      }
  }


}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create client ID
    String clientId = "Living Room Corner Lamp";

    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("liv/status/LivingRoomCornerLamp/STATE", "0");
      // ... and resubscribe
      client.subscribe("liv/set/LivingRoomCornerLamp/STATE");
      client.subscribe("liv/identify/LivingRoomCornerLamp");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(12, OUTPUT);     // Initialize the RELAY as an output
  pinMode(13, OUTPUT);     // Initialize the BUILTIN_LED as an output
  digitalWrite(13, LOW); 
  digitalWrite(12, HIGH); 

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  digitalWrite(13, HIGH); 
  digitalWrite(12, LOW); 

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}
