// midier scale/mode generation

const midier::Degree scaleDegrees[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21 };

const midier::Note roots[] = {
  midier::Note::C, midier::Note::D,  midier::Note::E, midier::Note::F, midier::Note::G, midier::Note::A, midier::Note::B
};

midier::Mode mode;
int scaleRoot = 0; // start at c, yawn.

int modeIndex = 0;
int currentMode[21];

void makeScale(midier::Note root, midier::Mode mode) {

  // the root note of the scale
  const midier::Note scaleRoot = root;

  //const midier::Mode mode = midier::Mode::Ionian;
  for (midier::Degree scaleDegree : scaleDegrees)
  {
    // find out the interval to be added to the root note for this chord degree and chord quality
    const midier::Interval interval = midier::scale::interval(mode, scaleDegree);

    // calculate the note of this chord degree
    const midier::Note note = scaleRoot + interval;
    currentMode[ scaleDegree - 1 ] = midier::midi::number(note, 2); // 3 is the octave


  }
}

int midiChannel = 1;  // Define which MIDI channel to transmit on (1 to 16).


// need to send note offs :)
void allOff() {
  for (int i = 18; i < 90; i++) {
    if ( ! debug) MIDI.sendNoteOff(i, 0, midiChannel);
  }
}

void HandleNoteOff(byte channel, byte note, byte velocity) {
  //envelope.noteOff();

  //digitalWrite(LED, LOW);
}

void HandleNoteOn(byte channel, byte note, byte velocity) {
  if (velocity == 0) {
    HandleNoteOff(channel, note, velocity);
    return;
  }
  //carrier_freq = mtof(note);
  //envelope.noteOn();
  //digitalWrite(LED_BUILTIN, HIGH);
}

void aNoteOff( float note, int velocity) {
  if (! debug) MIDI.sendNoteOff(note, 0, midiChannel);
  trigger_in = 0.0;
  env->gate(false);
  //digitalWrite(LED_BUILTIN, LOW);
}

void aNoteOn(float note, int velocity) {

  if (! debug) MIDI.sendNoteOff(lastNote, 0, midiChannel);
  if (! debug) MIDI.sendNoteOn(note, velocity, midiChannel);
   trigger_in = 1.0;
  // env->gate(true);
  //digitalWrite(LED, HIGH);
}

/*
  Converts a note frequency to the closest corresponding MIDI
  note number.
  Adapted from https://gist.github.com/francoisgeorgy/d155f4aa5e8bd767504c2b43e1ba2902
*/
static const uint32_t midi_note_at_zero_volts = 12;
static const float semitones_per_octave = 12.0f;
static const float volts_per_semitone = 1.0f / semitones_per_octave;
static const float a4_frequency = 440.0f;
static const uint32_t a4_midi_note = 69;
uint32_t frequency_to_midi_note(float frequency) {
  float note = 69.0f + logf(frequency / a4_frequency) / logf(2.0f) * semitones_per_octave;
  return ceil(note);
}
