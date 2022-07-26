#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h> 
#include <PubSubClient.h>  

#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"
#if !(defined ESP8266 )
#error Please select the ArduCAM ESP8266 UNO board in the Tools/Board
#endif

//This demo can only work on OV5642_MINI_5MP or OV5642_MINI_5MP_BIT_ROTATION_FIXED
//or OV5640_MINI_5MP_PLUS or ARDUCAM_SHIELD_V2 platform.
#if !(defined (OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_BIT_ROTATION_FIXED) || defined (OV5642_MINI_5MP_PLUS) ||(defined (ARDUCAM_SHIELD_V2) && defined (OV5642_CAM)))
#error Please select the hardware platform and camera module in the ../libraries/ArduCAM/memorysaver.h file
#endif   

//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

const int CS = 16;                  // Arducam   
const int relay = 15;                // relay 
const int trigPin = 16;             // ultrasound
const int echoPin = 2;

long duration;
float distanceCm;
float distanceInch;  

//you can change the value of wifiType to select Station or AP mode.
//Default is AP mode.
int wifiType = 0;   

//AP mode configuration
//Default is arducam_esp8266.If you want,you can change the AP_aaid  to your favorite name
const char *AP_ssid = "arducam_esp8266"; 
//Default is no password.If you want to set password,put your password here
const char *AP_password = "";

// WIFI  
const char *ssid = "WorkNet";
const char *password = "itsagoodquestion";  
//const char *ssid = "NETGEAR32";      
//const char *password = "sweetocean572";  

// MQTT Broker  
const char *mqtt_broker = "192.168.2.117"; // (ip address of my mac)
//const char *mqtt_broker = "10.0.0.16"; 
const int mqtt_port = 1883;      
const char *ultrasound_topic_cm = "esp8266/ultrasound/cm"; 
const char *ultrasound_topic_in = "esp8266/ultrasound/in";

// Initialise ArduCam server 
ESP8266WebServer server(80);
ArduCAM myCAM(OV5642, CS);

// Initialize our WIFI and MQTT client object
WiFiClient espClient;
PubSubClient client(espClient);  

void start_capture(){
  myCAM.clear_fifo_flag();
  myCAM.start_capture();
}

void camCapture(ArduCAM myCAM){
  WiFiClient client = server.client();
  
  size_t len = myCAM.read_fifo_length();
  if (len >= MAX_FIFO_SIZE){
    Serial.println("Over size.");
    return;
  }else if (len == 0 ){
    Serial.println("Size is 0.");
    return;
  }
  
  myCAM.CS_LOW();
  myCAM.set_fifo_burst();
  #if !(defined (OV5642_MINI_5MP_PLUS) ||(defined (ARDUCAM_SHIELD_V2) && defined (OV5642_CAM)))
  SPI.transfer(0xFF);
  #endif
  if (!client.connected()) return;
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: image/jpeg\r\n";
  response += "Content-Length: " + String(len) + "\r\n\r\n";
  server.sendContent(response);
  static const size_t bufferSize = 4096;
  static uint8_t buffer[bufferSize] = {0xFF};
  while (len) {
      size_t will_copy = (len < bufferSize) ? len : bufferSize;
      myCAM.transferBytes(&buffer[0], &buffer[0], will_copy);
      if (!client.connected()) break;
      client.write(&buffer[0], will_copy);
      len -= will_copy;
  }
  
  myCAM.CS_HIGH();
}

void serverCapture(){
  start_capture();
  Serial.println("CAM Capturing");

  int total_time = 0;
  total_time = millis();
  while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));
  total_time = millis() - total_time;
  Serial.print("capture total_time used (in miliseconds):");
  Serial.println(total_time, DEC);
  total_time = 0;
  Serial.println("CAM Capture Done!");
  total_time = millis();
  camCapture(myCAM);
  total_time = millis() - total_time;
  Serial.print("send total_time used (in miliseconds):");
  Serial.println(total_time, DEC);
  Serial.println("CAM send Done!");
}

