//MIDI NoteOn, NoteOff

#pragma once

#define _DEBUG

#include "AudioSetup.h"
#include "TwWavetable.h"
#include "OnControlChange.h"

//#include "VoiceTable.h"
//VoiceTable VoiceTab[2];  //defined in AUdioSetup.h

//const uint8_t midibase=1;

float note2freq(byte note) {
	float tmp = (note - 69) / 12.0;
	return 440.0 * pow(2.0, tmp);
}

float note2TWamp(byte note) {
	return double(TWamp[note] + 240) / 480;
}

void OnNoteOn(byte channel, byte note, byte velocity) {
	byte manualidx = channel-midibase; // 0:OM, 1:UM
	byte tonewheel = note-23-(1-manualidx)*12;        //C4: midi 60 -> sub_tw 37 (fund_tw 49) OM 1oct shift

	byte voiceNo;

	#ifdef _DEBUG
	Serial.printf("\nNoteOn:  ch %02i note %003i vel %003i ->", channel, note, velocity);
	#endif

	if ((tonewheel > 68) || (tonewheel <1)) {
		return;
	}

	// percussion
	if ((manualidx==0) && (VoiceTab[0].GetIdleVoices() == VoiceTab[0].GetMaxIdleVoices()) && (percnote > 0)) { //check no note is playing
		double pfreq = (double) TWfreq[tonewheel-1 + percnote] / 10;
		perc.frequency(pfreq);
		perc.amplitude(note2TWamp(tonewheel-1 + percnote));
		percEnvelope.noteOn();
		#ifdef _DEBUG
		Serial.printf("\nPercussion: freq_%6f", pfreq);
		#endif
	}

	// tone
	if (VoiceTab[manualidx].GetIdleVoices() > 0) {
		voiceNo = VoiceTab[manualidx].NoteOn(note);
		voiceNo += manualidx*8;
		osc[voiceNo].FundamentalTw(tonewheel);
		envelope[voiceNo].noteOn();
//		clickEnvelope.noteOn();

		digitalWrite(led, HIGH);
		#ifdef _DEBUG
		Serial.printf("man_%i voice_%2i idle_%2i", manualidx, voiceNo, VoiceTab[manualidx].GetIdleVoices());
/*
		Serial.print("\nCPU: ");
		  Serial.print(AudioProcessorUsage());
		  Serial.print(",");
		  Serial.print(AudioProcessorUsageMax());
		  Serial.print("    ");
		  Serial.print("\nMemory: ");
		  Serial.print(AudioMemoryUsage());
		  Serial.print(",");
		  Serial.print(AudioMemoryUsageMax());
		  Serial.print("    ");
*/

		#endif
	}

}

void OnNoteOff(byte channel, byte note, byte velocity) {
	byte manualidx=channel-midibase;
	byte voiceNo;
	//voice_t voiceNo = voices.NoteOff(note);
	voiceNo = VoiceTab[manualidx].NoteOff(note);
	if (voiceNo < 0xFF) {
		voiceNo += manualidx*8;
		envelope[voiceNo].noteOff();
	}
	digitalWrite(led, LOW);

#ifdef _DEBUG
	Serial.printf("\nNoteOff: ch %02i note %003i vel %003i -> ", channel, note, velocity);
	Serial.printf("man_%2i voice_%2i idle_%2i", manualidx, voiceNo, VoiceTab[manualidx].GetIdleVoices());
#endif
}
