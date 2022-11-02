// written by Victor Höller 2022
// V2.1
// fits for Board: DURST_VT4_V0.3
// use TimerOne.h from this side: https://github.com/PaulStoffregen/TimerOne
// replace the TimerOne folder C:\Users\Victor Höller\Documents\Arduino\libraries

//NEU: Zeile 66, 104

#include <TimerOne.h>
#include <EEPROM.h>
#include <Bounce2.h>
#include <SendOnlySoftwareSerial.h>
SendOnlySoftwareSerial midiSerial(3);

#define START_IN 0
#define TAP_IN 1
#define TEMPO_LED 2
#define PLAY_LED 4

#define BLINK_TIME 5

#define MIDI_START 0xFA
#define MIDI_STOP 0xFC

#define MIDI_TIMING_CLOCK 0xF8
#define CLOCKS_PER_BEAT 24

// BPM Variablen
Bounce A = Bounce();
Bounce B = Bounce();
float bpm = EEPROM.read(0);
float bpm_dec = EEPROM.read(1);
byte anzahl_press = 0;
long last_tap;
long now;
bool saved = true;

float bpm4;
float bpm3;
float bpm2;
float bpm1;
float bpm0;

// Algemeine Variablen
volatile int blinkCount = 0;

// Midi Variablen
long intervalMicroSeconds;
boolean playing = false;

void setup() {
  midiSerial.begin(31250);
  pinMode(TEMPO_LED, OUTPUT);
  pinMode(PLAY_LED, OUTPUT);

  // Attach the interrupt to send the MIDI clock and start the timer
  Timer1.initialize(intervalMicroSeconds);
  Timer1.setPeriod(calculateIntervalMicroSecs(bpm));
  Timer1.attachInterrupt(sendClockPulse);

  A.attach(TAP_IN, INPUT);
  A.interval(25);
  B.attach(START_IN, INPUT);
  B.interval(25);

  // bpm = bpm + bpm_dec/100;
}

void loop() {
  // Prozedur, wenn Tap gedrückt und debouncen des Tasters
  A.update();
  now = millis();  // Aktuelle Zeit in Millisekunden
  if (A.rose()) {
    anzahl_press++;               // erhöht den Wert der Tasthäufigkeit um 1
    if (anzahl_press > 1) {
      bpm4 = 60000 / float(now - last_tap);
      if (anzahl_press == 2) {
        bpm0 = bpm4;
        bpm1 = bpm4;
        bpm2 = bpm4;
        bpm3 = bpm4;
      }
      else {
        bpm0 = bpm1;
        bpm1 = bpm2;
        bpm2 = bpm3;
        bpm3 = bpm4;
      }
      bpm = (bpm4 + bpm3 + bpm2 + bpm1 + bpm0) / 5;
      Timer1.setPeriod(calculateIntervalMicroSecs(bpm));
      saved = false;
    }
    last_tap = now;
  }

  // Schaut, ob eine NEUE Sequenz begonnen werden soll
  if (now - last_tap > 2000) {  // nach 2 Sekunden gehört ein Tap nicht mehr zu der alten Sequenz und eine neue wird begonnen
    anzahl_press = 0;           // Anzahl Pressungen wird auf Null gesetzt.
    if (saved == false) {
      if (bpm > 255) {
        bpm = 255;
      }
      EEPROM.write(0, byte(bpm));
      //EEPROM.write(1, byte((bpm - byte(bpm) * 100)));
      saved = true;
    }
  }

  B.update();
  // Schaut, ob Startknopf gedrückt wurde
  if (B.fell()) {
    startOrStop();
  }
}
