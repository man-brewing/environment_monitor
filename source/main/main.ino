/*
 * @author: ndykstra
 * @date: 3/25/2019
 */
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "Adafruit_SHT31.h"

const int chipSelect = 8;               // SD card CS pin
const int pirPin = 9;                   // the input pin for PIR sensor
const int relayPin = 7;                 // connect the relay to this pin
int motionSensorInput = 0;              // variable for reading the pin status
long environmentInterval = 1 * 60000;   // minutes between environment readings
long previousMillis = 0;                // last time sensor was recorded
bool serialDebuggingEnabled = false;    // enable/disable serial messaging

Adafruit_SHT31 sht31 = Adafruit_SHT31();

/*
 * Perform configuration and hardware setup when the board powers on.
 */
void setup() {
  if (serialDebuggingEnabled)
  {
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }

    Serial.println("Configuring board and sensors");
  }
    
  pinMode(pirPin, INPUT);       // declare sensor as input
  pinMode(relayPin, OUTPUT);    // declare relay as output

  // make sure the temp/humid sensor is there
  if (!sht31.begin(0x44))
  {
    if (serialDebuggingEnabled)
    {
      Serial.println("Couldn't find SHT31");
    }
    
    while (1) delay(1);
  }

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    if (serialDebuggingEnabled)
    {
      Serial.println("Card failed, or not present");
    }
    
    // don't do anything more:
    while (1);
  }
  if (serialDebuggingEnabled)
  {
    Serial.println("Setup complete");
  }
}

/*
 * The loop that does the work.
 */
void loop() {  
  //read the input from the motion sensor
  motionSensorInput = digitalRead(pirPin);

  //write the sensor output to the relay
  digitalWrite(relayPin, motionSensorInput);

  //only read environment sensors at set intervals
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > environmentInterval) 
  {
    previousMillis = currentMillis;

    String dataString = "";
    
    float t = sht31.readTemperature();
    float h = sht31.readHumidity();

    dataString = String(t) + "," + String(h);
      
    if (serialDebuggingEnabled)
    {
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

    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File dataFile = SD.open("datalog.txt", FILE_WRITE);// if the file is available, write to it:
    if (dataFile)
    {
      dataFile.println(dataString);
      dataFile.close();
    }
    // if the file isn't open, pop up an error:
    else
    {
      if (serialDebuggingEnabled)
      {
        Serial.println("error opening datalog.txt");
      }
    }
  }
}
