/*
   Project Name: Teensy USB-MIDI CONTROLLER
   Author: EFF Service - Francesco Michieletto
   Version: V1.2
*/

// Libraries
#include <FastLED.h>
#include <MIDIUSB.h>

// Headers
#include "Setup.h"

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

	/*for(byte i = 0; i<NUM_LEDS; i++)
		leds[i] = CHSV(hue - (i * 17), 255, 255);
	EVERY_N_MILLISECONDS(5){
		hue++;
	}*/
	
	readmatrix();
	readfaders();

	fader_out();
	button_out();

	midi_in();
}