/*
*
*/

const int ledPin = 13;      // the pin for the LED
const int pirPin = 2;     // the input pin for PIR sensor
const int relayPin = 4;     // connect the relay to this pin
int motionSensorInput = 0;  // variable for reading the pin status

/*
 * Perform configuration and hardware setup when the board powers on.
 */
void setup() {
  pinMode(ledPin, OUTPUT);      // declare LED as output
  pinMode(pirPin, INPUT);     // declare sensor as input
  pinMode(relayPin, OUTPUT);    // declare relay as output
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
}
