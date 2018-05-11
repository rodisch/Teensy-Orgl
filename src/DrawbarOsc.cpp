// Copyright (c) 2015 Peter Teichman

#include "DrawbarOsc.h"

//#define _DEBUG

void DrawbarOsc::Fundamental(float freq) {
  if (freq < 0.0) {
      freq = 0.0;
  } else if (freq > AUDIO_SAMPLE_RATE_EXACT / 2) {
      freq = AUDIO_SAMPLE_RATE_EXACT / 2;
  };

  tone_incr = freq/2 * (4294967296.0 / AUDIO_SAMPLE_RATE_EXACT); //2^32
  wave_data = waveTab->waveData[1]; //default wavetable
}

void DrawbarOsc::Amplitude(float v) {
  if (v < 0.0) {
      v = 0.0;
  } else if (v > 1.0) {
      v = 1.0;
  }

  if ((tone_amp == 0) && v) {
  		tone_phase = 0;
  }
  tone_amp = (uint16_t)(32767.0*v);
}

void DrawbarOsc::FundamentalTw(uint8_t tw){
if ((tw < 1) || (tw>68)) return; // tw: fund tonewheel (8")
	float freq = (double) (TWfreq[tw-1])/10; // freq of 8"
	tone_incr = freq/2 * (4294967296.0 / AUDIO_SAMPLE_RATE_EXACT); //wavetable is always 2 periods of fundamental (=1period of sub)

	int range_idx; //key range -> wavetable no.
	for (range_idx=0;range_idx<7;range_idx++){
		if (tw<rng_key[range_idx+1]) {
			break;
   	}
	}
  __disable_irq();
	wave_data = waveTab->waveData[range_idx];
  __enable_irq();

  #ifdef _DEBUG
	Serial.printf("\nDrawbarOsc: Tonewheel %d range_idx %d fund_freq %4.f amp %d ",tw,range_idx,freq,tone_amp);
  #endif
}

void DrawbarOsc::update(void) {
    audio_block_t *block;
    uint32_t i, ph, inc, index, scale;
    int32_t val1, val2;

    if (tone_amp == 0) {
    	tone_phase += tone_phase * AUDIO_BLOCK_SAMPLES; //update phase
      return;
    }

    block = allocate();
    if (block) {
        ph = tone_phase;
        inc = tone_incr;

        for (i=0; i<AUDIO_BLOCK_SAMPLES; i++) {
        	index = ph >> 24;
        	val1 = wave_data[index];
        	val2 = wave_data[index+1];
        	scale = (ph >> 8) & 0xFFFF;
        	val2 *= scale;
        	val1 *= 0x10000 - scale;
          #if defined(KINETISK)
        	block->data[i] = multiply_32x32_rshift32(val1 + val2, tone_amp);
          #elif defined(KINETISL)
        	block->data[i] = (((val1 + val2) >> 16) * tone_amp) >> 16;
          #endif

        	ph += inc;

        }

        tone_phase = ph;

        transmit(block, 0);
        release(block);
    }
}
