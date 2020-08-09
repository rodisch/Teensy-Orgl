
#ifndef Distortion_h_
#define Distortion_h_

#include "Arduino.h"
#include "AudioStream.h"

class Distortion : public AudioStream
{

public:
        Distortion(void) : AudioStream(1, inputQueueArray) {
        	multiplier_in = 65536; //2^16
        	multiplier_out = 65536;
        }
        
        virtual void update(void);

        void gain(float gain) {
        	if (gain > 32767.0f) gain = 32767.0f;
        	else if (gain < -32767.0f) gain = -32767.0f;

        	multiplier_in = gain * 65536.0f;
        	multiplier_out = 1/gain * 65536.0f;
        }

private:
	int32_t multiplier_in;
	int32_t multiplier_out;
	audio_block_t *inputQueueArray[1];

};

#endif
