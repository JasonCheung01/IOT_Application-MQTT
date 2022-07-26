void setup() {
  pinMode(0, OUTPUT); // initialize pin gpio 0 as output
}

void loop() { 
  // ESP8266 LED is reversed wired 
  digitalWrite(0, HIGH); // will turn the LED lights off 
  delay(500);
  digitalWrite(0, LOW); // will turn the LED lights on 
  delay(500);
}
