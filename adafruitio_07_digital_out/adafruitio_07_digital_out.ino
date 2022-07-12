
#include "config.h"  

// headers for DH11 sensor 
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h> 

// headers for Pan Tilt servo 
#include <Servo.h>  

#define LED_PIN 5   // pin 5 for our LED lights 
#define DATA_PIN 2  // pin 2 for our DH11 sensor   

// Declare variables to keep track of time for DH11 sensor
unsigned long previousTimeDH11 = millis(); 
long timeIntervalDH11 = 5000;  

// Sets up test for one servo which will allow us to test the servo limit and 
// center point by issuing command through the serial monitor
int    servoPIN = 12; //servo signal pin
int    servoPos = 90; //initial servo position
String inString = ""; //string to hold incomming command
Servo  servo;         //create servo object to control the servo 

// creates our DHT11 instance
DHT_Unified dht(DATA_PIN, DHT11);

// sets up our Adafruit IO feeds 
AdafruitIO_Feed *digital = io.feed("Digital");
AdafruitIO_Feed *temperature = io.feed("Temperature");
AdafruitIO_Feed *humidity = io.feed("Humidity");

void setup() {
  
  pinMode(LED_PIN, OUTPUT);  
  
  servo.attach(servoPIN); // attaches the servo pin to the servo object
  servo.write(servoPos);  // set servo to initial position
  
  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial); 

  // initialize dht11
  dht.begin();

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect(); 

  // set up a message handler for the 'digital' feed.
  // handleMessage function will be called whenever a message is received from adafruit io.
  digital->onMessage(handleMessage);  
  
  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText()); 
  digital->get();  

  Serial.println("");
  Serial.println("-----------------------------------------------------------------------");
  Serial.println("- Welcome to Interactive Servo");
  Serial.println("- Open the serial monitor and ensure the Line Ending option (at bottom");
  Serial.println("- of the monitor window) is set to NewLine. To move the servo, enter a ");
  Serial.println("- number from 0 to 180(at top of monitor window)and press ENTER/SEND");
  Serial.println("-----------------------------------------------------------------------");
  Serial.print("Servo at "); Serial.print(servoPos); Serial.println(".\tNew position?");
}

void loop() {

  // keeps the client connected to io.adafruit.com, and processes any incoming data.
  io.run();  

  // Gets the current time to allow us to perform time tracking techniques to trigger an action  
  unsigned long currentTime = millis(); 

  // Record data of Humidity and Temperature Sensor every five seconds   
  if (currentTime - previousTimeDH11 > timeIntervalDH11) { 
    previousTimeDH11 = currentTime;  
    
    sensors_event_t event;
    dht.temperature().getEvent(&event);
  
    float celsius = event.temperature;
    float fahrenheit = (celsius * 1.8) + 32; 

    /*
    Serial.print("celsius: ");
    Serial.print(celsius);
    Serial.println("C");
  
    Serial.print("fahrenheit: ");
    Serial.print(fahrenheit);
    Serial.println("F"); 
    */ 
  
    // save fahrenheit (or celsius) to Adafruit IO
    temperature->save(fahrenheit);
  
    dht.humidity().getEvent(&event);  

    /*
    Serial.print("humidity: ");
    Serial.print(event.relative_humidity);
    Serial.println("%"); 
    */
  
    // save humidity to Adafruit IO
    humidity->save(event.relative_humidity);   
  } 

  // Our interactive servo test
  while (Serial.available()) {
    char inChar = Serial.read();
    if(inChar == '\n')  {
      int num = inString.toInt();
      inString = "";

      /*if valid number move servo*/
      if((num >= 0) && (num <= 180)) {
        for(int i = servoPos; i != num; i += (servoPos < num? 1 : -1)) {
          servo.write(i); delay(15); //move servo +/- 1 degree every 15 msec
        }
        servoPos = num; //update servo position
        Serial.print("Servo at "); Serial.print(servoPos); Serial.println(".\tNew position?");
      }
      else { Serial.print(num); Serial.println(" not valid.  Must be between 0 and 180"); }
    }

    /*add char to string if string not too long and valid digit*/
    else {
      if(inString.length() > 2) continue;
      if(isDigit(inChar)) inString += inChar;
    }
  } 
}

// this function is called whenever an 'digital' feed message is received from Adafruit IO
void handleMessage(AdafruitIO_Data *data) {
  /*
  Serial.print("received <- ");

  if(data->toPinLevel() == HIGH)
    Serial.println("HIGH");
  else
    Serial.println("LOW");  
  */

  digitalWrite(LED_PIN, data->toPinLevel());
}
