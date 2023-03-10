# what this script does is that it gives us the ability to send messages to 
# our ESP266 in which we can turn on/off the LED light  

import paho.mqtt.client as mqtt; 
import time; 

MQTT_address = '192.168.2.117';		# (office address) 
#MQTT_topic = 'esp8266/LED';	  

def on_connect(client, userdata, flags, rc):  
	if rc == 0: 
		print("Successfully connected to broker"); 
	else: 
		print("Failed to connect to broker");  

MQTT_client = mqtt.Client();		# creates instance for our client 

MQTT_client.on_connect = on_connect;	# binds our call back function  
 
print("Connecting to MQTT broker");
MQTT_client.connect(MQTT_address, 1883);  # connect to address with that port  
MQTT_client.loop_start();


try:  
	while True: 
		msg = input('Enter your message: '); 
		MQTT_client.publish('esp8266/LED', msg);	
		
except KeyboardInterrupt:  
	MQTT_client.disconnect(); 
	MQTT_client.loop_stop();  

