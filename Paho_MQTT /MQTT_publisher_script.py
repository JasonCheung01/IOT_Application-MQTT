# what this script does is ask the user what MQTT topic they would like to 
# publish to   

import paho.mqtt.client as mqtt;
import time;

MQTT_address = '192.168.2.117';         # (office address) 

MQTT_topic = input("What topic would you like to publish to? "); 
# Self note, usual port we use is 1883
MQTT_port = input("What port would you like to use? ");

def on_connect(client, userdata, flags, rc):
        if rc == 0:
                print("Successfully connected to broker");
        else:
                print("Failed to connect to broker");

MQTT_client = mqtt.Client();            # creates instance for our client 

MQTT_client.on_connect = on_connect;    # binds our call back function  

print("Connecting to MQTT broker");
MQTT_client.connect(MQTT_address, MQTT_port);  
MQTT_client.loop_start();
      
try:
        while True:
                Publisher_msg = input('Enter your message: ');
                MQTT_client.publish(MQTT_topic, Publisher_msg);

except KeyboardInterrupt:
        MQTT_client.disconnect();
        MQTT_client.loop_stop();

