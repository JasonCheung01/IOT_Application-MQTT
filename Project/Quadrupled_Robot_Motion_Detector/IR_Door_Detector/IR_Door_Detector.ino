#include <ESP8266WiFi.h>
#include <PubSubClient.h>   

// libraries for OLED display
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>  
//#include <Fonts/FreeSerif9pt7b.h>

#define OLED_SCREEN_WIDTH 128 // OLED display width, in pixels
#define OLED_SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT, &Wire, -1); 

const int IR = A0;                  // Analog reading for IR 

// WIFI  
const char *ssid = "WorkNet";
const char *password = "itsagoodquestion";   

// MQTT Broker 
const char *mqtt_broker = "192.168.2.116"; // ip address of Raspberry Pi   
//const char *mqtt_broker = "192.168.2.117"; // ip address of Raspberry Pi  
const int mqtt_port = 1883;   
const char *IR_topic = "raspberry/IR"; 
const char *relay_topic = "raspberry/relay"; 
const char *spider_robot_topic = "raspberry/spider";

// Initialize our WIFI and MQTT client object
WiFiClient espClient;
PubSubClient client(espClient);   

void setup() { 
  Serial.begin(115200);  
  while(! Serial);       // wait for serial monitor to open  

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }  
  delay(2000);   

  display.clearDisplay();   // clear the buffer  
  
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


void IR_sensor() {      
  static unsigned long lastTime = 0; 
  const long timeInterval_IR = 500; 
  
  unsigned long time_IR = millis();    

  if (time_IR - lastTime > timeInterval_IR) { 
    lastTime = time_IR; 
    int val = analogRead(IR);    

    display.setTextSize(2); 
    display.setTextColor(WHITE); 
    display.setCursor(0,0);
    display.print("IR sensor value: ");      
    display.println(val); 
    display.display();   
    
    if (val < 600) {  
      String msg = "Door is fully closed";  
      String relay_msg = "off";   
      String robot_msg = "stay";
      
      if (client.publish(IR_topic, msg.c_str())) { 
        Serial.println("IR message successfully sent");  
        //Serial.println("Door is closed");
      } else {  
        Serial.println("IR message failed to send to MQTT Broker");
      }  
      
      if (client.publish(relay_topic, relay_msg.c_str())) { 
        Serial.println("Relay message successfully sent");  
        //Serial.println("Relay is off");
      } else {  
        Serial.println("Relay message failed to send to MQTT Broker");
      } 
      
      if (client.publish(spider_robot_topic, robot_msg.c_str())) { 
        Serial.println("Spider message successfully sent");  
        //Serial.println("Relay is off");
      } else {  
        Serial.println("Spider message failed to send to MQTT Broker");
      }
      
    } else { 
      String msg = "Door is fully open"; 
      String relay_msg = "on"; 
      String robot_msg = "move";  
      
      if (client.publish(IR_topic, msg.c_str())) { 
        Serial.println("IR message successfully sent");  
        //Serial.println("Door is open");
      } else {  
        Serial.println("IR message failed to send to MQTT Broker");
      } 
      
      if (client.publish(relay_topic, relay_msg.c_str())) { 
        Serial.println("Relay message successfully sent");  
        //Serial.println("Relay is on");
      } else {  
        Serial.println("Relay message failed to send to MQTT Broker");
      }   
      
      if (client.publish(spider_robot_topic, robot_msg.c_str())) { 
        Serial.println("Spider message successfully sent");  
        //Serial.println("Relay is on");
      } else {  
        Serial.println("Spider message failed to send to MQTT Broker");
      }
      
    } 
    display.clearDisplay();   // clear the buffer 
  }
}  

void loop() { 
  IR_sensor(); 
}
