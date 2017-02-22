
// Import required libraries
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi parameters
const char* ssid = "";
const char* password = "";

// MQTT parameters
const char* server = "192.168.1.248";
const int port = 1883;

const char* status_topic = "home/living/tv/status";

bool debug = false;

char message_buff[128];

WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
  Serial.begin(115200);     //Facultatif pour le debug
  pinMode(5,OUTPUT);     //Pin 2 
  setup_wifi();           //On se connecte au réseau wifi
  client.setServer(server, port);    //Configuration de la connexion au serveur MQTT
  client.setCallback(callback);  //La fonction de callback qui est executée à chaque réception de message   
}
 
//Connexion au réseau WiFi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connexion a ");
  Serial.println(ssid);
 
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
    if (client.connect("ESP8266ClientTV")) {
      Serial.println("OK");
      client.subscribe("home/living/tv");
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
  
}
 
// Déclenche les actions à la réception d'un message
// D'après http://m2mio.tumblr.com/post/30048662088/a-simple-example-arduino-mqtt-m2mio
void callback(char* topic, byte* payload, unsigned int length) {
 
  int i = 0;
  if ( debug ) {
    Serial.println("Message recu =>  topic: " + String(topic));
    Serial.print(" | longueur: " + String(length,DEC));
  }
  // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  
  String msgString = String(message_buff);
  if ( debug ) {
    Serial.println("Payload: " + msgString);
  }
  
  if ( msgString == "ON" ) {
    digitalWrite(5,LOW); 
    client.publish(status_topic,"ON",true);
  } else {
    digitalWrite(5,HIGH);
    client.publish(status_topic,"OFF",true);
  }
}
