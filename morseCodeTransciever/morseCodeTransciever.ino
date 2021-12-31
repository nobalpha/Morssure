//TODO:
// Refactoring
// Commenting
// Debugging

#include <math.h>
#include <SoftwareSerial.h>

SoftwareSerial BTSerial(10, 11);

// const int buzzerPin = 3;
// const int ledPin = 9;
const int vibratorPin = 6;
const int sensorPin = A0;
const int activatorPin = 2;
const int threshold = 500.0;
const int millisecond = 1000;
const float wpm = 1.0; // 1 wpm = "PARIS " per minute => 50 units per minute
const float ups = 5.0 / 6.0 * wpm; // 5 units per 6 seconds
const float timeunit = millisecond / ups; // milliseconds per unit
const float padding = 1.0 / 6.0 * timeunit;
const float filter_limit = 100.0;

/* Morse Code Timing Table */
// . 1 tu
// - 3 tu
// US 1 tu (Unit Space)
// CS 3 tu (Character Space)
// WS 7 tu (Word Space)

const int timingtable[5] = {1, 3, 1, 3, 7};
const char CUSTOM_ENCODING_TABLE[5] = {'.', '-', 'U', 'W', 'C'};

float value, delta_time;
float initial_time, final_time = millis();
bool started = false;

String morseCode = "";

void dataTransmitter(int mode, float delta_time) { // Sends data if code is C
  char code = deltaTimeCalculator(mode, delta_time);
  if (code == 'e' || code == NULL || code == ' ' || code == 0) return;
  if (code == 'C') {
    // Send morseCode, empty it
    Serial.print("Sending the code now. \t\t");
    Serial.println(morseCode);
    bufferResetter();
  } else if (code == '-' || code == '.' || code == 'U' || code == 'W') { // I didn't understand why there are some nully data passing on... also some duplicates, not sure if it's due to the accuracy of sensor
    // Buffer morseCode
    int encoded = customEncoder(code);
    Serial.print("Code: \t\t");
    Serial.println(code);
    Serial.print("Encoded: \t");
    Serial.println(encoded);
    bufferConstructor(encoded);
  }
}

void bufferResetter() {
  morseCode = "";
}

void bufferConstructor(char code) {
  morseCode += code;
}


int customEncoder(char code) {
  for (int i = 0; i < sizeof(CUSTOM_ENCODING_TABLE) / sizeof(CUSTOM_ENCODING_TABLE[0]); i++) {
    if (code == CUSTOM_ENCODING_TABLE[i]) return i + 1 + '0' ;
  }
}

char deltaTimeCalculator(int mode, float delta_time) {
  char output;
  // Character
  if (mode == 1) {
    if ((timeunit * timingtable[1]) - padding <= delta_time) {
      output = '-';
    } else if (filter_limit <= delta_time) {
      output = '.';
    }
  }
  // Space
  else if (mode == 0) {
    if ((timeunit * timingtable[4]) - padding <= delta_time) {
      output = 'W';
    }
    else if ((timeunit * timingtable[3]) - padding <= delta_time) {
      output = 'C';
    } else if (filter_limit <= delta_time) {
      output = 'U';
    }
  } else {
    Serial.println("[ERROR] Invalid mod number...");
    return 'e'; // error
  }
  return output;
}

void setup() {
  pinMode(vibratorPin, OUTPUT);
  pinMode(activatorPin, OUTPUT);
  pinMode(sensorPin, INPUT);

  Serial.begin(9600);
  BTSerial.begin(38400);  // HC-05 default speed in AT command more

  Serial.print("Timeunit: ");
  Serial.println(timeunit / millisecond);
  Serial.print("Padding: ");
  Serial.println(padding);
}

void loop() {

  while (analogRead(sensorPin) < threshold);

  if (!started) {
    initial_time = final_time = millis();
    Serial.println("Not started yet");
  }
  else final_time = millis();

  final_time = millis();

  digitalWrite(vibratorPin, 1);

  delta_time = final_time - initial_time;

  dataTransmitter(0, delta_time);

  initial_time = final_time;

  started = true;

  while (analogRead(sensorPin) > threshold); // loop if no signal

  final_time = millis();

  digitalWrite(vibratorPin, 0);

  delta_time = final_time - initial_time;

  dataTransmitter(1, delta_time);

  initial_time = final_time;



}
