// #include <Arduino.h>
#include <WProgram.h>
#include <Control_Surface.h>

#define MIDI_BAUD 31250
#define LED_PIN 13
#define VS1053_RST 9

int main() {

  static HardwareSerialMIDI_Interface midi {.serial = Serial7, .baud = MIDI_BAUD};

  static NoteButton laser1 {.pin = 34, .address = MIDI_Notes::Db(3)};
  static NoteButton laser2 {.pin = 35, .address = MIDI_Notes::Eb(3)};
  static NoteButton laser3 {.pin = 36, .address = MIDI_Notes::Gb(3)};
  static NoteButton laser4 {.pin = 14, .address = MIDI_Notes::Ab(3)};
  static NoteButton laser5 {.pin = 15, .address = MIDI_Notes::Bb(3)};
  static NoteButton laser6 {.pin = 16, .address = MIDI_Notes::Db(4)};

  laser1.invert();
  laser2.invert();
  laser3.invert();
  laser4.invert();
  laser5.invert();
  laser6.invert();
  
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