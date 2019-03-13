/*
*
*/

const int ledPin = 13;      // the pin for the LED
const int inputPin = 2;     // the input pin for PIR sensor
const int relayPin = 4;     // connect the relay to this pin
int motionSensorInput = 0;  // variable for reading the pin status

/*
 * Perform configuration and hardware setup when the board powers on.
 */
void setup() {
  pinMode(ledPin, OUTPUT);      // declare LED as output
  pinMode(inputPin, INPUT);     // declare sensor as input
  pinMode(relayPin, OUTPUT);    // declare relay as output
}

/*
 * The loop that does the work.
 */
void loop() {
  //read the input from the motion sensor
  motionSensorInput = digitalRead(inputPin);
  
  if (motionSensorInput == HIGH)
  {    
    //turn the LED on, drop the relay
    digitalWrite(ledPin, HIGH);
    digitalWrite(relayPin, LOW);
  } 
  else
  {
    //turn the LED off, drive the relay
    digitalWrite(ledPin, LOW);
    digitalWrite(relayPin, HIGH);
  }
}
