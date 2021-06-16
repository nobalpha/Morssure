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
const float timespeed = 1.0;
const int millisecond = 1000;
const float wpm = 1.0; // 1 wpm = "PARIS " per minute = 50 units per minute
const float timeunit = 6.0 / 5.0 / wpm * millisecond;
const float padding = 1.0 / 4.0 * timeunit;

/* Morse Code Timing Table */
// . 1 tu
// - 3 tu
// SS 1 tu
// SL 3 tu
// SW 7 tu

const int timingtable[5] = {1, 3, 1, 3, 7};

float value, delta_time;
float initial_time, final_time = millis();

String morseCode;

void morseToPulse(char* morseCode) {
    for (int i = 0; i < sizeof(morseCode); i++) {
      if (morseCode[i] == '.') {
        Serial.println(morseCode[i]);
        digitalWrite(vibratorPin, 1);
        delay(timingtable[0] * timeunit);  
        digitalWrite(vibratorPin, 0);
        delay(timingtable[0] * timeunit);  
      } else if (morseCode[i] == '-') {
        digitalWrite(vibratorPin, 1);
        delay(timingtable[1] * timeunit);  
        digitalWrite(vibratorPin, 0);  
        delay(timingtable[0] * timeunit);  
      }
    }
}

void timeunitToMorse(int mode, float delta_time) {

  if (mode == 1) {
      if ((timeunit * timingtable[1]) - padding <= delta_time) {
        Serial.println('-');
        morseCode += '-';
      } else if (50 <= delta_time) { 
        morseCode += '.';
        Serial.println('.');

      }
  } else if (mode == 0) {
      if ((timeunit * timingtable[3]) - padding <= delta_time) {
        // Serial.println("Inter-character Space");
        if (morseCode.length()) {
          // Send bluetooth
          Serial.println("[OK] Starting bluetooth transaction...");
            Serial.println(morseCode);
            int size = morseCode.length();
            char copy[size];
            strcpy(copy, morseCode.c_str());
            Serial.println(copy); 
            char str[80];
            sprintf(str, "%s", morseCode);
            puts(str);
            BTSerial.write(copy, size);
              
          morseCode = "";
        }
        return;
      } else if (50 <= delta_time) {
        Serial.println("Intra-character Space");
        
      }
  } else {
    Serial.println("[ERROR] Invalid function call.");
  }
  

}


void setup(){
  
  // pinMode(ledPin, OUTPUT);
  // pinMode(buzzerPin, OUTPUT);
  pinMode(vibratorPin, OUTPUT);
  pinMode(activatorPin, OUTPUT);
  digitalWrite(activatorPin, 1);
  
  pinMode(sensorPin, INPUT);
  
  // morseToPulse(".-");

  Serial.begin(9600);  
  BTSerial.begin(38400);  // HC-05 default speed in AT command more
  
  Serial.print("Timeunit: ");
  Serial.println(timeunit);
  Serial.print("Padding: ");
  Serial.println(padding);

}

void loop(){
  
  while(analogRead(sensorPin) < threshold);
  
  // tone(buzzerPin, 2000);
  
  initial_time = millis() * timespeed;
  
  delta_time = initial_time - final_time;
  
  // Serial.print("Low time ");
  // Serial.println(delta_time); // t1 - t2
  //Serial.println(delta_time / timespeed); // t1 - t2
  
  timeunitToMorse(0, delta_time);
  // delay(padding);
  
  while(analogRead(sensorPin) > threshold);
  
  // noTone(buzzerPin);
  
  final_time = millis() * timespeed;
  
  delta_time = final_time - initial_time;
  
  // Serial.print("High time ");
  // Serial.println(delta_time); // t2 - t1
  //Serial.println(delta_time / timespeed); // t2 - t1
  
  timeunitToMorse(1, delta_time);
  // delay(padding);
  
  // Serial.println(round(delta_time / timespeed / 60.0)); // Using speed because of the accuracy of Tinkercad's emulation timer.

 
  
}
