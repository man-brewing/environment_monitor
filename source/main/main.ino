/*
 * @author: ndykstra
 * @date: 3/25/2019
 */
//#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
// #include <SD.h>
#include "Adafruit_SHT31.h"
#include <Ethernet.h>

const int pirPin = 8;                   // the input pin for PIR sensor
const int relayPin = 9;                 // connect the relay to this pin
int motionSensorInput = 0;              // variable for reading the pin status
long environmentInterval = 1 * 5000;   // minutes between environment readings
long previousMillis = 0;                // last time sensor was recorded

Adafruit_SHT31 sht31 = Adafruit_SHT31();

EthernetClient client;
char server[] = "krimthered.ddns.net";
IPAddress ip(192, 168, 1, 130);
IPAddress myDns(8, 8, 8, 8);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

/*
 * Perform configuration and hardware setup when the board powers on.
 */
void setup() {
  Ethernet.init(10);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Configuring board and sensors");
    
  pinMode(pirPin, INPUT);       // declare sensor as input
  pinMode(relayPin, OUTPUT);    // declare relay as output

  // make sure the temp/humid sensor is there
  if (!sht31.begin(0x44))
  {  
    Serial.println("SHT31 not configured");
    while (1);
  }

  Serial.println("SHT31 configured");

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  }
  
  // give the Ethernet shield a second to initialize:
  delay(3000);

  // see if the card is present and can be initialized:
//  if (!SD.begin(4)) {
//    while (1);
//  }

  Serial.print("My IP is ");
  Serial.println(Ethernet.localIP());
  Serial.println("Setup complete");  
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
    float t = sht31.readTemperature();
    float h = sht31.readHumidity();

    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
//    File dataFile = SD.open("datalog.txt", FILE_WRITE);// if the file is available, write to it:
//    if (dataFile)
//    {
//      dataFile.println(dataString);
//      dataFile.close();
//    }

    // if you get a connection, report back via serial:
    if (client.connect(server, 80))
    {
      String postData = "temp=" + String(t) + "&humidity=" + String(h);
      Serial.println("post");
      client.println("POST /beerroom/environment HTTP/1.1");
      client.println("User-Agent: arduino-ethernet");
      client.println("Host: 192.168.1.115:3000");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.println("Connection:close");
      client.print("Content-Length:");
      client.println(postData.length());
      client.println();
      client.print(postData);
    } else {
      Serial.println("connection failed");
    }
    if (!client.connected()) {
      Serial.println("disconnecting.");
      client.stop();
    }
  }
}
