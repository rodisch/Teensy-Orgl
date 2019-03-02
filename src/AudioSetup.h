
#ifndef _AudioSetup_H
#define _AudioSetup_H

// #define _DEBUG

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
//#include <SD.h>
//#include <SerialFlash.h>

#include "DrawbarOsc.h"
#include "TwWavetable.h"
#include "VoiceTable.h"
#include "AudioSum.h"
#include "Vibrato.h"

// #include "Leslie.h"
// const int led = 13; // OnBoard LED Teensy

// const int TOTAL_VOICES = 16;
#define TOTAL_VOICES 16
#define MANUAL_VOICES 8

// Audio Hardware
AudioControlSGTL5000     audioShield;
AudioOutputI2S           i2s_out;
// AudioOutputUSB					 usb_out;

// Input --
// Output (TgOut,0)
DrawbarOsc              osc[TOTAL_VOICES];
AudioEffectEnvelope     envelope[TOTAL_VOICES];
AudioConnection* 				patchOsc[TOTAL_VOICES];
AudioConnection* 				patchEnv[TOTAL_VOICES];
AudioMixer9 				 		sumDb[2]; // sum osc + click

AudioSynthNoisePink     click;
AudioEffectEnvelope     clickEnvelope[2];
Vibrato          				ChVib[2];

AudioSynthWaveformSine   perc;
AudioEffectEnvelope      percEnvelope;
AudioAmplifier           percAmp;


AudioMixer4      TgOut;  // --> Output Tonegenerator(TgOut,0)

// patches Osc->Env-> Sum8_xx in setup function AudioInit()
AudioConnection patchTg[] = {
	{click, clickEnvelope[0]},
	{clickEnvelope[0], 0, sumDb[0], 8},
	{sumDb[0], 0, ChVib[0], 0},
	{ChVib[0], 0, TgOut, 0},

	{click, clickEnvelope[1]},
	{clickEnvelope[1], 0, sumDb[1], 8},
	{sumDb[1], 0, ChVib[1], 0},
	{ChVib[1], 0, TgOut, 1},

	{perc, percEnvelope},
	{percEnvelope, 0, percAmp, 0},
	{percAmp, 0, TgOut, 2},
};

//--- Some additional buffers
TwWavetable WaveTab[2]; //Wavetable OM, UM
VoiceTable VoiceTab[2](MANUAL_VOICES); //Tables for note -> voice conversion

// uint8_t OM_initRegister[9]={6,8,8,6,0,0,0,0,0};
uint8_t OM_initRegister[9]={6,5,4,6,0,0,0,0,5};
// uint8_t OM_initRegister[9]={8,8,8,8,8,8,8,8,8};
uint8_t UM_initRegister[9]={6,8,8,6,0,0,0,0,0};

void AudioInit(){
	AudioMemory(35);

	//--- Tonegenerators
	WaveTab[0].SetRegister(OM_initRegister);
	WaveTab[1].SetRegister(UM_initRegister);

	//--- for each osc+envelope
	for (int i=0; i<TOTAL_VOICES; ++i){
		patchOsc[i]=new AudioConnection(osc[i],envelope[i]);
		envelope[i].attack(0.7);
		envelope[i].decay(0.0);
		envelope[i].sustain(1.0);
		envelope[i].release(0.7);

		if (i<MANUAL_VOICES) { // UpperM
			osc[i].setWavetab(&WaveTab[0]);
			patchEnv[i]=new AudioConnection(envelope[i],0,sumDb[0],i);
			sumDb[0].gain(i,0.5);
		} else { // LowerM
			osc[i].setWavetab(&WaveTab[1]);
			patchEnv[i]=new AudioConnection(envelope[i],0,sumDb[1],i-MANUAL_VOICES);
			sumDb[1].gain(i-MANUAL_VOICES,0.5);
		}
	}

  //--- KeyCklick for each manual
	click.amplitude(0.2);
	for (int i=0; i<2; ++i){
		clickEnvelope[i].attack(0.3);
		clickEnvelope[i].sustain(0.0);
		clickEnvelope[i].decay(7.0);
		sumDb[i].gain(8,0.1); // KeyClick
		ChVib[i].SetMode(Off);
	}

	//--- Percussion
	percEnvelope.attack(0.3);
	percEnvelope.decay(150.0);
	percEnvelope.sustain(0.0);
	percEnvelope.release(0.1);
	percAmp.gain(0.);

	//--- Setup Mixers
  TgOut.gain(0, 1.0);  // Upper Man
	TgOut.gain(1, 1.0);  // Lower Man
	TgOut.gain(2, 1.0);   // PercussionVol is set in OnControlChange
	TgOut.gain(3, 0.);   // unused

	// Audio hardware
	audioShield.enable();
	audioShield.volume(0.8);
	audioShield.audioPostProcessorEnable(); // allow EQ
	audioShield.eqSelect(TONE_CONTROLS);    // Low/Hi shelf EQ: CC 75, CC76
}

#endif
