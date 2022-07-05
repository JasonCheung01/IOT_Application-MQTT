const int PIRSensorOutPin = 12;  

void setup() { 
  Serial.begin(9600); 
  pinMode(PIRSensorOutPin, INPUT); 
}

void loop() { 
  if (digitalRead(PIRSensorOutPin) == LOW)
    {
       Serial.println("Person detected!");
       exit();//Print to serial monitor
    }
    else { }//Serial.println("No detection");}
}
