// DrawBar Oscillator

#ifndef _DRAWBAROSC_H
#define _DRAWBAROSC_H

#include <Audio.h>
#include "TwWavetable.h"

class DrawbarOsc : public AudioStream {

  public:
    DrawbarOsc() : AudioStream(0, NULL) {};

    void setWavetab(TwWavetable *wavt) {
      waveTab = wavt;
    }

    void FundamentalTw(uint8_t tw);
	  void Fundamental(float freq);
    void Amplitude(float v);

    void update(void);

 private:
    uint16_t tone_amp=32767;
    uint32_t tone_phase=0;
    uint32_t tone_incr=0;

    TwWavetable *waveTab; //pointer to wavetable(s)
    int16_t   *wave_data; // actually used wavetable, dependend on fundamental tonewheel (note)
  };

#endif
