#include "TwWavetable.h"

void TwWavetable::UpdateWav(void) {
    int16_t buf[DRAWBAR_WAVETABLE_LENGTH+1];

    //prepare wavedata for range 0..8
    for (int kk=0; kk<8; kk++){
    	for (int i=0; i<DRAWBAR_WAVETABLE_LENGTH+1; i++) {
    		int32_t tmpbuf=0;
    		for (int jj=0;jj<9; jj++){
          uint8_t harm = rng_mult[kk][jj];  // multiplier (harmonic)
          uint8_t pho = ph_offset[harm];    // phase-offset (assures max. peak <4.0)
          uint8_t amp = rng_amp[kk][jj];    // amplitudes (decays for high tonewheels)
          tmpbuf += AudioWaveformSine[(pho+harm*i) & 0xff] * (amp * drawbars[jj]); //add all drawbars, phase index is always integer!
    		}
    		buf[i]= tmpbuf >> 16;
    	}

    	__disable_irq();
    	memcpy(waveData[kk], buf, sizeof(buf));
    	__enable_irq();
    }
}

void TwWavetable::SetDrawbar(int n, uint8_t val) {
	uint8_t newval;
	if (val < 0) {
		val = 0;
	} else if (val > 8) {
		val = 8;
	}
	newval=drawbar_scale[val];
	if (newval!=drawbars[n]){
		drawbars[n] = newval;
		UpdateWav();
	}
}

void TwWavetable::SetRegister(uint8_t val[9]) {
	for (int n=0;n<9;n++){
		drawbars[n]= drawbar_scale[val[n]];
	}
	UpdateWav();
}
