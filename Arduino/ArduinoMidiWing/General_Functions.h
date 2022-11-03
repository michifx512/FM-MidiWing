/*
	Functions Header File
	Version: V1.2
	Changelog: Fixed fader fluctuations, better value mapping for correction
*/

// FUNCTION DECLARATIONS
void noteOn(byte channel, byte pitch, byte velocity);
void noteOff(byte channel, byte pitch, byte velocity);
void controlChange(byte channel, byte control, byte value);

void power_on();
void powerup_animation();

void mode_button();
void setupchanged(byte r, byte c);

void led_setup_feedback_update();
void vegasmode();

void sleepmode();

void readmatrix();
void readfaders();

void midi_in();
void led_feedback_update(byte i);

// ---------- MIDIUSB UTILITY FUNCTIONS ----------
void noteOn(byte channel, byte pitch, byte velocity) {
	midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
	MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
	midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
	MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
	midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
	MidiUSB.sendMIDI(event);
}
// ----------  POWER ON ----------
void power_on() {
	while (!power) {
		if (!digitalRead(MODE_PIN))
			power = true;
		FastLED.clear();
		FastLED.show();
		midi_in();
	}
	powerup_animation();
}

void powerup_animation() {
	for (byte i = 0; i < NUM_LEDS; i++) {
		if (i > 3) {
			leds[i - 8] = CHSV(hue - i * 5, 255, 0);
			leds[i - 7] = CHSV(hue - i * 5, 255, 32);
			leds[i - 6] = CHSV(hue - i * 5, 255, 64);
			leds[i - 5] = CHSV(hue - i * 5, 255, 96);
			leds[i - 4] = CHSV(hue - i * 5, 255, 128);
			leds[i - 3] = CHSV(hue - i * 5, 255, 160);
			leds[i - 2] = CHSV(hue - i * 5, 255, 192);
			leds[i - 1] = CHSV(hue - i * 5, 255, 224);
		}
		if (i < 56) {
			leds[i] = CHSV(hue - i * 5, 255, 255);
			leds[i + 1] = CHSV(hue - i * 5, 255, 224);
			leds[i + 2] = CHSV(hue - i * 5, 255, 192);
			leds[i + 3] = CHSV(hue - i * 5, 255, 160);
			leds[i + 4] = CHSV(hue - i * 5, 255, 128);
			leds[i + 5] = CHSV(hue - i * 5, 255, 96);
			leds[i + 6] = CHSV(hue - i * 5, 255, 64);
			leds[i + 7] = CHSV(hue - i * 5, 255, 32);
			leds[i + 8] = CHSV(hue - i * 5, 255, 0);
		}
		delay(30);
		FastLED.show();
		midi_in();
	}
	for (byte i = NUM_LEDS; i < NUM_LEDS + 8; i++) {
		leds[i - 8] = CHSV(hue - i * 5, 255, 0);
		leds[i - 7] = CHSV(hue - i * 5, 255, 32);
		leds[i - 6] = CHSV(hue - i * 5, 255, 64);
		leds[i - 5] = CHSV(hue - i * 5, 255, 96);
		leds[i - 4] = CHSV(hue - i * 5, 255, 128);
		leds[i - 3] = CHSV(hue - i * 5, 255, 160);
		leds[i - 2] = CHSV(hue - i * 5, 255, 192);
		leds[i - 1] = CHSV(hue - i * 5, 255, 224);
		delay(30);
		FastLED.show();
		midi_in();
	}
	for (byte i = 0; i < NUM_LEDS; i++)
		led_feedback_update(i);

	show = true;
}

// ---------- MODE BUTTON ----------
void mode_button() {
	while (!digitalRead(MODE_PIN)) {
		show = false;
		for (byte r = 0; r < SETUPROWS; r++) {
			pinMode(ROWPINS[r], OUTPUT);
			digitalWrite(ROWPINS[r], LOW);
				setup_state[r][c] = !digitalRead(COLPINS[c]);
				if (setup_state[r][c] != setup_state_pre[r][c]) {
					setup_state_pre[r][c] = setup_state[r][c];
					if (setup_state[r][c])
						setupchanged(r, c);
				}
			}
			pinMode(ROWPINS[r], INPUT); // Sets the row back to high impedance
			delayMicroseconds(125);
		}
		midi_in();
		led_setup_feedback_update();
		FastLED.setBrightness(led_brightness);
		FastLED.show();
		FastLED.clear();
		lastpress = millis();
	}
	if (!show) {
		for (byte i = 0; i < NUM_LEDS; i++)
			led_feedback_update(i);
		show = true;
		FastLED.show();
	}
}

			midi_channel = c;
		else if (c == 5 || c == 6) // CC/NOTE boolean has changed
			fader_output_note = c - 5;
	} else if (r == 1) {
		if (c < 3) // led feedback mode has changed
			ledfeedback = c;
		else if (c > 4 && c < 10) // led preset selector has changed
			switch (ledfeedback) {
				case 1: // hybrid midi feedback
					led_hybrid_selector = c - 5;
					break;

				case 2: // full internal "standalone" feedback
					led_standalone_selector = c - 5;
			}
		else if (c > 9) {
			led_brightness_selector = c - 10;
			led_brightness = led_brightness_selector * 63.75;
		}
	} else if (r == 2)
		if (c == 7)
			vegasmode();
}

