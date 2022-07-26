import cv2 
import numpy as np 
import urllib.request 


stream = urllib.request.urlopen('http://192.168.2.128/stream');		# ip address of ESP8266  
bytes = bytes(); 

while True: 
    bytes += stream.read(1024)  
    a = bytes.find(b'\xff\xd8');     # JPEG start  
    b = bytes.find(b'\xff\xd9');     # JPEG end 

    if  a!=-1 and b!=-1: 
        jpg = bytes[a:b+2];         # actual image 
        bytes = bytes[b+2:];        # other informations  

        # decode to colored image 
        img = cv2.imdecode(np.fromstring(jpg, dtype = np.uint8), cv2.IMREAD_COLOR); 
        cv2.imshow('Cam', img)      # displays image while receiving data   

        if cv2.waitKey(1) == 27:     # if user hits the esc key 
            exit(0);

