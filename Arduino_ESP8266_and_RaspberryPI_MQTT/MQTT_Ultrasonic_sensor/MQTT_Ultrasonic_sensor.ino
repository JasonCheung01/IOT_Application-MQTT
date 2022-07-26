#include <ESP8266WiFi.h>
#include <PubSubClient.h>   

const int trigPin = 16;
const int echoPin = 2;

//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch; 

// WIFI  
const char *ssid = "WorkNet";
const char *password = "itsagoodquestion";  
//const char *ssid = "NETGEAR32";      
//const char *password = "sweetocean572";  

// MQTT Broker  
const char *mqtt_broker = "192.168.2.69"; // (ip address of pi)  
const char *ultrasound_topic_cm = "esp8266/ultrasound/cm"; 
const char *ultrasound_topic_in = "esp8266/ultrasound/in";
const int mqtt_port = 1883;   

// Initialises our WIFI and MQTT Client objects
WiFiClient espClient;
PubSubClient client(espClient);    

void setup() { 
  Serial.begin(115200); // Starts the serial communication
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input 

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
}

void loop() { 
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_VELOCITY/2;
  
  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;
  
  // Prints the distance on the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  Serial.print("Distance (inch): ");
  Serial.println(distanceInch);   
  
  if (client.publish(ultrasound_topic_cm, String(distanceCm).c_str())){   
    Serial.println("Ultrasound in cm units sent");        
  } else {  
    Serial.println("Ultrasound in cm failed to send to MQTT Broker ");  
  }  

  if (client.publish(ultrasound_topic_in, String(distanceInch).c_str())){   
    Serial.println("Ultrasound in inches units sent");        
  } else {  
    Serial.println("Ultrasound in inches failed to send to MQTT Broker ");  
  } 

  delay(2000);
}
