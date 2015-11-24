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

// the number of the pushbutton pins
const int startButtonPin = 2;
const int modeButtonPin = 3;

const int timelapseLED = 10;
const int comparisonLED = 9;
const int leftInLed = 12;
const int rightInLed = 11;

const int m_size = 200;
const int meanFiltSize = 8;

int runs = 1;
int mode = 1;
int ledToggle = 0;
int startButton = 0;         // variable for reading the pushbutton status
int modeButton = 0;

int sensorPin = A0;    // select the input pin for the light sensor

float sensorValues[m_size];

void setup() {
  pinMode(startButtonPin, INPUT);
  pinMode(modeButtonPin, INPUT);

  // declare the ledPin as an OUTPUT:
  pinMode(timelapseLED, OUTPUT);
  pinMode(comparisonLED, OUTPUT);

  pinMode(leftInLed, OUTPUT);
  pinMode(rightInLed, OUTPUT);

  // start serial port at 9600 bps:
  Serial.begin(9600);

  //analogReference(INTERNAL);
  //digitalWrite(comparisonLED, HIGH);
  //digitalWrite(timelapseLED, LOW);
  checkMode();
}

void loop() {
  startButton = digitalRead(startButtonPin);
  modeButton = digitalRead(modeButtonPin);
  delay(150);
  
  if(modeButton == HIGH){
    if(mode == HIGH){
      mode = LOW;
      digitalWrite(comparisonLED, LOW);
      digitalWrite(timelapseLED, HIGH);
    }
    else{
      mode = HIGH;
      digitalWrite(comparisonLED, HIGH);
      digitalWrite(timelapseLED, LOW);
    }
  }

  if(startButton == HIGH){
    if(mode == HIGH){
      for(int i = 0; i < 20; i++){
        digitalWrite(comparisonLED, LOW);
        delay(20);
        digitalWrite(comparisonLED, HIGH);
        delay(20);
      }
      comparisonMode();
    }
    else{
      for(int i = 0; i < 20; i++){
        digitalWrite(timelapseLED, LOW);
        delay(20);
        digitalWrite(timelapseLED, HIGH);
        delay(20);
      }
      timelapseMode();
    }
    runs++;
    checkMode();
  }
}

void checkMode(){
  if(mode == HIGH){
    digitalWrite(comparisonLED, HIGH);
    digitalWrite(timelapseLED, LOW);
  }
  else{
    digitalWrite(comparisonLED, LOW);
    digitalWrite(timelapseLED, HIGH);
  }
  Serial.print("Run No. ");
  Serial.println(runs);
}

void comparisonMode(){
  long blinkMillis = 0;
  int blinkSpeed = 300;
  long samplingMillis = 0;

  memset(sensorValues, 0, sizeof(sensorValues));
  
  measurement(blinkMillis);

  for(int i = 0; i < 20;){
    if(millis() - samplingMillis > 10000){
      samplingMillis = millis();
      measurement(blinkMillis);
      i = i + 1;
      Serial.println(meanFilter());
      //minutes = sendData(minutes, interval_min);
    }
    blinkMillis = ledBlink(blinkMillis, blinkSpeed);

    //sendData();
  }
}

void timelapseMode(){
  int interval_min = 5;
  int minutes = 0;
  
  memset(sensorValues, 0, sizeof(sensorValues));

  long samplingMillis = 0;
  long blinkMillis = 0;
  int blinkSpeed = 300;

  measurement(blinkMillis);
  minutes = sendData(minutes, interval_min);
  
  for(int i = 0; i < 180;){
    if(millis() - samplingMillis > 300000){
      samplingMillis = millis();
      measurement(blinkMillis);
      i = i + 1;

      minutes = sendData(minutes, interval_min);
    }
    blinkMillis = ledBlink(blinkMillis, blinkSpeed);
  }
}

int sendData(int minutes, int interv){
  Serial.print(minutes);
  minutes = minutes + interv;
  Serial.print(" ");
  Serial.println(meanFilter());
  memset(sensorValues, 0, sizeof(sensorValues));
  return minutes;
}


void measurement(long blinkMillis) {
  digitalWrite(leftInLed, HIGH);   // Turn on the LEDs
  digitalWrite(rightInLed, HIGH);  // Turn on the LEDs
  delay(400);   // Put a small delay in order for the LEDs to physically turned on

  for(int i = 0; i < m_size; i++){
    int rd = analogRead(sensorPin);
    delay(5);
    //Serial.print(rd);
    //Serial.print(" ");
    insertionSort(rd, i);
    //blinkMillis = ledBlink(blinkMillis);
    //delay(10);
  }
  
  digitalWrite(leftInLed, LOW);   // Turn off the LEDs
  digitalWrite(rightInLed, LOW);
}


void insertionSort(int value, int i){
  if(i != 0){
    for(int c = i; c >= 0; c--){
      if(value >= sensorValues[c-1]){
        if(sensorValues[c] == 0){
          sensorValues[c] = value;
        }
      }
      else{
        sensorValues[c] = sensorValues[c-1];
        sensorValues[c-1] = value;
      }
    }
  }
  else {
    sensorValues[i] = value;
  }
}


float meanFilter(){
  //Serial.println();
  //for(int i = 0; i < m_size; i++){
  //  Serial.print(sensorValues[i]);
  //  Serial.print(" ");
  //}
  //Serial.println();
  float meanVal = 0;
  for(int i = (m_size / 2) - (meanFiltSize / 2); i < (m_size / 2) + (meanFiltSize / 2); i++){
    meanVal += sensorValues[i];
  }
  meanVal = meanVal / meanFiltSize;
  return meanVal;
}


long ledBlink(long blinkMillis, int blinkSpeed){
  if(millis() - blinkMillis > blinkSpeed){
    blinkMillis = millis();
    if(ledToggle){
      if(mode){
        digitalWrite(comparisonLED, LOW);
      }
      else{
        digitalWrite(timelapseLED, LOW);
      }
      ledToggle = 0;
    }
    else{
      if(mode){
        digitalWrite(comparisonLED, HIGH);
      }
      else{
        digitalWrite(timelapseLED, HIGH);
      }
      ledToggle = 1;
    }
  }
  return blinkMillis;
}

