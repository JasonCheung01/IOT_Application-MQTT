#include <ESP8266WiFi.h>
#include <PubSubClient.h>   

const int relay = D0; 

// WIFI  
const char *ssid = "WorkNet";
const char *password = "itsagoodquestion";    

// MQTT Broker  
const char *mqtt_broker = "192.168.2.116";
const int mqtt_port = 1883;   

// Initialises our WIFI and MQTT Client objects
WiFiClient espClient;
PubSubClient client(espClient);   

void setup() {  
  pinMode(relay, OUTPUT);  
  
  Serial.begin(115200);  

  // Connecting to WIFI network  
  WiFi.begin(ssid,password);  
  while (WiFi.status() != WL_CONNECTED) {  
    delay(2000); 
    Serial.print("Attempting to connect to "); 
    Serial.println(ssid); 
    Serial.println(""); 
  }  
  
  Serial.print("Successfully connected to ");  
  Serial.println(ssid);   

  // Connecting to MQTT broker    
  client.setServer(mqtt_broker, mqtt_port); 
  client.setCallback(callback);    

  while (!client.connected()) { 
    String client_id = "esp8266-client-"; 
    client_id += String(WiFi.macAddress()); 

    Serial.printf("The client %s connects to mosquitto mqtt broker\n", client_id.c_str()); 
    
    if (client.connect(client_id.c_str())) { 
      Serial.println("Public emqx mqtt broker connected"); 
    } else { 
      Serial.print("failed with state "); 
      Serial.print(client.state()); 
      Serial.println(" ");   
      delay(2000); 
    } 
  }  
  client.subscribe("raspberry/relay"); 
}

void loop() { 
  client.loop();
} 

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message received in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  String message;
  for (int i = 0; i < length; i++) 
  {
    message = message + (char)payload[i];
  }
  Serial.print(message);
  if (message == "on") 
  {
    digitalWrite(relay, HIGH);
  }
  else if (message == "off") 
  {
    digitalWrite(relay, LOW);
  }
  Serial.println();
  Serial.println("-----------------------");
} 
