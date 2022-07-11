import paho.mqtt.client as mqtt; 
import time;  

MQTT_address = '192.168.2.117';	# (office address)   
#MQTT_address = '10.0.0.16';		# (home address) 
#MQTT_topic = '#';  

# list of call back functions  

# notifies if we connnected to the broker successfully, if so subscribe to 
# our MQTT topic  
def on_connect(client, userdata, flags, rc): 
	print("Connected successfully with the resulted code: " + str(rc)); 
	#client.subscribe(MQTT_topic); 
	client.subscribe('shellies/shellyplugus-083af2004f8c/relay/0/');   

# allows us to see the message that we publish  
def on_message(client, userdata, msg): 
	print(msg.topic + ' ' + str(msg.payload.decode("utf-8")));  
	
	#if(str(msg.payload.decode("utf-8")) == "on"): 
		#client.publish('shellyplugus-083af2004f8c/switch/0', ) 
	#elif(str(msg.payload.decode("utf-8")) == "false"): 
		#call Switch.Set '{"id": 0,"output":false}';   
	
# notifies if we disconnected from the broker successfully 
def on_disconnect(client, userdata, flags, rc): 
	print("Disconnected successfully with the resulted code: " + str(rc)); 

MQTT_client = mqtt.Client();	# creates instance for our client 

# binds our call back functions  
MQTT_client.on_connect = on_connect; 
MQTT_client.on_message = on_message; 
MQTT_client.on_disconnect = on_disconnect; 

print("Connecting to MQTT broker "); 

MQTT_client.connect(MQTT_address, 1883);	# connect to MQTT address in the following port  
MQTT_client.loop_forever();			# stay inside the client   

