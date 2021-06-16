#include <SoftwareSerial.h>
#include <String.h>

SoftwareSerial BTSerial(10, 11);

const int vibratorPin = A0;

const float timespeed = 1.0;
const int millisecond = 1000;
const float wpm = 5.0 / 6.0; // 1 wpm = "PARIS " per minute = 50 units per minute
const float timeunit = 6.0 / 5.0 * wpm * millisecond;

/* Morse Code Timing Table */
// . 1 tu
// - 3 tu
// SS 1 tu
// SL 3 tu
// SW 7 tu

const int timingtable[5] = {1, 3, 1, 3, 7};

char *morseCode, *latinCode;

const char* morseAlphabet[26] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--",
  "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."
};

char morseToLatin(char* morseCode) {
  for (int i = 0; i < sizeof(morseAlphabet); i++) {
    if (morseCode == morseAlphabet[i]) {
      Serial.println(char(65 + i));
      return char(65 + i);
    }
  }
}

void morseToPulse(char* morseCode) {
  for (int i = 0; i < sizeof(morseCode); i++) {
    if (morseCode[i] == '.') {
      Serial.println(morseCode[i]);
      digitalWrite(vibratorPin, 1);
      Serial.println(timingtable[0] * timeunit);
      delay(timingtable[0] * timeunit);
      digitalWrite(vibratorPin, 0);
    } else if (morseCode[i] == '-') {
      digitalWrite(vibratorPin, 1);
      delay(timingtable[1] * timeunit);
      digitalWrite(vibratorPin, 0);
    }
  }
}

void setup()
{
  // pinMode(9, OUTPUT);
  // digitalWrite(9, HIGH);
  pinMode(vibratorPin, OUTPUT);

  Serial.begin(9600);
  //morseToLatin(".-");
  morseToPulse(".-");
  BTSerial.begin(38400);  // HC-05 default speed in AT command more
}

void loop()
{
  // Keep reading from HC-05 and send to Arduino Serial Monitor
  if (BTSerial.available() > 0) {
    char buffer = BTSerial.read();
    Serial.println();
    Serial.println(morseCode);
    morseToPulse(morseCode);
  }


}
