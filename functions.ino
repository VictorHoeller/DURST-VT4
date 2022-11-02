// Funktionen:

// Tap Tempo
void sendClockPulse() {
  midiSerial.write(MIDI_TIMING_CLOCK);
  LEDblink();
}

long calculateIntervalMicroSecs(float bpm) {
  // Take care about overflows!
  return 60L * 1000000 / bpm / CLOCKS_PER_BEAT / 0.9795; //0.9864268;  // Attiny correction
}

// Start Stopp
void startOrStop() {
  if (!playing) {
    midiSerial.write(MIDI_START);
    digitalWrite(PLAY_LED, HIGH);
  } else {
    midiSerial.write(MIDI_STOP);
    digitalWrite(PLAY_LED, LOW);
  }
  playing = !playing;
}

// LED
void LEDblink() {
  blinkCount = (blinkCount + 1) % CLOCKS_PER_BEAT;
  if (blinkCount == 0) {
    digitalWrite(TEMPO_LED, HIGH);
  }
  else {
    if (blinkCount == BLINK_TIME) {
      digitalWrite(TEMPO_LED, LOW);
    }
  }
}