void serverStream(){

  WiFiClient client = server.client();
  
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  server.sendContent(response);
  
  while (1){
    start_capture();   
    while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)); 
    size_t len = myCAM.read_fifo_length();
    if (len >= MAX_FIFO_SIZE){
      Serial.println("Over size.");
      continue;
    }else if (len == 0 ){
      Serial.println("Size is 0.");
      continue;
    }

    myCAM.CS_LOW();
    myCAM.set_fifo_burst(); 
    #if !(defined (OV5642_MINI_5MP_PLUS) ||(defined (ARDUCAM_SHIELD_V2) && defined (OV5642_CAM)))
    SPI.transfer(0xFF);
    #endif   
    if (!client.connected()) break;
    response = "--frame\r\n";
    response += "Content-Type: image/jpeg\r\n\r\n";
    server.sendContent(response);
    
    static const size_t bufferSize = 4096;
    static uint8_t buffer[bufferSize] = {0xFF};
    
    while (len) {
      size_t will_copy = (len < bufferSize) ? len : bufferSize;
      myCAM.transferBytes(&buffer[0], &buffer[0], will_copy);
      if (!client.connected()) break;
      client.write(&buffer[0], will_copy);
      len -= will_copy;
    }
    myCAM.CS_HIGH();
    if (!client.connected()) break;
  }
}

void handleNotFound(){
  String message = "Server is running!\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  server.send(200, "text/plain", message);
  
  if (server.hasArg("ql")){
    int ql = server.arg("ql").toInt();
    myCAM.OV5642_set_JPEG_size(ql);
    delay(1000);
    Serial.println("QL change to: " + server.arg("ql"));
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

void setup() {  
  uint8_t vid, pid;
  uint8_t temp;
#if defined(__SAM3X8E__)
  Wire1.begin();
#else
  Wire.begin();
#endif
  Serial.begin(115200);
  Serial.println("ArduCAM Start!");  

  pinMode(CS, OUTPUT); 
  pinMode(relay, OUTPUT); 
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input  

  // initialize SPI:
  SPI.begin();
  SPI.setFrequency(4000000); //4MHz 

  //Check if the ArduCAM SPI bus is OK
  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55){
    Serial.println("SPI1 interface Error!");
    while(1);
  }
  //Check if the camera module type is OV5642
  myCAM.wrSensorReg16_8(0xff, 0x01);
  myCAM.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
  myCAM.rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
   if((vid != 0x56) || (pid != 0x42)){
   Serial.println("Can't find OV5642 module!");
   while(1);
   }
   else
   Serial.println("OV5642 detected.");
 
  //Change to JPEG capture mode and initialize the OV5642 module
  myCAM.set_format(JPEG);
   myCAM.InitCAM();
   myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);   //VSYNC is active HIGH
   myCAM.OV5642_set_JPEG_size(OV5642_320x240);
   delay(1000);
  if (wifiType == 0){
    if(!strcmp(ssid,"SSID")){
       Serial.println("Please set your SSID");
       while(1);
    }
    if(!strcmp(password,"PASSWORD")){
       Serial.println("Please set your PASSWORD");
       while(1);
    }
    // Connect to WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("WiFi connected");
    Serial.println("");
    Serial.println(WiFi.localIP());
  }else if (wifiType == 1){
    Serial.println();
    Serial.println();
    Serial.print("Share AP: ");
    Serial.println(AP_ssid);
    Serial.print("The password is: ");
    Serial.println(AP_password);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_ssid, AP_password);
    Serial.println("");
    Serial.println(WiFi.softAPIP()); 
  } 

  // Start the server
  server.on("/capture", HTTP_GET, serverCapture);
  server.on("/stream", HTTP_GET, serverStream);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("Server started"); 

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

void ArduCam() { 
  server.handleClient(); 
} 

void relay_sensor() { 
  client.loop();
}  

void ultrasound() {        
  static unsigned long lastTime = 0; 
  const long timeInterval_ultrasound = 2000; 
  
  unsigned long time_Ultrasound = millis();  

  if (time_Ultrasound - lastTime > timeInterval_ultrasound) {     
    lastTime = time_Ultrasound;   
      
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
  } 
} 

void loop() {    
  ArduCam(); 
  relay_sensor();
  ultrasound(); 
} 
