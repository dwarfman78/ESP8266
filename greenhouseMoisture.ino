// Import required libraries
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi parameters
const char* ssid = "";
const char* password = "";

// MQTT parameters
const char* server = "192.168.1.248";
const int port = 1883;

const char* moistureSensorTopic = "home/greenhouse/moisture";

char messageMoistureSensor[10];
const int pinMoistureSensor = A0;

const int sleepTimeS = 30;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  setup_wifi();           //On se connecte au réseau wifi
  client.setServer(server, port);    //Configuration de la connexion au serveur MQTT 

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int moistureValue = analogRead(pinMoistureSensor);
  dtostrf(moistureValue,4,0,messageMoistureSensor);

  client.publish(moistureSensorTopic,messageMoistureSensor,true);

  ESP.deepSleep(sleepTimeS * 1000000);

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
    if (client.connect("ESP8266ClientMoisture")) {
      Serial.println("OK");
      client.subscribe(moistureSensorTopic);
    } else {
      Serial.print("KO, erreur : ");
      Serial.print(client.state());
      Serial.println(" On attend 5 secondes avant de recommencer");
      delay(5000);
    }
  }
}
void loop() {
  

}
