import paho.mqtt.client as mqtt; 
import time; 

def on_connect(client, userdata, flags, rc): 
    if rc == 0: 
        print("Successfully connected to broker"); 
    else: 
        print("Failed to connect to broker"); 

def on_message(client, userdata, msg): 
    print(msg.topic + ' ' + str(msg.payload.decode("utf-8")));  

def on_disconnect(client, userdata, flags, rc): 
    print("Disconnected successsfully with the resulted code: " + str(rc));  

MQTT_address = '192.168.2.69';                  # ip address of pi  
MQTT_topic_ultrasound = 'esp8266/ultrasound/+'; 
MQTT_topic_relay = 'esp8266/relay';  
MQTT_port = 1883;  

MQTT_client = mqtt.Client(); 

MQTT_client.on_connect = on_connect; 
MQTT_client.on_message = on_message;
MQTT_client.on_disconnect = on_disconnect; 

print("Connecting to MQTT broker"); 
MQTT_client.connect(MQTT_address, MQTT_port); 
MQTT_client.loop_start(); 

try: 
    MQTT_client.subscribe(MQTT_topic_ultrasound); 

    while True: 
        publisher_msg = input("Enter your message: "); 
        MQTT_client.publish(MQTT_topic_relay, publisher_msg); 
        pass;  

except KeyboardInterrupt:  
    MQTT_client.disconnect(); 
    MQTT_client.loop_stop(); 

