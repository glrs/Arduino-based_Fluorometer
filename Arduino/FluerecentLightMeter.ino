/*
 FluerecentLightMeter
 It uses a TSL250R-LF light sensor to measure the light emitted from a protein
 sample. The LEDs trigger the sample (different light emitting proteins may be
 triggered by different wavelength, so a different LED color is needed) and it
 emits a fluorescent light. As the light sensor detects a wide band of
 wavelengths, a specific light filter is needed to let only the fluorescent
 light to pass to the light sensor, and to block the LED light. (A dark chamber
 or a dark place is needed in order to avoid external noise.)

 The circuit:
 * TSL250R-LF light sensor analog output pin attached to analog input 0
 * 10K resistor attached to analog input 0 from ground
 * Ground pin of the sensor to ground
 * Vcc pin of the sensor to +5V
 * LED anodes (long legs) attached to digital output 11 and 12
 * LED cathodes (short legs) attached to ground
 * Each LED has a 220ohm resistor to its anode
 * Push button attached to pin 2 from +5V
 * 10K resistor attached to pin 2 from ground


 Created by Anastasios Glaros
 
*/

const int buttonPin = 2;    // the number of the pushbutton pin
int buttonState = 0;        // variable for reading the pushbutton status
int sensorPin = A0;         // select the input pin for the potentiometer

// select the pin for the LEDs
int leftInLed = 12;
int rightInLed = 11;

int sensorValue = 0;        // variable to store the value coming from the sensor

void setup() {
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  
  // start serial port at 9600 bps:
  Serial.begin(9600);
  
  // declare the LED pins as an OUTPUT:
  pinMode(leftInLed, OUTPUT);
  pinMode(rightInLed, OUTPUT);
}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    for(int i = 0; i < 100; i++){
      measurement();
    }
  }
}

void measurement() {
  // Turn on the LEDs
  digitalWrite(leftInLed, HIGH);
  digitalWrite(rightInLed, HIGH);
  
  // Give a small delay in order for the LEDs to physically turned on
  delay(4);
  
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);

  // Turn off the LEDs
  digitalWrite(leftInLed, LOW);
  digitalWrite(rightInLed, LOW);

  // Send the value of analog input 0:
  Serial.println(sensorValue);      // Multiplication of the signal may be needed (e.g. 10x)
}
