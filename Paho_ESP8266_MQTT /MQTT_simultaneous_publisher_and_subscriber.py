import paho.mqtt.client as mqtt;
import time;

MQTT_address = '192.168.2.117';         # (office address) 

MQTT_publish_topic = input("What topic would you like to publish to? "); 
MQTT_subscribe_topic = input("What topic would you like to subscribe to? "); 
MQTT_port = int(input("What port would you like to use? "));

def on_connect(client, userdata, flags, rc):
        if rc == 0:
                print("Successfully connected to broker");
        else:
                print("Failed to connect to broker");

# allows us to see the message that we publish  
def on_message(client, userdata, msg):
        print(msg.topic + ' ' + str(msg.payload.decode("utf-8")));

# notifies if we disconnected from the broker successfully 
def on_disconnect(client, userdata, flags, rc):
        print("Disconnected successfully with the resulted code: " + str(rc));

MQTT_client = mqtt.Client();            # creates instance for our client 

# binds our call back function
MQTT_client.on_connect = on_connect;
MQTT_client.on_message = on_message;
MQTT_client.on_disconnect = on_disconnect;

print("Connecting to MQTT broker");
MQTT_client.connect(MQTT_address, MQTT_port);
MQTT_client.loop_start();

try:
        MQTT_client.subscribe(MQTT_subscribe_topic);

        while True:
                Publisher_msg = input('Enter your message: ');
                MQTT_client.publish(MQTT_publish_topic, Publisher_msg);
                pass;

except KeyboardInterrupt:
        MQTT_client.disconnect();
        MQTT_client.loop_stop();
