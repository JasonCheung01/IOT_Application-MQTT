import paho.mqtt.client as mqtt; 	# imports the client 
import time; 

# So what this script does is that it sets up our client and broker  
# Afterwards, we can then subscribe to a topic and publish on that opic 

# call back functions  
# procesess the recieved messages coming back from the broker and prints it out   
def on_log(client, userdata, level, buf): 
	print("log: " + buf); 

# notifies if we connected to the broker successfully
def on_connect(client, userdata, flags, rc): 
	if rc==0: 
		print("connected OK"); 
	else: 
		print("Bad connection Returned code=", rc);

# notifies if we disconnected from the broker successfully  
def on_disconnect(client, userdata, flags, rc = 0):  
	print("Disconnect result code " + str(rc)); 

# this function allows us to see the message that we published  
def on_message(client, userdata, msg): 
	topic = msg.topic; 
	m_decode = str(msg.payload.decode("utf-8")); 
	print("message recieved", m_decode); 

broker = "test.mosquitto.org";		# this is our broker for our client 
client = mqtt.Client("python1");	# creates new instances for our client  

client.on_connect = on_connect; 	# bind call back function 
client.on_log= on_log;  
client.on_message = on_message;   
client.on_disconnect = on_disconnect; 

print("Connecting to broker ", broker); 
  
client.connect(broker);  		# connect to broker
client.loop_start();			# start loop of our client 
time.sleep(2);				# callback functions are asycnhronous, so if you did not had this sleep
					# call, you will not be sure of the order of each call/log being made  

client.subscribe("house/sensor1");		# client will subscribe to the following topic 
time.sleep(2); 
client.publish("house/sensor1", "turn on");	# client publishing a message to topic "house/sensor1"
time.sleep(2); 

client.loop_stop();			# stop loop 
client.disconnect();			# disconnect
