#pragma once

#include "Arduino.h"

const uint8_t VIDLE = 0xFF;  //flag for idle voice

class VoiceTable
{
public:
    VoiceTable(uint8_t buflen){
      notes = new uint8_t[buflen];
      maxVoices = buflen;
      for (int i=0; i<maxVoices; i++) {
            notes[i] = VIDLE;
      }
      idleVoices=maxVoices;
    }

    uint8_t GetIdleVoices(){
    	return idleVoices;
    };

    uint8_t GetMaxVoices(){
    	return maxVoices;
    };

    uint8_t NoteOn(byte note){
        uint8_t vc=maxVoices;
        if (idleVoices) {
          vc=findNote(VIDLE);
          notes[vc]=note;
          idleVoices--;
        }
        return vc;
    };

    uint8_t NoteOff(byte note){
    	uint8_t vc=VIDLE;
    	if (idleVoices<maxVoices) {
    		vc=findNote(note);
    		if (vc<maxVoices) {
    			notes[vc]=VIDLE;
    			idleVoices++;
    		}
    	}
    	return vc;
    };

private:
    uint8_t maxVoices;
    uint8_t idleVoices;
    uint8_t *notes; //buffer vor note -> voice

    uint8_t findNote(uint8_t note){
    	uint8_t vc=maxVoices;
    	for (int i=0; i<maxVoices; i++) {
    		if (notes[i] == note) {
    			vc=i;
    			break;
    		}
    	}
    	return vc;
    };
};
