//MIDI NoteOn, NoteOff

#ifndef _MidiSetup_H
#define _MidiSetup_H

#define _DEBUG

#include "AudioSetup.h"
#include "Leslie.h"
#include "TwWavetable.h"

// MIDI base channel
const uint8_t midibase=1; //Midi channel OM, LM=midibase+

// MIDI controller CC-numbers
const uint8_t cc_drawbars[] = { 41, 42, 43, 44, 45, 46, 47, 48, 28 }; //9 Drawbars
const uint8_t cc_dbmin = 28;
const uint8_t cc_dbmax = 48;

const uint8_t cc_vibchor = 21;  //vib/chorus mode: 64 off, 63..0: V1..V3, 65..127: C1..C3

const uint8_t cc_percvol = 22;  //percussion volume: 64 perc off, 63..0: vol 2nd, 65..127: vol 3rd
const uint8_t cc_percdecay = 23; //percussion decay
float percvol = 0.0;
uint8_t percnote = 0;

const uint8_t cc_lslspeed = 01; //Modulation -> Leslie speed
const uint8_t cc_lslbal  = 24;
const uint8_t cc_lsldist = 25;  // Distortion

const uint8_t cc_click  = 26; // keyclick volume

const uint8_t cc_volume = 7;  // volume of TG upper/lower manual
const uint8_t cc_expression = 11; // volume hardware via footpedal (range 0.4..0.9)

const uint8_t cc_release = 72; // tone envelope
const uint8_t cc_attack = 73;  // tone envelope

const uint8_t cc_eqlow = 75; // eq hardware
const uint8_t cc_eqhi = 76;  // eq hardware
float eqlow = 0, eqhi = 0;

//--- Note On --------------------------------------------------------------------
void OnNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
	uint8_t manualidx = channel-midibase; // 0:OM, 1:UM
	uint8_t tonewheel = note-23-(1-manualidx)*12;        //C4: midi 60 -> sub_tw 37 (fund_tw 49), OM shifted by 1 octave

	#ifdef _DEBUG
	Serial.printf("\nNoteOn:  ch %02i note %003i vel %003i -> ", channel, note, velocity);
	#endif

	if ((tonewheel > 68) || (tonewheel <1)) {
		return;
	}

	//--- Percussion
	if ((manualidx==0) && (VoiceTab[0].GetIdleVoices() == VoiceTab[0].GetMaxVoices()) && (percnote > 0)) { //check  OM + no note is playing + perc on
		float pfreq = (float)(TWfreq[tonewheel-1 + percnote]) / 10;
		float pamp =  (float)(TWamp[tonewheel-1 + percnote] + 240) / 480;
		perc.frequency(pfreq);
		perc.amplitude(pamp);
		percEnvelope.noteOn();
		#ifdef _DEBUG
		Serial.printf("perc(freq_%6.2f amp_%4.2f) + ", pfreq, pamp);
		#endif
	}

	//--- Tone
	if (VoiceTab[manualidx].GetIdleVoices() > 0) {
		uint8_t voiceNo = VoiceTab[manualidx].NoteOn(note);
		voiceNo += manualidx*8;
		osc[voiceNo].FundamentalTw(tonewheel);
		envelope[voiceNo].noteOn();

	//--- Click
		clickEnvelope[manualidx].noteOn();

		// digitalWrite(led, HIGH);
		#ifdef _DEBUG
		Serial.printf("voice_%2i idle_%2i", voiceNo, VoiceTab[manualidx].GetIdleVoices());
		#endif
	}
}

//--- Note Off --------------------------------------------------------------------

void OnNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
	uint8_t manualidx=channel-midibase;
	uint8_t voiceNo = VoiceTab[manualidx].NoteOff(note);

	if (voiceNo < 0xFF) {
		voiceNo += manualidx*8;
		envelope[voiceNo].noteOff();
	}

	if ( (manualidx==0) && (VoiceTab[0].GetIdleVoices()==8) && percEnvelope.isActive() ) // Stop percussion if last note is released
		percEnvelope.noteOff();

	// digitalWrite(led, LOW);

	#ifdef _DEBUG
	Serial.printf("\nNoteOff: ch %02i note %003i vel %003i -> ", channel, note, velocity);
	Serial.printf("voice_%2i idle_%2i", voiceNo, VoiceTab[manualidx].GetIdleVoices());
	#endif
}


//--- CC-Controller ------------------------------------------------------------------

