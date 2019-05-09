/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "esp_smirg" every two seconds
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

const char* ssid = "Lenovo K8 Plus 7618";
const char* password = "Abir9346";
const char* server = "broker.mqtt-dashboard.com";
const char* topic = "esp_smirg";

WiFiClient espClient;
PubSubClient client(espClient);
int analogPin = A0;
int moisture;
int percentage;
int f=0;
int map_low = 0;
int map_high = 100;
String msg;

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(server, 1883);
  client.setCallback(callback);
}
String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  String clientName;
  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientName+= macToStr(mac);
  clientName+= "-";
  clientName += String(micros() & 0xff, 16);

  Serial.print("Connecting to server s");
  Serial.print(server);
  Serial.print(" as ");
  Serial.println(clientName);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  f=(int)payload[0];
  f-=48;
  Serial.println();
  
  Serial.println(f);
  
  // Switch on the LED if an 1 was received as first character
  if (f == 0){                      //1 for high and 0 for low
    digitalWrite(BUILTIN_LED, HIGH);//urn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  }
  else if(f==1){
    digitalWrite(BUILTIN_LED, LOW);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(server)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(topic, "hello world");
      // ... and resubscribe
      client.subscribe("esp_smirg");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  
  moisture = analogRead(analogPin);
  percentage = map(moisture, map_low, map_high, 0, 100);
  msg="RAW: ";
  msg+=(String)moisture;
  msg+="  Percentage: ";
  msg+=(String)(percentage);
  msg+="%";
  delay(3000);
    client.publish(topic,(char*)msg.c_str());
  client.loop();
  
   /*
  if(moisture>800)
  {
    digitalWrite(5,HIGH);
  }
  else
  {
    digitalWrite(5,LOW);
  }
  */  
}
