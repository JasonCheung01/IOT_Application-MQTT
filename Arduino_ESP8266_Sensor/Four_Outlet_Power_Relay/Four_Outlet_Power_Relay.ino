int relay = D0;              // Tells Arduino the relay is connected to pin 13

void setup()
{
pinMode(relay, OUTPUT);      // Initialize the Atmel GPIO pin as an output
}

void loop()                  // Loops forever
{
digitalWrite(relay, HIGH);   // Turn the relay on (HIGH is the voltage level = 1)
delay(5000);                 // Stay ON for 5 seconds
digitalWrite(relay, LOW);    // Turn the relay off by making the voltage LOW = 0
delay(5000);                 // Stay OFF for 5 seconds
} 
