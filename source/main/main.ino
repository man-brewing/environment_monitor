/*
   @author: ndykstra
   @date: 8/5/2021
*/

#include "secrets.h"
#include <Wire.h>
#include <SPI.h>
#include "Adafruit_SHT31.h"
#include <Ethernet.h>

const int pirPin = 8;                   // the input pin for PIR sensor
const int relayPin = 9;                 // connect the relay to this pin
int motionSensorInput = 0;              // variable for reading the pin status
long environmentInterval = 15 * 60000;   // minutes between environment readings
long previousMillis = 0;                // last time sensor was recorded

Adafruit_SHT31 sht31 = Adafruit_SHT31();

EthernetClient client;
char server[] = "manbrewingapi.azurewebsites.net";
IPAddress ip(192, 168, 1, 130);
IPAddress myDns(1, 1, 1, 1);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

/*
   Perform configuration and hardware setup when the board powers on.
*/
void setup() {  
  Ethernet.init(10);

  delay(3000);

  pinMode(pirPin, INPUT);       // declare sensor as input
  pinMode(relayPin, OUTPUT);    // declare relay as output

  // make sure the temp/humid sensor is there
  if (!sht31.begin(0x44))
  {
    while (1);
  }

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  }

  // give the Ethernet shield some time to initialize:
  delay(3000);
}

/*
   The loop that does the work.
*/
void loop() {
  //only read environment sensors at set intervals
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > environmentInterval || previousMillis == 0)
  {
    //read the input from the motion sensor
    motionSensorInput = digitalRead(pirPin);

    //write the sensor output to the relay
    digitalWrite(relayPin, motionSensorInput);

    previousMillis = currentMillis;
    float t = sht31.readTemperature();
    float h = sht31.readHumidity();

    // if you get a connection, report back via serial:
    if (client.connect(server, 80))
    {
      Serial.println("connected to server");
      String postData = "temp=" + String(t) + "&humidity=" + String(h);
      client.println("POST /beerroom/environment/" + String(AUTHTOKEN) + " HTTP/1.1");
      client.println("User-Agent: arduino-ethernet");
      client.println("Host: manbrewingapi.azurewebsites.net");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.println("Connection:close");
      client.print("Content-Length:");
      client.println(postData.length());
      client.println();
      client.print(postData);
    } else {
      //can't really do much if we didn't connect
    }
    if (!client.connected()) {
      client.stop();
    }
  }
}
