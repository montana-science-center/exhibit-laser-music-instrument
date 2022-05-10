// #include <Arduino.h>
#include <WProgram.h>
#include <Control_Surface.h>

#define MIDI_BAUD 31250
#define LED_PIN 13
#define VS1053_RST 9

int main() {

  static HardwareSerialMIDI_Interface midi {.serial = Serial2, .baud = MIDI_BAUD};

  // NoteButtons lasers {.buttons = [18, 19]}
  static NoteButton laser1 {.pin = 18, .address = MIDI_Notes::Db(4)};
  static NoteButton laser2 {.pin = 19, .address = MIDI_Notes::Eb(4)};

  laser1.invert();
  laser2.invert();
  
  pinMode(LED_PIN, OUTPUT);
  pinMode(VS1053_RST, OUTPUT);

  digitalWrite(LED_PIN, HIGH);

  digitalWrite(VS1053_RST, LOW);
  delay(10);
  digitalWrite(VS1053_RST, HIGH);
  delay(10);

  Control_Surface.begin();

  while(1) {
    Control_Surface.loop();
  }


  // while(1) {
    // digitalWrite(ledPin, HIGH);   // set the LED on
    // delay(100);                  // wait for a second
    // digitalWrite(ledPin, LOW);    // set the LED off
    // delay(100);                  // wait for a second
  // }
}