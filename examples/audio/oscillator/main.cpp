#include <cmath>

#include <libluna/Application.hpp>
#include <libluna/Color.hpp>

#ifdef __SWITCH__
#define CANVAS_WIDTH 1920
#define CANVAS_HEIGHT 1080
#else
#define CANVAS_WIDTH 800
#define CANVAS_HEIGHT 600
#endif

using namespace std;
using namespace Luna;
using namespace Luna::Audio;

constexpr float kA4Freq = 440.0f;

namespace {
  float noteToFrequency(const char *str) {
    char note = str[0];
    char modifier = str[1];
    int octave = str[2] - '0';

    // Mapping from notes to their positions in an octave
    int note_positions[] = {'c', 'd', 'e', 'f', 'g', 'a', 'b'};
    int base_semitone = 0;

    switch(note) {
        case 'c': base_semitone = 0; break;
        case 'd': base_semitone = 2; break;
        case 'e': base_semitone = 4; break;
        case 'f': base_semitone = 5; break;
        case 'g': base_semitone = 7; break;
        case 'a': base_semitone = 9; break;
        case 'b': base_semitone = 11; break;
    }

    if (modifier == '#') {
      base_semitone += 1;
    } else if (modifier == 'b') {
      base_semitone -= 1;
    }

    int semitone_distance = (octave - 4) * 12 + base_semitone;
    return kA4Freq * pow(2.0, semitone_distance / 12.0);
  }

  const char *melody[] = {
    "c#4", "d#4", "e-4", "ab3", "d#4", "gb3", "e-3", "d#3", "c#3", "ab3",
    "c#4", "e-4", "d#4", "e-4", "gb4", "ab3", "b-3", "e-4", "d#4", "c#4",
    "ab3", "c#4", "f-4", "c#4", "gb4", "c#4", "ab4", "c#4", "a-4", "c#4",
    "d#4", "b-3", "ab3", "b-3", "d#4", "b-3", "e-4", "b-3", "gb4", "b-3",
    "ab4", "b-3", "c#4", "a-3", "ab3", "a-3", "c#4", "e-4", "b-3", "ab3",
    "gb3", "ab3", "b-3", "e-4", "gb3", "ab3", "a-3", "e-3", "d#3", "e-3",
    "gb3", "d#3", "b-2", "a-2", "ab2", "e-3", "a-2", "e-3", "ab2"
  };

  const int melodyLength = sizeof(melody) / sizeof(melody[0]);
}

static void setInstrument(shared_ptr<OscillatorNode> node, int instrument) {
  switch (instrument) {
    default:
      node->setType(OscillatorNode::kSine);
      break;
    case 1:
      node->setType(OscillatorNode::kSquare);
      node->setDuty(0.5f);
      break;
    case 2:
      node->setType(OscillatorNode::kSquare);
      node->setDuty(0.25f);
      break;
    case 3:
      node->setType(OscillatorNode::kSawtooth);
      break;
    case 4:
      node->setType(OscillatorNode::kTriangle);
      break;
  }
}

int main(int argc, char **argv) {
  Application app(argc, argv);

  shared_ptr<Canvas> canvas;
  shared_ptr<OscillatorNode> node1;
  shared_ptr<OscillatorNode> node2;
  int currentNote = 0;
  double t = 0.0;
  double elapsedAudioTime;

  int instrument = 0;

  app.whenReady([&]() {
    canvas = app.makeCanvas({CANVAS_WIDTH, CANVAS_HEIGHT});
    canvas->setVideoDriver(app.getDefaultVideoDriver());
    canvas->setBackgroundColor({0.0f, 0.9f, 0.6f, 1.0f});

    auto gain1 = app.getAudioManager()->createGain(0.5f);
    gain1->connect(app.getAudioDestinationNode());

    node1 = app.getAudioManager()->createOscillator(440, OscillatorNode::kSine);
    node1->connect(gain1);

    auto gain2 = app.getAudioManager()->createGain(0.2f);
    gain2->connect(app.getAudioDestinationNode());

    auto delay = app.getAudioManager()->createDelay(0.1f);
    delay->connect(gain2);

    node2 = app.getAudioManager()->createOscillator(440, OscillatorNode::kSine);
    node2->connect(delay);

    setInstrument(node1, instrument);
    setInstrument(node2, instrument);

    elapsedAudioTime = app.getAudioManager()->getTime();
  });

  app.addInterval(60, [&](float elapsedTime) {
    node1->setFrequency(noteToFrequency(melody[currentNote]));
    node2->setFrequency(noteToFrequency(melody[currentNote]));

    double audioDelta = app.getAudioManager()->getTime() - elapsedAudioTime;
    elapsedAudioTime = app.getAudioManager()->getTime();

    t += audioDelta;

    if (t >= 0.25) {
      t -= 0.25;
      ++currentNote;

      if (currentNote >= melodyLength) {
        currentNote -= melodyLength;

        instrument = (instrument + 1) % 5;
        setInstrument(node1, instrument);
        setInstrument(node2, instrument);
      }
    }
  });

  return app.run();
}
