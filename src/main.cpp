

#include <Arduino.h>
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#include <ESP8266Ping.h>

//needed for library

#include <PubSubClient.h>
//define your default values here, if there are different values in config.json, they are overwritten.
const char *ssid = "ZyXEL";
const char *password = "kopo2008";
const char *mqtt_server = "192.168.1.201";
const char *listentopic = "/halli/light";
const char *statustopic = "/halli/lightstatus";
const IPAddress remote_ip(192, 168, 1, 201);   //mqtt server(docker) ip
long current_time;
long looptime;
int inputint;
char msg[50];
long lastMsg=0;

//flag for saving data

WiFiClient espClient;
PubSubClient client(espClient);
//callback notifying us of the need to save config

void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
}
void callback(char *topic, byte *payload, int length)
{

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    //  Serial.print((char)payload[i]);
  }

  // Switch on the LED if an 1 was received as first character

  if ((char)payload[0] == '1')
  {

    digitalWrite(D2, LOW); // Turn the LED on (Note that LOW is the voltage level
    delay(100);             // but actually the LED is on; this is because
    digitalWrite(D2, HIGH);  // it is active low on the ESP-01)
    delay(50);
    inputint = digitalRead(D1);

    client.publish("/halli/lightstatus", String(inputint).c_str());
  }
}
boolean reconnect()
{
  String clientId = "ESP8266Client-";

  clientId += String(random(0xffff), HEX);

  if (client.connect(clientId.c_str()))
  {
    // Once connected, publish an announcement...
    Serial.println("mqttenabled");
    // client.publish(mqtt_topic,"wifimanager");
    // ... and resubscribe
    client.subscribe(listentopic);
    // client.subscribe(statustopic);
  }
  return client.connected();
}
void setup()
{
  Serial.begin(9600);
  // put your setup code here, to run once:
  setup_wifi();
  Serial.println("local ip");
  Serial.println(WiFi.localIP());
  pinMode(D1, INPUT);
  pinMode(D2, OUTPUT);
  digitalWrite(D2,HIGH);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  current_time = millis();
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();
  current_time = millis();
    if (millis() - lastMsg > 5000)
  {
    lastMsg = millis();
      if(Ping.ping(remote_ip,1)) {
    Serial.println("Success!!");
  } else {
    Serial.println("Error :(");
     ESP.restart();
  }

  }
}