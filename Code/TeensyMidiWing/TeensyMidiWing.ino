/*
  Project Name: FM-MidiWing 
  Second Name (old): Teensy USB-MIDI CONTROLLER
  Author: EFF Service - Francesco Michieletto
  Version: V2.1
*/

// Libraries
#include <FastLED.h>
#include <MIDIUSB.h>

// Headers
#include "Setup.h"
#include "General_Functions.h"

// ---------- SETUP and LOOP ----------
void setup() {
    pinSetup();
    FastLED_InitialSetup();
    powerOn();
    //Serial.begin(9600);
    FastLED.clear();
    FastLED.show();
}

void loop() {
    setupPage();
    sleepMode();
    readMatrix();
    readFaders();
    buttonOut();
    faderOut();
    midiIn();
}