/*
*
*/
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"

Adafruit_SHT31 sht31 = Adafruit_SHT31();

const int ledPin = 13;                  // the pin for the LED
const int pirPin = 2;                   // the input pin for PIR sensor
const int relayPin = 4;                 // connect the relay to this pin
int motionSensorInput = 0;              // variable for reading the pin status
long environmentInterval = 30 * 60000;  // minutes between environment readings
long previousMillis = 0;

/*
 * Perform configuration and hardware setup when the board powers on.
 */
void setup() {
  Serial.begin(9600);
    
  pinMode(ledPin, OUTPUT);      // declare LED as output
  pinMode(pirPin, INPUT);       // declare sensor as input
  pinMode(relayPin, OUTPUT);    // declare relay as output

  // make sure the temp/humid sensor is there
  if (!sht31.begin(0x44))
  {
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }
}

/*
 * The loop that does the work.
 */
void loop() {  
  //read the input from the motion sensor
  motionSensorInput = digitalRead(pirPin);

  //write the sensor output to the LED and relay
  digitalWrite(ledPin, motionSensorInput);
  digitalWrite(relayPin, motionSensorInput);

  //
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > environmentInterval) 
  {
    previousMillis = currentMillis;
    
    float t = sht31.readTemperature();
    float h = sht31.readHumidity();
  
    if (!isnan(t)) 
    {
      Serial.print("Temp *C = "); 
      Serial.println(t);
    } 
    else 
    { 
      Serial.println("Failed to read temperature");
    }
    
    if (! isnan(h))
    {  // check if 'is not a number'
      Serial.print("Hum. % = "); Serial.println(h);
    }
    else
    { 
      Serial.println("Failed to read humidity");
    }
  }
}
