import cv2 
import numpy as np 
import urllib.request 
import paho.mqtt.client as mqtt;  
import threading; 
import time; 

def on_connect(client, userdata, flags, rc): 
    if rc == 0: 
        print("Successfully connected to broker"); 
    else: 
        print("Failed to connect to broker");  

def on_message(client, userdata, msg): 
    print(msg.topic + ' ' + str(msg.payload.decode("utf-8")));  

def on_disconnect(client, userdata, flags, rc): 
    print("Disconnec*ted successfully with the resulted code: " + str(rc));

def arduCam_loop():  
    global bytes; 

    bytes = bytes(); 
    #stream = urllib.request.urlopen('http://10.0.0.38/stream');  
    stream = urllib.request.urlopen('http://192.168.2.128/stream');   

    while True: 
        bytes += stream.read(1024); 
        a = bytes.find(b'\xff\xd8');        # JPEG start 
        b = bytes.find(b'\xff\xd9');        # JPEG end  

        if a!=-1 and b!= -1: 
            jpg = bytes[a:b+2];             # actual image 
            bytes = bytes[b+2:];            # other informations  

            # decode to colored image
            img = cv2.imdecode(np.fromstring(jpg, dtype = np.uint8), cv2.IMREAD_COLOR); 
            cv2.imshow('Cam', img); 

            if cv2.waitKey(1) == 27: 
                exit(0); 

def MQTT_publish_subscribe(): 
    try: 
        MQTT_client.subscribe('esp8266/ultrasound/cm'); 
        MQTT_client.subscribe('esp8266/ultrasound/in'); 

        while True:  
            publisher_msg = input("Enter your message: "); 
            MQTT_client.publish('esp8266/relay', publisher_msg);   

    except KeyboardInterrupt:  
        MQTT_client.disconnect();  
        MQTT_client.loop_stop();

MQTT_address = '192.168.2.69';          # ip address of pi 
#MQTT_address = '10.0.0.16'; 
MQTT_port = 1883; 

MQTT_client = mqtt.Client(); 

MQTT_client.on_connect = on_connect;
MQTT_client.on_message = on_message;
MQTT_client.on_disconnect = on_disconnect;  

print("Connecting to MQTT broker"); 
MQTT_client.connect(MQTT_address, MQTT_port); 
MQTT_client.loop_start(); 

thread1 = threading.Thread(target = arduCam_loop); 
thread1.start(); 

thread2 = threading.Thread(target = MQTT_publish_subscribe); 
thread2.start(); 
