// #include <Arduino.h>
#include <WProgram.h>
#include <Control_Surface.h>
// #include "midiProgramNames.h"

#define MIDI_BAUD 31250
#define NUM_NOTES 6

#include "midiProgramNames.h"

class SelectorDisplayCallback
{
private:
    /* data */
public:
    SelectorDisplayCallback()
    {

    };
    ~SelectorDisplayCallback() {} ;

    // Begin function is called once by Control Surface.
    // Use it to initialize everything.
    void begin() 
    {
        Serial.begin(9600);
    }
 
    // Update function is called continuously by Control Surface.
    // Use it to implement things like fading, blinking ...
    void update() {
      // Serial.println("in the update!");
    }
 
    // Update function with arguments is called when the setting
    // changes.
    // Use it to update the LEDs.
    void update(setting_t oldSetting, setting_t newSetting) 
    {
        // Program numbers start 1, but indexes start at 0, so we subtract 1
        Serial.println(PROGRAM_NAMES[newSetting]);
    }
};

void lightElWires(Bankable::NoteButton lasers[], const uint8_t EL_WIRE_PINS[]) {

  for (size_t i = 0; i < NUM_NOTES; i++)
  {
    auto state = lasers[i].getButtonState();
    Serial.println(state);

    // The laser NoteButtons are inverted, so our logic for turning the EL wires on has to be inverted as well. A falling edge is "on", and a rising edge is "off".
    if (state == Button::Falling) {
      digitalWrite(EL_WIRE_PINS[i], HIGH);
    }
    else if (state == Button::Rising) {
      digitalWrite(EL_WIRE_PINS[i], LOW);
    }
  }
}

int main() {

  static HardwareSerialMIDI_Interface midiSynth {.serial = Serial7, .baud = MIDI_BAUD};

  const uint8_t EL_WIRE_PINS[NUM_NOTES] = {2, 3, 4, 5, 6, 7};

  static BidirectionalMIDI_Pipe loopbackPipe;
  static MIDI_Pipe synthTxPipe;

  /* Set up a loopback (MIDI out -> MIDI in) for the "control surface" 
    (the teensy) so we can read the MIDI messages we send. This allows us to
    light up EL wires when a note is sent, for example */
  Control_Surface | loopbackPipe | Control_Surface;

  // Connect MIDI out from the teensy to MIDI in of the synth module
  Control_Surface >> synthTxPipe >> midiSynth;

  static ProgramChanger<128> programChanger {
    {  // list of midi program numbers
      0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127
    },
    CHANNEL_1, // MIDI channel to use
  };

  static GenericEncoderSelector<128, SelectorDisplayCallback> programSelector {
    programChanger,
    {},
    {32, 31},
    1,
    Wrap::Wrap,
  };

  static Transposer<-12, +12> transposer;

  static EncoderSelector<transposer.getNumberOfBanks()> pitchSelector {
    transposer,
    {25, 27},
    1,
    Wrap::Clamp
  };
  
  static Bankable::NoteButton lasers[NUM_NOTES] {
    {.bank = transposer, .pin = 34, .address = MIDI_Notes::Db(3)},
    {.bank = transposer, .pin = 35, .address = MIDI_Notes::Eb(3)},
    {.bank = transposer, .pin = 36, .address = MIDI_Notes::Gb(3)},
    {.bank = transposer, .pin = 14, .address = MIDI_Notes::Ab(3)},
    {.bank = transposer, .pin = 15, .address = MIDI_Notes::Bb(3)},
    {.bank = transposer, .pin = 16, .address = MIDI_Notes::Db(4)},
  };
  
  for (auto &&laser : lasers)
  {
    laser.invert();
  }


  // TODO: for some reason, bankable NoteLEDs were not working right. I could only get one el wire to work. Non-bankable NoteLEDs worked just fine. For now, I'm turning the el wires on by checking button state instead of listening for midi notes
  // static Bankable::NoteLED<transposer.getNumberOfBanks()> elWire1 {
  //   .bank = transposer, .pin = 2, .address = MIDI_Notes::Db(3),
  // };
  // static Bankable::NoteLED<transposer.getNumberOfBanks()> elWire2 {
  //   .bank = transposer, .pin = 3, .address = MIDI_Notes::Eb(3),
  // };
  // static Bankable::NoteLED<transposer.getNumberOfBanks()> elWire3 {
  //   .bank = transposer, .pin = 4, .address = MIDI_Notes::Gb(3),
  // };
  
  
  for (auto &&elWirePin : EL_WIRE_PINS)
  {
    pinMode(elWirePin, OUTPUT);
  }

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // Serial.begin(9600);


  Control_Surface.begin();

  // programChanger.select(0);

  while(1) {
    Control_Surface.loop();

    lightElWires(lasers, EL_WIRE_PINS);

  }
}