// ---------- FastLED FUNCTIONS ----------
void led_setup_feedback_update() {
	for (byte i = 0; i < 5; i++) { // midi channel
		if (i != midi_channel)
			leds[i] = CRGB(30, 30, 30);
		else
			leds[i] = CRGB(0, 255, 255);
	}
	if (fader_output_note == 0) {
		leds[5] = CRGB(255, 127, 0);
		leds[6] = CRGB(30, 30, 30);
	} else {
		leds[5] = CRGB(30, 30, 30);
		leds[6] = CRGB(255, 127, 0);
	}
	switch (ledfeedback) {
		case 0:
			leds[15] = CRGB(255, 127, 0);
			leds[16] = CRGB(30, 30, 30);
			leds[17] = CRGB(30, 30, 30);
			for (byte i = 20; i < 25; i++) // no presets
				leds[i] = CRGB(0, 0, 0);
			break;

		case 1:
			leds[15] = CRGB(30, 30, 30);
			leds[16] = CRGB(255, 127, 0);
			leds[17] = CRGB(30, 30, 30);
			for (byte i = 20; i < 25; i++) { // hybrid preset
				if (i - 20 != led_hybrid_selector)
					leds[i] = CRGB(30, 30, 30);
				else
					leds[i] = CRGB(0, 255, 255);
			}
			break;

		case 2:
			leds[15] = CRGB(30, 30, 30);
			leds[16] = CRGB(30, 30, 30);
			leds[17] = CRGB(255, 127, 0);
			for (byte i = 20; i < 25; i++) { // standalone preset
				if (i - 20 != led_standalone_selector)
					leds[i] = CRGB(30, 30, 30);
				else
					leds[i] = CRGB(0, 255, 255);
			}
			break;
	}
	for (byte i = 25; i < 30; i++) { // note preset
		if (i - 25 != led_brightness_selector)
			leds[i] = CRGB(30, 30, 30);
		else
			leds[i] = CRGB(0, 255, 255);
	}
	leds[37] = CHSV(hue, 255, 255);
	EVERY_N_MILLISECONDS(5) { hue++; }
}

void vegasmode() {
	FastLED.clear();
	bool first = true;
	bool kstate, kstatepre;
	kstatepre = !digitalRead(MODE_PIN);
	while (first) {
		for (byte i = 0; i < NUM_LEDS; i++) {
			leds[i] = CHSV(hue - (i * 17), 255, 255);
		}
		EVERY_N_MILLISECONDS(5) { hue++; }
		kstate = !digitalRead(MODE_PIN);
		if (kstate != kstatepre) {
			kstatepre = kstate;
			if (kstate) {
				first = false;
			}
		}
		FastLED.show();
	}
}

void sleepmode() {
	/*
			if(millis() - lastpress >= 10000){ // 300000 =  5 min
					show = false;
					FastLED.clear();

					for(byte i=255; i>0; i--){
							leds[random(0,NUM_LEDS)] = CHSV(0, 255, i);
							leds[random(0,NUM_LEDS)] = CHSV(0, 255, i-50);
							leds[random(0,NUM_LEDS)] = CHSV(0, 255, i-100);
							leds[random(0,NUM_LEDS)] = CHSV(0, 255, i-150);
							leds[random(0,NUM_LEDS)] = CHSV(0, 255, i-200);
							leds[random(0,NUM_LEDS)] = CHSV(0, 255, i-250);
							FastLED.show();
							delay(5);
					}
			}else{
					show = true;
			}
			*/
}

// ---------- READ FUNCTIONS ----------
void readmatrix() {
	for (byte r = 0; r < NROWS; r++) {
		pinMode(ROWPINS[r], OUTPUT);
		for (byte c = 0; c < NCOLS; c++)
			keystate[r][c] = !digitalRead(COLPINS[c]);
		pinMode(ROWPINS[r], INPUT); // Sets the row back to high impedance
		delayMicroseconds(10);
	}
	fl_state = !digitalRead(FOOTSW_PINL);
	fr_state = !digitalRead(FOOTSW_PINR);
}

