//collects data from an analog sensor

int sensorpin = A0;           // ADC port for Assembled Feather ESP8266
int val = 0;                 // variable to store the values from sensor(initially zero)

void setup()
{
  Serial.begin(9600);               // starts the serial monitor
}
 
void loop()
{
  val = analogRead(sensorpin);       // reads the value of the sharp sensor
  Serial.println(val);            // prints the value of the sensor to the serial monitor
  delay(1000);                    // wait for this much time before printing next value
}
