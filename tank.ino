

// Import required libraries
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi parameters
const char* ssid = "";
const char* password = "";

// MQTT parameters
const char* server = "192.168.1.248";
const int port = 1883;

const char* pumpStatusTopic = "home/tank/pump/status";
const char* pumpTopic = "home/tank/pump";
const char* tankLightSensorTopic = "home/tank/lightsensor";
const char* tankSensorTopic = "home/tank/sensor";
  
char messageTankLightSensor[10];
char messageTankSensor[10];

const int pinRelay = 5;
const int pinTankSensor = 16;
const int pinLightSensor = A0;

char message_buff[128];

WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
  pinMode(pinTankSensor, INPUT);
  pinMode(pinRelay, OUTPUT); 
  setup_wifi();           //On se connecte au réseau wifi
  client.setServer(server, port);    //Configuration de la connexion au serveur MQTT 
  client.setCallback(callback);
}
 
//Connexion au réseau WiFi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connexion a ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("Connexion WiFi etablie ");
  Serial.print("=> Addresse IP : ");
  Serial.print(WiFi.localIP());
}
 
//Reconnexion
void reconnect() {
  //Boucle jusqu'à obtenur une reconnexion
  while (!client.connected()) {
    Serial.print("Connexion au serveur MQTT...");
    if (client.connect("ESP8266ClientTank")) {
      Serial.println("OK");
      client.subscribe(pumpTopic);
    } else {
      Serial.print("KO, erreur : ");
      Serial.print(client.state());
      Serial.println(" On attend 5 secondes avant de recommencer");
      delay(5000);
    }
  }
}
 
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
// read the input on analog pin 0:
  int lightValue = analogRead(pinLightSensor);
  int tankValue = digitalRead(pinTankSensor);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = lightValue * (5.0 / 1023.0);

dtostrf(voltage*100, 4, 0, messageTankLightSensor);
dtostrf(tankValue,4,0,messageTankSensor);
  
  client.publish(tankLightSensorTopic,messageTankLightSensor,true);
  client.publish(tankSensorTopic,messageTankSensor,true);
  delay(1000);
  
}
void callback(char* topic, byte* payload, unsigned int length) {
 
  int i = 0;

  // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  
  String msgString = String(message_buff);
 
  
  if ( msgString == "ON" ) {
    digitalWrite(5,HIGH); 
    client.publish(pumpStatusTopic,"ON",true);
  } else {
    digitalWrite(5,LOW);
    client.publish(pumpStatusTopic,"OFF",true);
  }
}
