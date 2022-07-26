#include <DHT.h>  
#include <ESP8266WiFi.h>    // Enables the ESP8266 to connect to the local network (WIFI) 
#include <PubSubClient.h>   // connect and publish to the MQTT broker  

#define DHT_PIN 2           // DHT sensor is conencted on pin 2  
#define DHTTYPE DHT11       // Declaring which DHT sensor version we are currently using  

DHT dht(DHT_PIN, DHTTYPE);  // creates DHT11 instance 

// WIFI  
//const char *ssid = "WorkNet";
//const char *password = "itsagoodquestion";  

const char *ssid = "NETGEAR32";      
const char *password = "sweetocean572"; 

// MQTT Broker  
//const char *mqtt_broker = "192.168.2.117"; // Enter your WiFi or Ethernet IP (ip address of my mac)

const char *mqtt_broker = "10.0.0.16";

const char *humidity_topic = "room/humidity"; 
const char *celsius_topic = "room/temperature/celsius"; 
const char *fahrenheit_topic = "room/temperature/fahrenheit"; 
const int mqtt_port = 1883; 

// Initialises our WIFI and MQTT Client objects
WiFiClient espClient;
PubSubClient client(espClient); 

void setup() {
  Serial.begin(115200); // Set Serial baud to 115200  
  dht.begin();          // initialize our dht 11 sensor   

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
  float humidity = dht.readHumidity(); 
  float celsius = dht.readTemperature();   

  // check if returns are valid (if it is print the value)  
  if(isnan(celsius) || isnan(humidity)){  
    Serial.println("Failed to read from DHT"); 
  }else{  
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

    // MQTT can only transmit strings  
    //String humidity_str = "Humidity: " + String((float)humidity) + "%";  
    //String celsius_str = "Celsius: " + String((float)celsius) + "C"; 
    //String fahrenheit_str = "Fahrenheit: " + String((float)fahrenheit) + "F";        

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

  delay(15000);   // prints new values from dht sensor every 15 seconds 
  
}
