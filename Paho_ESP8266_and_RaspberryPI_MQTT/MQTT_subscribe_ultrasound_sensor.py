import paho.mqtt.client as mqtt; 
import time; 

MQTT_address = '192.168.2.69'; 
MQTT_topic_ultrasound = 'esp8266/ultrasound/+'; 

def on_connect(client, userdata, flags, rc): 
    print("Connected successfully with the resulted code: " + str(rc)); 
    client.subscribe(MQTT_topic_ultrasound); 

def on_message(client, userdata, msg): 
    print(msg.topic + ' ' + str(msg.payload.decode("utf-8"))); 

def on_disconnect(client, userdata, flags, rc): 
    print("Disconnected successfully with the resulted code: " + str(rc)); 

MQTT_client = mqtt.Client(); 

MQTT_client.on_connect = on_connect; 
MQTT_client.on_message = on_message; 
MQTT_client.on_disconnect = on_disconnect;  

print("Connecting to MQTT broker "); 

MQTT_client.connect(MQTT_address, 1883);  
MQTT_client.loop_forever();
