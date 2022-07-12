#include <ESP8266WiFi.h>
#include <PubSubClient.h> 
#include <DHT.h>            // headers for DH11 sensor   

#define DHT_PIN 2         // pin 2 for our DH11 sensor  
#define DHTTYPE DHT11 

DHT dht(DHT_PIN, DHTTYPE);  // creates DHT11 instance  
const int relay = 4;   

// Declare variables to keep track of time for DH11 sensor
unsigned long previousTimeDH11 = millis(); 
long timeIntervalDH11 = 15000; 

// WIFI  
const char *ssid = "WorkNet";
const char *password = "itsagoodquestion";  
//const char *ssid = "NETGEAR32";      
//const char *password = "sweetocean572";  

// MQTT Broker  
const char *mqtt_broker = "192.168.2.117"; // (ip address in office)
//const char *mqtt_broker = "10.0.0.16";  
const char *humidity_topic = "room/humidity"; 
const char *celsius_topic = "room/temperature/celsius"; 
const char *fahrenheit_topic = "room/temperature/fahrenheit"; 
const int mqtt_port = 1883;   

// Initialises our WIFI and MQTT Client objects
WiFiClient espClient;
PubSubClient client(espClient);   

void setup() {  
  pinMode(relay, OUTPUT);    
  dht.begin();           // initialize dht11 
  
  Serial.begin(115200);  
  while(! Serial);       // wait for serial monitor to open 
  

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
  client.subscribe("esp8266/relay");   
}

void loop() {   
  client.loop();
    
  // Gets the current time to allow us to perform time tracking techniques to trigger an action  
  unsigned long currentTime = millis();  
  
  float humidity = dht.readHumidity(); 
  float celsius = dht.readTemperature();   

  // check if returns are valid (if it is print the value)  
  if(isnan(celsius) || isnan(humidity) && currentTime - previousTimeDH11 > timeIntervalDH11 ){  
    Serial.println("Failed to read from DHT"); 
  }else if (currentTime - previousTimeDH11 > timeIntervalDH11) { 
    previousTimeDH11 = currentTime; 
    
    // prints to serial console 
    Serial.print("Humidity: "); 
    Serial.print(humidity); 
    Serial.println("%");  
    
    Serial.print("Celsius: "); 
    Serial.print(celsius); 
    Serial.println("C"); 
    
    float fahrenheit = (celsius * 1.8) + 32;   

    Serial.print("Fahrenheit: "); 
    Serial.print(fahrenheit); 
    Serial.println("F");  
    Serial.println("");        

    // Publish to MQTT broker (topic = humidity) 
    if (client.publish(humidity_topic, String(humidity).c_str())){  
      Serial.println("Humidity sent");       
    }   
    else { 
      Serial.println("Humidity failed to send to MQTT Broker "); 
    }

    // Publish to MQTT broker (topic = Celsius) 
    if (client.publish(celsius_topic, String(celsius).c_str())){  
      Serial.println("Celsius sent");       
    }  
    else { 
      Serial.println("Celsius failed to send to MQTT Broker "); 
    }  

    // Publish to MQTT broker (topic = fahrenheit) 
    if (client.publish(fahrenheit_topic, String(fahrenheit).c_str())){  
      Serial.println("Fahrenheit sent");       
    }  
    else { 
      Serial.println("Fahrenheit failed to send to MQTT Broker "); 
    } 
  } 
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
