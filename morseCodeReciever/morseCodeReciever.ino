#include <SoftwareSerial.h>
//#include <String.h>

SoftwareSerial BTSerial(10, 11);

const int vibratorPin = 7;

const int millisecond = 1000;
const float wpm = 2.0; // 1 wpm = "PARIS " per minute => 50 units per minute
const float ups = 5.0 / 6.0 * wpm; // 5 units per 6 seconds
const float timeunit = millisecond / ups; // milliseconds per unit

/* Morse Code Timing Table */
// . 1 tu
// - 3 tu
// US 1 tu (Unit Space)
// CS 3 tu (Character Space)
// WS 7 tu (Word Space)

const int timingtable[5] = {1, 3, 1, 3, 7};

char morseCode[] = "", latinCode[] = "";

const char* morseAlphabet[26] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--",
  "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."
};

const char CUSTOM_ENCODING_TABLE[5] = {'.', '-', 'U', 'W', 'C'};

void dataParser(String encoded) {
  const int encoded_length = encoded.length();
  // Serial.println(encoded_length);
  String decoded = encoded;
  for (int i = 0; i < encoded_length; i++) {
    decoded.setCharAt(i, customDecoder(encoded[i]));
    // Serial.println(decoded);
  }

  Serial.println(decoded);

  decoded.replace("U", ""); // Implement it with char array in order to get rid of string type...
  Serial.println(decoded);
  morseToPulse(decoded);
  Serial.println(splitter('W', decoded));

}

/*char** split(char seperator, String string) {
  int occurences = count(seperator, string);
  char chunks[occurences + 1][occurences];
  for (int i = 0, j=0, c=0; i < string.length(); i++) {
    if (string.charAt(i) == seperator) {
      string.substring(j, i-1).toCharArray(chunks[c], occurences);
      ++i, c++, j=i;
    }
  }
  return chunks;
  }*/

char* splitter(char seperator, String string) {
  const int occurences = count(seperator, string) + 2;
  char* output;
  output = (char*)calloc(occurences, sizeof(char));
  for (int i = 0, j = 0, c = 0; i <= string.length(); i++) {
    if (string.charAt(i) == seperator || i == string.length()) {
      char chr = morseToLatin(string.substring(j, i));
      output[c] = chr;
      ++i, ++c, j = i;
    }
  }
  return output;
}


int count(char chr, String string) {
  int counter = 0;
  for (int i = 0; i < string.length(); i++) {
    if (string[i] == chr) counter++;
  }
  return counter;
}

/*char* str_remove(char chr, char string[]) {
  const int str_len = strlen(string);
  char removed[str_len - count(chr, string)];
  for (int i = 0, j = 0; i < str_len; i++) {
    if (string[i] == chr) {
      removed[j] = customDecoder(string[i]);
      j++;
    }
  }
   return removed;
  }*/

char morseToLatin(String morseCode) {
  for (int i = 0; i < sizeof(morseAlphabet) / sizeof(morseAlphabet[0]); i++) {
    if (morseCode == morseAlphabet[i]) {
      // Serial.println(char(65 + i));
      return char(65 + i);
    }
  }
}

char customDecoder(char code) {
  return CUSTOM_ENCODING_TABLE[code - '0' - 1];
}

void morseToPulse(String morseCode) {
  for (int i = 0; i < morseCode.length(); i++) {
    if (morseCode.charAt(i) == '.') {
      digitalWrite(vibratorPin, 1);
      delay(timingtable[0] * timeunit);
      digitalWrite(vibratorPin, 0);
    } else if (morseCode.charAt(i) == '-') {
      digitalWrite(vibratorPin, 1);
      delay(timingtable[1] * timeunit);
      digitalWrite(vibratorPin, 0);
    } else if (morseCode.charAt(i) == 'W') {
      digitalWrite(vibratorPin, 0);
      delay(timingtable[4] * timeunit);
    }
    delay(timingtable[2] * timeunit);
  }
  delay(timingtable[3] * timeunit);   // Delay due to the char space (data is cut by char space, so we need to simulate it...)

}

void setup()
{
  // pinMode(9, OUTPUT);
  // digitalWrite(9, HIGH);
  pinMode(vibratorPin, OUTPUT);

  Serial.begin(9600);
  Serial.print("Time unit is set:");
  Serial.println(timeunit);
  // morseToPulse(".-.. --- .-. . --   .. .--. ... ..- --   -.. --- .-.. --- .-.   ... .. -   .- -- . - --..--   -.-. --- -. ... . -.-. - . - ..- . .-.   .- -.. .. .--. .. ... -.-. .. -. --.   . .-.. .. - .-.-.-   ..- -   --- -.. .. --- .-.-.-   -. .- --   ... . -..   . ... - .-.-.-   -. .- --   .-  ");
  dataParser("13141323234132413");
  // BTSerial.begin(38400);  // HC-05 default speed in AT command more
}

void loop()
{
  /*
    // Keep reading from HC-05 and send to Arduino Serial Monitor
    if (BTSerial.available() > 0) {
    char buffer = BTSerial.read();
    Serial.println();
    Serial.println(morseCode);
    morseToPulse(morseCode);
    }
  */

}
