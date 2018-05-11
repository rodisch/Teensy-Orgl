
const int led = 13; // OnBoard LED Teensy

#include "AudioSetup.h"
#include "Leslie.h"
#include "MidiSetup.h"

#define _DEBUG

//--- Connect Tonegenerator and Leslie
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
	Serial.print("/nTeensy Orgl1");
	#endif

	// Audio system
	AudioInit();
	LeslieInit();

	// MIDI
	usbMIDI.setHandleNoteOff(OnNoteOff);
	usbMIDI.setHandleNoteOn(OnNoteOn);
	usbMIDI.setHandleControlChange(OnControlChange);
	/*
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
  if(millis() - last_time >= 5000) {
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

		// if (LslInputPk.available()){
		// 	Serial.print("LslPeak = ");
		// 	Serial.println(LslInputPk.read());
		// }
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
