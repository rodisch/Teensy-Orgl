
/*
A Tonewheel organ emulation for Teensy 3.1+Audio board
based on ideas and code of 'roto2' of Peter Teichmann, https://github.com/pteichman/roto2

New:
- Generation of Wavetables with regard to fold over of tonewheels on high and sub registers
- A flimsy Rotary emulation
- Comprehensive MIDI-control
*/


//#define _DEBUG

#include "AudioSetup.h"
#include "Leslie.h"
#include "MidiSetup.h"

const int led = 13; // OnBoard LED Teensy

//--- Connections Tonegenerator, Leslie and I2S-output
AudioConnection patchInst[] = {
	{TgOut, 0, LslInput,0},   // -> input Leslie
	{LslOutL, 0, i2s_out, 0}, // Output Leslie -> audioshield
	{LslOutR, 0, i2s_out, 1},
	// {LslOutL, 0, usb_out, 0}, // Output Leslie -> USB
	// {LslOutR, 0, usb_out, 1}
};

void setup() {
	pinMode(led, OUTPUT);

	#ifdef _DEBUG
	Serial.begin(115200);
	Serial.print("/nTeensy Orgl");
	#endif

	// Audio system
	AudioInit();
	LeslieInit();

	// MIDI  -> MidiSetup.h
	usbMIDI.setHandleNoteOff(OnNoteOff);
	usbMIDI.setHandleNoteOn(OnNoteOn);
	usbMIDI.setHandleControlChange(OnControlChange);
	/* not implemented
	 usbMIDI.setHandleVelocityChange(OnVelocityChange);
	 usbMIDI.setHandleProgramChange(OnProgramChange);
	 usbMIDI.setHandleAfterTouch(OnAfterTouch);
	 usbMIDI.setHandlePitchChange(OnPitchChange);
	 */

	// Blink LED
	digitalWrite(led, HIGH);
	delay(100);
	digitalWrite(led, LOW);
	delay(100);
	digitalWrite(led, HIGH);
	delay(100);
	digitalWrite(led, LOW);
}

unsigned long last_time = millis();
void loop() {
	usbMIDI.read(); // USB MIDI receive

	#ifdef _DEBUG
	if (millis() - last_time >= 5000)
	{
		Serial.print("\nProc = ");
		Serial.print(AudioProcessorUsage());
		Serial.print(" (");
		Serial.print(AudioProcessorUsageMax());
		Serial.print("),  Mem = ");
		Serial.print(AudioMemoryUsage());
		Serial.print(" (");
		Serial.print(AudioMemoryUsageMax());
		Serial.println(")");
		last_time = millis();
	}
	#endif

	// LED as clipping indicator
	if (LslInputPk.available()){
		if (LslInputPk.read()>0.95)  //Leslie input is clipping
			digitalWrite(led, HIGH);
		else
			digitalWrite(led, LOW);
	}

}
