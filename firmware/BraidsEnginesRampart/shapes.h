#include <iostream>


/*
int main() {
    const char* const list[] = {"zip", "zam", "bam"};
    const size_t len = sizeof(list) / sizeof(list[0]);

    for (size_t i = 0; i < len; ++i)
        std::cout << list[i] << "\n";

    const std::vector<std::string> v(list, list + len);
    std::copy(v.begin(), v.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
}*/
  
  
const char* const oscnames[] = {
"CSAW",
"MORPH",
"SAW_SQUARE",
"SINE_TRIANGLE",
"BUZZ",
"SQUARE_SUB",
"SAW_SUB",
"SQUARE_SYNC",
"SAW_SYNC",
"TRIPLE_SAW",
"TRIPLE_SQUARE",
"TRIPLE_TRIANGLE",
"TRIPLE_SINE",
"TRIPLE_RING_MOD",
"SAW_SWARM",
"SAW_COMB",
"TOY",
"DF_LP",
"DF_PK",
"DF_BP",
"DF_HP",
"VOSIM",
"VOWEL",
"VOWEL_FOF",
"HARMONICS",
"FM",
"FEEDBACK_FM",
"CHAOTIC_FEEDBACK_FM",
"PLUCKED",
"BOWED",
"BLOWN",
"FLUTED",
"STRUCK_BELL",
"STRUCK_DRUM",
"KICK",
"CYMBAL",
"SNARE",
"WAVETABLES",
"WAVE_MAP",
"WAVE_LINE",
"WAVE_PARAPHONIC",
"FILTERED_NOISE",
"TWIN_PEAKS_NOISE",
"CLOCKED_NOISE",
"GRANULAR_CLOUD",
"PARTICLE_NOISE",
  };
 const size_t nameslength = sizeof(oscnames) / sizeof(oscnames[0]);

// Scale names per bank
const char* const scaleNamesBank0[] = {"Ionian", "Dorian", "Phrygian", "Lydian", "Mixolydian", "Aeolian", "Locrian"};
const char* const scaleNamesBank1[] = {"Nahawand", "Bayati", "NawaAthar", "Farafahza", "Nikriz", "Bhairav"};
const char* const scaleNamesBank2[] = {"Suznak", "Zamzam", "KijazKarKurd", "RomanianMinor", "Enigmatic", "Partch1"};
const char* const scaleNamesBank3[] = {"Pelog", "Zhi", "Yu", "MajPent", "Kumoi", "Jiao", "Prometheus", "Chinese", "Gong", "Egypt"};
const char* const scaleNamesBank4[] = {"MajorPentatonic", "RomanMinor", "NeapolitanMinor", "MelodicMinor", "Spanish", "LydianMinor"};
const char* const scaleNamesBank5[] = {"HexDorian", "HexAeolian", "HexSus", "HexPhrygian", "HexMaj6", "HexMaj7", "PhrygianFreq"};

const char* const* const scaleNames[] = {
  scaleNamesBank0,
  scaleNamesBank1,
  scaleNamesBank2,
  scaleNamesBank3,
  scaleNamesBank4,
  scaleNamesBank5
};

const uint8_t scaleNamesLengths[] = {7, 6, 6, 10, 6, 7};
const uint8_t numScaleBanks = 6;