void readfaders() {
	for (byte i = 0; i < NFADERS; i++) {
		value[i] = analogRead(FADERPINS[i]);

		smoothed[i] = (alpha)*smoothed[i] + (1 - alpha) * value[i];

		faderval_10bit[i] = int(smoothed[i]);

		if (faderval_10bit[i] < 26)
			faderval_10bit[i] = map(faderval_10bit[i], 0, 25, 0, 136);
		else if (faderval_10bit[i] >= 26 && faderval_10bit[i] <= 52)
			faderval_10bit[i] = map(faderval_10bit[i], 26, 52, 137, 210);
		else if (faderval_10bit[i] >= 53 && faderval_10bit[i] <= 958)
			faderval_10bit[i] = map(faderval_10bit[i], 53, 958, 211, 792);
		else if (faderval_10bit[i] >= 959 && faderval_10bit[i] <= 997)
			faderval_10bit[i] = map(faderval_10bit[i], 959, 997, 793, 886);
		else
			faderval_10bit[i] = map(faderval_10bit[i], 998, 1023, 887, 1023);

		if (faderval_10bit[i] == faderval_10bit_pre[i] + 1)
			faderval_10bit[i]--;
		else if (faderval_10bit[i] == faderval_10bit_pre[i] - 1)
			faderval_10bit[i]++;
		if (faderval_10bit[i] == faderval_10bit_pre[i] + 2)
			faderval_10bit[i] -= 2;
		else if (faderval_10bit[i] == faderval_10bit_pre[i] - 2)
			faderval_10bit[i] += 2;
		faderval_10bit_pre[i] = faderval_10bit[i];

		smoothed_b[i] = (alpha)*smoothed_b[i] + (1 - alpha) * faderval_10bit[i];
		faderval_10bit[i] = int(smoothed_b[i]);

		faderval_7bit[i] = faderval_10bit[i] / 8;
	}
// ---------- MIDI OUT COMMANDS ----------
void fader_out() {
	for (byte i = 0; i < NFADERS; i++) {
		if (faderval_7bit[i] !=
			faderval_7bit_pre[i]) { // send only if value changed
			faderval_7bit_pre[i] = faderval_7bit[i];
			if (fader_output_note == false) // cc values need to be sent
				controlChange(midi_channel, i + 60, faderval_7bit[i]);
			else
				noteOn(midi_channel, i + 60, faderval_7bit[i]);
			MidiUSB.flush();
			lastpress = millis();
		}
	}
}

void button_out() {
	for (byte r = 0; r < NROWS; r++)
		for (byte c = 0; c < NCOLS; c++)
			if (keystate[r][c] != keystate_pre[r][c]) { // send only if state changed
				keystate_pre[r][c] = keystate[r][c];
				if (keystate[r][c])
					noteOn(midi_channel, note[r][c], 127);
				else
					noteOff(midi_channel, note[r][c], 0);
				MidiUSB.flush();
				lastpress = millis();
			}
	if (fl_state != fl_state_pre) {
		fl_state_pre = fl_state;
		if (fl_state == true)
			noteOn(midi_channel, fl_note, 127);
		else
			noteOff(midi_channel, fl_note, 0);
		MidiUSB.flush();
		lastpress = millis();
	}
	if (fr_state != fr_state_pre) {
		fr_state_pre = fr_state;
		if (fr_state == true)
			noteOn(midi_channel, fr_note, 127);
		else
			noteOff(midi_channel, fr_note, 0);
		MidiUSB.flush();
		lastpress = millis();
	}
}

// ---------- LED FEEDBACK ----------

void midi_in() {
	midiEventPacket_t rx;
	do {
		rx = MidiUSB.read();
		if (rx.header != 0)
			for (byte i = 0; i < NUM_LEDS; i++)
				if (rx.byte2 == i) {
					if (rx.byte1 == 0x90) {
						if (ledfeedback == 0) // full midi in feedback
							feedback_color[i] = rx.byte3;
						else { // hybrid feedback
							if (rx.byte3 == 0x7F)
								feedback_color[i] = hybrid_color[led_hybrid_selector][i];
							else if (rx.byte3 == 0)
								feedback_color[i] = hybrid_color[led_hybrid_selector][i] - 2;
						}
					} else if (rx.byte1 == 0x80)
						feedback_color[i] = 0;

					if (show) {
						led_feedback_update(i);
						FastLED.show();
					}
				}
	} while (rx.header != 0);
}

void led_feedback_update(byte i) {
	if (feedback_color[i] > 123)
		leds[i] = CHSV(hue_vel[feedback_color[i]], 0, value_vel[feedback_color[i]]);
	else
		leds[i] =
			CHSV(hue_vel[feedback_color[i]], 255, value_vel[feedback_color[i]]);
}