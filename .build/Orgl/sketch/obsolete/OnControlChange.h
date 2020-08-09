#pragma once
// All MIDI-Controlers

#include "AudioSetup.h"

const uint8_t midibase=1;

// MIDI controller
const int cc_drawbars[] = { 41, 42, 43, 44, 45, 46, 47, 48, 28 }; //9 Drawbars
const int cc_dbmin = 28;
const int cc_dbmax = 48;

const int cc_vibchor = 21;  //vib/chorus mode
const int cc_percvol = 22;  //percussion volume
const int cc_percdecay = 23; //percussion decay
float percvol = 0.0;
int percnote = 0;

// const int cc_dist = 25;
// float dstgain  = 1.0;
const int cc_Lslspeed = 01; //Modulation -> Leslie speed

const int cc_eqlow = 26;
const int cc_eqhi = 27;
float eqlow = 0, eqhi = 0;

#define _DEBUG

void OnControlChange(byte channel, byte control, byte value) {
#ifdef _DEBUG
	Serial.printf("\nControlChange: ch %02x cc %3d val %3d ", channel, control,
			value);
#endif
	byte manualidx; //0: OM, 1:UM
	if (channel>midibase+1)
		return;
	else
		manualidx=channel-midibase;

//----- Drawbars
	if (control >= cc_dbmin && control <= cc_dbmax) {
		for (int i = 0; i < 9; i++) {
			if (control == cc_drawbars[i]) {
				int8_t val = map(value, 0, 128, 0, 9); // map to 0..8
				WaveTab[manualidx].SetDrawbar(i, val);

#ifdef _DEBUG
				Serial.printf("\ncc_drawbar: db %1d val %1d", i, val);
#endif
//				break;
				return;
			}
		}
	}

//--- other cc controlers
	switch ((int) control) {

//--- Chorus Vibrato
	case cc_vibchor: {
		int vmi = map(value, 0, 128, 0, 6);
		VibratoMode vm = static_cast<VibratoMode>(vmi);
		vibrato.SetMode(vm);

#ifdef _DEBUG
		Serial.printf("\ncc_vibchor: mode %1d", vmi);
#endif
		break;
	}

//--- Percussion
	case cc_percvol: {
		//percvol = (float) value / 127;
		percvol=(float)((value<60)*(60-value)+(value>67)*(value-67))/60;
		mixer_TG.gain(mxinPerc, 0.08 * percvol);
		percnote=(value<60)*12+(value>68)*19;

#ifdef _DEBUG
		Serial.printf("\ncc_percvol: vol %3.2f note %d", percvol, percnote);
#endif
		break;
	}
	case cc_percdecay: {
		int pdecay = 50 + value * 2;
		percEnvelope.decay(pdecay);
#ifdef _DEBUG
		Serial.printf("\ncc_percdecay: decay %d", pdecay);
#endif
		break;
	}

/*
//--- Distortion
	case cc_dist: {
		dstgain = (float) value/16 + 1;
		dist.gain(dstgain);
#ifdef _DEBUG
		Serial.printf("\ncc_distortion: dstgain %f", dstgain);
#endif
		break;
	}
*/
//--- Distortion
	case cc_Lslspeed: {
		float speed = (float) value/16;  //speed=0..8
		modLFO_L.frequency(speed*1.03);
		modLFO_R.frequency(speed*1.03);
		modLFO_Bs.frequency(speed);
#ifdef _DEBUG
		Serial.printf("\ncc_Lslspeed: speed %f", speed);
#endif
		break;
	}


	case 92: {
		float ff = (float) value/128*6000 ;
		filterL.frequency(ff);
		filterR.frequency(ff);

		break;
	}



//--- Audio EQ
	case cc_eqlow: {
		eqlow = (float) value/64 - 1 ;
		audioShield.eqBands(eqlow, eqhi);
#ifdef _DEBUG
		Serial.printf("\ncc_eqlow: eqlow %f", eqlow);
#endif
		break;
	}

	case cc_eqhi: {
		eqhi = (float) value / 64 - 1;
		audioShield.eqBands(eqlow, eqhi);
#ifdef _DEBUG
		Serial.printf("\ncc_eqhi: eqhi %f", eqhi);
#endif
		break;
	}
	} //switch
}
