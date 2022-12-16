/*
  Project Name: FM-MidiWing 
  Second Name (old): Teensy USB-MIDI CONTROLLER
  Author: EFF Service - Francesco Michieletto
  Version: V2.0
*/

// Libraries
#include <FastLED.h>
#include <MIDIUSB.h>

// Headers
#include "Setup.h"
#include "General_Functions.h"

// ---------- SETUP and LOOP ----------
void setup() {
	pin_setup();
	FastLED_InitialSetup();
	power_on();
	//Serial.begin(9600);
	FastLED.clear();
	FastLED.show();
}

void loop() {
	mode_button();
	sleepmode();
	readmatrix();
	readfaders();
	fader_out();
	button_out();
	midi_in();
}