#ifndef _LESLIE_H
#define _LESLIE_H

#include "Arduino.h"
#include <Audio.h>
#include "AudioSum.h"

// Input (LslInput,0) Input Amplifier
// Output (LslOutL,0) (LslOutR,0)  Output Mixers

// modulation src
AudioSynthWaveformDc     modOffsLR, modOffsBs;         //mod Offset
AudioSynthWaveformSine   modLFO_L,modLFO_R,modLFO_Bs ; // modulation LFOs
AudioSum2                sum2_L, sum2_R, sum2_Bs;      //sum modulation Offset+LFO

// modulation env & filters
AudioEffectMultiply      modEnvL, modEnvR, modEnvBs; // modulation amplitude
AudioFilterStateVariable filterL, filterR; //modulation filter

AudioAmplifier  LslInput;          // input node
AudioEffectWaveshaper LslWavesh;        // not really

AudioFilterStateVariable filterDist,filterX;  // crossover Filter - Input
// AudioEffectDelay delayBs, delayTg; // just for a littel phase spread
AudioEffectDelay delayBs;
AudioMixer4 LslOutL, LslOutR;      // output mixer

AudioAnalyzePeak LslInputPk; // Input Monitor

AudioConnection patchLsl[] = {
  {LslInput,LslInputPk}, //monitor input
	{LslInput,LslWavesh},
  {LslWavesh,filterDist},

	{filterDist, 0,filterX,0},
  {filterDist, 0, LslOutL, 2},
  {filterDist, 0, LslOutR, 2},

	{filterX,2, modEnvL,0},  // Audio: filter HP (>800hz) -> Env
	{modEnvL, 0, filterL, 0}, // Env -> modulation Filter L
	{filterL, 1, LslOutL, 0}, // Filter -> Output L
	{modLFO_L, 0, sum2_L, 0}, // mod source Offs+LFO
  {modOffsLR, 0, sum2_L, 1},
	{sum2_L, 0, modEnvL, 1}, // mod amplitude
	{sum2_L, 0, filterL, 1}, // mod filter freq

	{filterX,2,modEnvR,0},   // Audio: filter HP (>800hz) -> Env
	{modEnvR, 0, filterR, 0}, // Env -> modulation Filter
	{filterR, 1, LslOutR, 0}, // Filter -> Ouput R
	{modLFO_R, 0, sum2_R, 0}, //modulation Offs+LFO
  {modOffsLR, 0, sum2_R, 1},
	{sum2_R, 0, modEnvR, 1},  // mod amplitude
	{sum2_R, 0, filterR, 1},  // mod filter freq

	{filterX,0, modEnvBs,0},  // Audio: filter LP (<800hz) -> Env
	{modEnvBs, delayBs},      // Env -> Delay
	{delayBs, 0, LslOutL, 1},	  // Delay -> Output L
  {delayBs, 1, LslOutR, 1},	  // Delay -> Output R
	{modLFO_Bs, 0, sum2_Bs, 0},{modOffsBs, 0, sum2_Bs, 1}, // modulation source Offs+LFO
	{sum2_Bs, 0, modEnvBs, 1} //mod amplitude
};

void SetLslDistortion(uint8_t dist){
	float shdata[257];
	// float c=(float)dist/140; // dist=0..127 -> c=0...0.9
  float c=(float)dist/135; // dist=0..127 -> c=0...0.98
  float k=2*c/(1-c);

	for (int ii=0; ii<257; ii++){
		 float x=(float)(ii-128)/129;
		 // shdata[ii]=(1+c)*x-c/2*x*x*x;   //polynom (1+c)*x - c/2*x^3 - ...)
     // if (x>0)
     //  shdata[ii] +=-c/2*x*x;         // c/2*x*abs(x) (some 2nd harmonics)
     // else
     //  shdata[ii] += c/2*x*x;
     if (x>0)
      shdata[ii] = (1+k)*x/(1+k*x);
     else
      shdata[ii] = (1+k)*x/(1-k*x);
	}
	LslWavesh.shape(shdata,257);

	#ifdef _DEBUG
	Serial.printf("\nTube: dst_%i c_%3.2f", dist, c);
	Serial.printf("\n %3.2f  %3.2f  %3.2f  %3.2f  %3.2f", shdata[32],shdata[64],shdata[128],shdata[196],shdata[222]);
	#endif
}

void SetLslBalance(uint8_t bal){
	float wet = (float)bal/127;
	LslOutL.gain(0,0.4*wet);  // HighL
	LslOutL.gain(1,0.4*wet);  // Bs
	LslOutL.gain(2,0.2*(1-wet));   // TG dry

	LslOutR.gain(0,0.4*wet);  // HighR
	LslOutR.gain(1,0.4*wet);  // Bs
	LslOutR.gain(2,0.2*(1-wet));   // TG dry
}

void SetLslSpeed(uint8_t spd){
	float speed = (float) spd/16;  //speed=0..8
	modLFO_L.frequency(speed+0.5);
	modLFO_R.frequency(speed+0.5);
	modLFO_Bs.frequency(speed);
}

void LeslieInit(){
    LslInput.gain(2.);
    SetLslDistortion(32);  //almost clean
    SetLslBalance(100);
    SetLslSpeed(16);

    // // TG direct path
    // delayTg.delay(0,0.5);  //delay Bs to left
    // delayTg.delay(1,2.2);  //delay Bs to right

    // Leslie X-filter
    filterDist.frequency(7000); //Lowpassfilter
    filterDist.resonance(0.7);
    filterX.frequency(800); //crossover frequency
    filterX.resonance(0.7);

    //---Hi LFO - AM and filter modulation
    modOffsLR.amplitude(0.7);  //DC Offs
    modLFO_L.amplitude(0.4);  //modulation depth
    modLFO_R.amplitude(0.4);
    modLFO_R.phase(0.8*90);   // phase diff L - R (1 = 90°)
    sum2_L.gain(1); // add LFO+Offset DC
    sum2_R.gain(1);

    filterL.frequency(2000);
    filterL.resonance(0.8);
    filterL.octaveControl(0.5); // modulation depth filter

    filterR.frequency(2000);
    filterR.resonance(0.8);
    filterR.octaveControl(0.5);

    //--- Bass LFO - AM modulation
    modOffsBs.amplitude(0.7);
    modLFO_Bs.amplitude(0.15);
    sum2_Bs.gain(1); // add LFO+Offset DC
    delayBs.delay(0,0.5);  //delay Bs to left
    delayBs.delay(1,1.5);  //delay Bs to right
}

#endif
