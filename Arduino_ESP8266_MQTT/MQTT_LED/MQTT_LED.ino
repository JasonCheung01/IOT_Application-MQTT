// What this program does is subscribe to the esp8266/LED topic so 
// that it can read and perform an action based off the message 
// recieved 

#include <ESP8266WiFi.h>
#include <PubSubClient.h>  

#define LED_PIN 5  

// WIFI  
const char *ssid = "WorkNet";
const char *password = "itsagoodquestion";  
//const char *ssid = "NETGEAR32";      
//const char *password = "sweetocean572"; 

// MQTT Broker  
const char *mqtt_broker = "192.168.2.117"; // (ip address in office)
//const char *mqtt_broker = "10.0.0.16"; 
const int mqtt_port = 1883; 

// Initialises our WIFI and MQTT Client objects
WiFiClient espClient;
PubSubClient client(espClient);  

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
    digitalWrite(LED_PIN, HIGH);
  }
  else if (message == "off") 
  {
    digitalWrite(LED_PIN, LOW);
  }
  Serial.println();
  Serial.println("-----------------------");
} 

void setup() {  
  pinMode(LED_PIN, OUTPUT); 
  
  Serial.begin(115200); // Set Serial baud to 115200   

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
  client.subscribe("esp8266/LED"); 
}

void loop() { 
  client.loop();
} 
