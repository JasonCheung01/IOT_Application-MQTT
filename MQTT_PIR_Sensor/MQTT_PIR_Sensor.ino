int PIRSensor = 12;  
int relaySensor = 14; 
int sensorValue = 0;

void setup() { 
  Serial.begin(9600);
  pinMode(PIRSensor, INPUT); 
  pinMode(relaySensor, OUTPUT);  
}

void loop() {  
  sensorValue = digitalRead(PIRSensor); 
  
  if (sensorValue == HIGH) {  
      Serial.println("Motion Detected");
      digitalWrite(relaySensor, HIGH);
  } else {    
    digitalWrite(relaySensor, LOW);
  }
}
