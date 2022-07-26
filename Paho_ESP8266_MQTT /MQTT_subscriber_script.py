# What this script does is ask the user what MQTT topic they would like to 
# subscribe to   

import paho.mqtt.client as mqtt;
import time; 

MQTT_address = '192.168.2.117';       # (office address)   
#MQTT_address = '10.0.0.16';             # (home address)  

MQTT_topic = input("What MQTT topic would you like to subscribe to? "); 
MQTT_port = int(input("What port would you like to use? ")); 

# notifies if we connnected to the broker successfully, if so subscribe to 
# our MQTT topic  
def on_connect(client, userdata, flags, rc):
        print("Connected successfully with the resulted code: " + str(rc));
        client.subscribe(MQTT_topic);

# allows us to see the message that we publish  
def on_message(client, userdata, msg):
        print(msg.topic + ' ' + str(msg.payload.decode("utf-8"))); 

# notifies if we disconnected from the broker successfully 
def on_disconnect(client, userdata, flags, rc):
        print("Disconnected successfully with the resulted code: " + str(rc));

MQTT_client = mqtt.Client();    	# creates instance for our client 

# binds our call back functions  
MQTT_client.on_connect = on_connect;
MQTT_client.on_message = on_message;
MQTT_client.on_disconnect = on_disconnect;

print("Connecting to MQTT broker ");

MQTT_client.connect(MQTT_address, MQTT_port);  
MQTT_client.loop_forever();		# stays inside the client forever   