void OnControlChange(uint8_t channel, uint8_t control, uint8_t value) {
	uint8_t manualidx; //0: OM, 1:UM
	if (channel>midibase+1)
		return;
	else
		manualidx=channel-midibase;

	#ifdef _DEBUG
	Serial.printf("\nCC: ch %02x cc %3d val %3d -> ", channel, control,
			value);
	#endif

//--- Drawbars
	if (control >= cc_dbmin && control <= cc_dbmax) {
		for (uint8_t i = 0; i < 9; i++) {
			if (control == cc_drawbars[i]) {
				uint8_t val = map(value, 0, 128, 0, 9); // map to 0..8
				WaveTab[manualidx].SetDrawbar(i, val);

				#ifdef _DEBUG
				Serial.printf("drawbar: manual_%i db_%1d val_%1d",manualidx, i, val);
				#endif
				return;
			}
		}
	}

//--- Volume Tonegenerator
	switch ((uint8_t) control) {

	case cc_volume: {
		float vol = (float) value/127;
		TgOut.gain(manualidx,powf(vol,4));  // make it logarithmic

		#ifdef _DEBUG
		Serial.printf("volume: manual_%i vol_%f",manualidx, vol);
		#endif
		break;
	}

	//--- Chorus Vibrato
	case cc_vibchor: {
		uint8_t vmi = map(value, 0, 128, 0, 6);
		VibratoMode vm = static_cast<VibratoMode>(vmi);
		ChVib[manualidx].SetMode(vm);

		#ifdef _DEBUG
		Serial.printf("vibchor: manual_%i mode_%1d",manualidx, vmi);
		#endif
		break;
	}

	//--- KeyCLick volume
	case cc_click: {
		// float vol = 0.04f*(float) value / 127;
		// click.amplitude(vol);
		float vol = (float) value / 127;
		sumDb[manualidx].gain(8,powf(vol,4));

		#ifdef _DEBUG
		Serial.printf("click: manual_%i vol_%3.2f",manualidx,vol);
		#endif
		break;
	}

	//--- Tone envelope
	case cc_attack: {
		float att = 0.5+200.0f*(float) value / 127;
		for (uint8_t i=0; i<7;++i){
			envelope[manualidx*8+i].attack(att);
		}

		#ifdef _DEBUG
		Serial.printf("attack: manual_%i att_%3.2f",manualidx, att);
		#endif
		break;
	}

	case cc_release: {
		float rel = 0.5+500.0f*(float) value / 127;
		for (uint8_t i=0; i<7;++i){
			envelope[manualidx*8+i].release(rel);
		}

		#ifdef _DEBUG
		Serial.printf("release: manual_%i rel_%3.2f",manualidx, rel);
		#endif
		break;
	}

//--- Percussion
	case cc_percvol: {
		if (manualidx) break; // upper man only;
		//percvol = (float) value / 127;
		percnote=(value<60)*12+(value>68)*19; //select percussion off, 2nd or 3rd
		percvol=(float)((value<60)*(60-value)+(value>67)*(value-67))/60;
		// TgOut.gain(2, 0.08 * percvol);
		percAmp.gain(0.08f * percvol);

		#ifdef _DEBUG
		Serial.printf("percvol: vol %3.2f note %d", percvol, percnote);
		#endif
		break;
	}
	case cc_percdecay: {
		if (manualidx) break; // upper man only;
		float pdecay = 50 + (float)value * 2;
		percEnvelope.decay(pdecay);
		#ifdef _DEBUG
		Serial.printf("percdecay: decay %f", pdecay);
		#endif
		break;
	}

	//--- Lesley
	case cc_lslbal: {
		SetLslBalance(value); //defined in AudioSetup.h
		#ifdef _DEBUG
		Serial.printf("lslbalance: val_%d", value);
		#endif

		break;
	}

	case cc_lsldist: {
		SetLslDistortion(value); //defined in AudioSetup.h
		#ifdef _DEBUG
		Serial.printf("lsldistortion: val_%d", value);
		#endif
		break;
	}

	case cc_lslspeed: {
		SetLslSpeed(value); //defined in AudioSetup.h
		#ifdef _DEBUG
		Serial.printf("lslspeed: val_%d", value);
		#endif
		break;
	}

//--- Audio Shield
	case cc_expression: {
		float vol = 0.4f + 0.4f *(float) value/127;
		audioShield.volume(vol);
		#ifdef _DEBUG
		Serial.printf("auido_volume: vol_%f", vol);
		#endif
		break;
	}

	case cc_eqlow: {
		eqlow = (float) value/64 - 1 ;
		audioShield.eqBands(eqlow, eqhi);
		#ifdef _DEBUG
		Serial.printf("audio_eq: eqlow %f", eqlow);
		#endif
		break;
	}

	case cc_eqhi: {
		eqhi = (float) value / 64 - 1;
		audioShield.eqBands(eqlow, eqhi);
		#ifdef _DEBUG
		Serial.printf("audio_eq: eqhi %f", eqhi);
		#endif
		break;
	}

	} //switch
}

#endif
