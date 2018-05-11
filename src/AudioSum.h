
//#ifndef _AudioSum8_h
//#define _AudioSum8_h
#pragma once

#include "Arduino.h"
#include "AudioStream.h"

class AudioMixer9 : public AudioStream {

public:
    AudioMixer9(void) : AudioStream(9, inputQueueArray) {
    	for (int i=0; i<9; i++) multiplier[i] = 65536;
    }

    virtual void update(void);
    //
    // void gain(float gain) {
    //   for (int i=0; i<9; i++) multiplier[i] = 65536;
    // }

    void gain(unsigned int channel, float gain) {
      if (channel >= 9) return;
      if (gain > 32767.0f) gain = 32767.0f;
      else if (gain < -32767.0f) gain = -32767.0f;
      multiplier[channel] = gain * 65536.0f; // TODO: proper roundoff?
    }

private:
  	int32_t multiplier[9];
  	audio_block_t *inputQueueArray[9];
};

class AudioSum2 : public AudioStream {

public:
    AudioSum2(void) : AudioStream(2, inputQueueArray) {
    	multiplier = 65536; // = MULTI_UNITYGAIN
    }

    virtual void update(void);

    void gain(float gain) {
    	if (gain > 32767.0f) gain = 32767.0f;
    	else if (gain < -32767.0f) gain = -32767.0f;
    	multiplier = gain * 65536.0f; // TODO: proper roundoff?
    }

private:
  	int32_t multiplier;
  	audio_block_t *inputQueueArray[2];

};


//#endif
