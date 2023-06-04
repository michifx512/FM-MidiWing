/*
	Functions Header File
	Version: V2.1
	Changelog: Changed Fader Value Correction Engine, fixed small fluctuation algorithm
*/

// FUNCTION DECLARATIONS
void noteOn(byte channel, byte pitch, byte velocity);
void noteOff(byte channel, byte pitch, byte velocity);
void controlChange(byte channel, byte control, byte value);

void powerOn();
void powerupAnimation();

void setupPage();
void setupChanged(byte r, byte c);

void led_setup_feedback_update();
void vegasMode();

void sleepMode();

int correctFader(int val);
void readMatrix();
void readFaders();

void buttonOut();
void faderOut();

void midiIn();
void led_feedback_update(byte i);



// ---------- MIDIUSB UTILITY FUNCTIONS ----------
void noteOn(byte channel, byte pitch, byte velocity) {
    midiEventPacket_t noteOn = { 0x09, 0x90 | channel, pitch, velocity };
    MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
    midiEventPacket_t noteOff = { 0x08, 0x80 | channel, pitch, velocity };
    MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
    midiEventPacket_t event = { 0x0B, 0xB0 | channel, control, value };
    MidiUSB.sendMIDI(event);
}

// ----------  POWER ON ----------
void powerOn() {
    while (!power) {
        if (!digitalRead(MODE_PIN))
            power = true;
        FastLED.clear();
        FastLED.show();
        midiIn();  // just to flush all incoming data
    }
    powerupAnimation();
    lastPress = millis();
}

void powerupAnimation() {
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
        midiIn();
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
        midiIn();
    }
    for (byte i = 0; i < NUM_LEDS; i++)
        led_feedback_update(i);

    showing = true;
}

// ---------- MODE BUTTON ----------
void setupPage() {
    while (!digitalRead(MODE_PIN)) {
        showing = false;
        for (byte r = 0; r < SETUPROWS; r++) {
            pinMode(ROWPINS[r], OUTPUT);
            digitalWrite(ROWPINS[r], LOW);
            for (byte c = 0; c < SETUPCOLS; c++) {
                setup_state[r][c] = !digitalRead(COLPINS[c]);
                if (setup_state[r][c] != setup_state_pre[r][c]) {
                    setup_state_pre[r][c] = setup_state[r][c];
                    if (setup_state[r][c])
                        setupChanged(r, c);
                }
            }
            pinMode(ROWPINS[r], INPUT);  // Sets the row back to high impedance
            delayMicroseconds(15);
        }
        midiIn();
        led_setup_feedback_update();
        FastLED.setBrightness(ledBrightness);
        FastLED.show();
        //FastLED.clear();
        lastPress = millis();
    }
    if (!showing && !sleeping) {
        for (byte i = 0; i < NUM_LEDS; i++)
            led_feedback_update(i);
        showing = true;
        FastLED.show();
    }
}

void setupChanged(byte r, byte c) {
    if (r == 0) {
        if (c < 5)  // midi channel has changed
            midiChannel = c;
        else if (c == 5 || c == 6)  // CC/NOTE boolean has changed
            fader_output_note = c - 5;
    } else if (r == 1) {
        if (c < 3)  // led feedback mode has changed
            ledFeedback = c;
        else if (c > 4 && c < 10)  // led preset selector has changed
            switch (ledFeedback) {
                case 1:  // hybrid midi feedback
                    led_hybrid_selector = c - 5;
                    break;

                case 2:  // full internal "standalone" feedback
                    led_standalone_selector = c - 5;
            }
        else if (c > 9) {
            ledBrightness_selector = c - 10;
            ledBrightness = ledBrightness_selector * 63.75;
        }
    } else if (r == 2)
        if (c == 7)
            vegasMode();
}

// ---------- FastLED FUNCTIONS ----------
void led_setup_feedback_update() {
    for (byte i = 0; i < 5; i++) {  // midi channel
        if (i != midiChannel)
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
    switch (ledFeedback) {
        case 0:
            leds[15] = CRGB(255, 127, 0);
            leds[16] = CRGB(30, 30, 30);
            leds[17] = CRGB(30, 30, 30);
            for (byte i = 20; i < 25; i++)  // no presets
                leds[i] = CRGB(0, 0, 0);
            break;

        case 1:
            leds[15] = CRGB(30, 30, 30);
            leds[16] = CRGB(255, 127, 0);
            leds[17] = CRGB(30, 30, 30);
            for (byte i = 20; i < 25; i++) {  // hybrid preset
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
            for (byte i = 20; i < 25; i++) {  // standalone preset
                if (i - 20 != led_standalone_selector)
                    leds[i] = CRGB(30, 30, 30);
                else
                    leds[i] = CRGB(0, 255, 255);
            }
            break;
    }
    for (byte i = 25; i < 30; i++) {  // note preset
        if (i - 25 != ledBrightness_selector)
            leds[i] = CRGB(30, 30, 30);
        else
            leds[i] = CRGB(0, 255, 255);
    }
    leds[37] = CHSV(hue, 255, 255);
    EVERY_N_MILLISECONDS(5) {
        hue++;
    }
}

void vegasMode() {
    FastLED.clear();
    bool first = true;
    bool kstate, kstatepre;
    kstatepre = !digitalRead(MODE_PIN);
    while (first) {
        for (byte i = 0; i < NUM_LEDS; i++) {
            leds[i] = CHSV(hue - (i * 17), 255, 255);
        }
        EVERY_N_MILLISECONDS(5) {
            hue++;
        }
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

void sleepMode() {
    const byte fade_k = 12;
    if (millis() - lastPress >= SLEEP_AFTERTIME) {  // 300000 =  5 min
        showing = false;
        sleeping = true;
        EVERY_N_MILLISECONDS(20 * fade_k) {
            leds[random(NUM_LEDS)] = CHSV(random8() / 10 + 125, 255, 255);
        }
        EVERY_N_MILLISECONDS(fade_k) {
            fadeToBlackBy(leds, NUM_LEDS, 1);
        }
        FastLED.show();
    } else if (sleeping) {
        sleeping = false;
    }
}

// ---------- READ FUNCTIONS ----------
void readMatrix() {
    nButtonsChanged = 0;
    byte i = 0, k = 0;
    for (byte r = 0; r < NROWS; r++) {
        pinMode(ROWPINS[r], OUTPUT);
        digitalWrite(ROWPINS[r], LOW);
        for (byte c = 0; c < NCOLS; c++) {
            keystate[i] = !digitalRead(COLPINS[c]);
            if (keystate[i] != keystate_pre[i]) {
                keystate_pre[i] = keystate[i];
                keyChanged[k] = i;
                k++;
                nButtonsChanged++;
            }
            i++;
        }
        pinMode(ROWPINS[r], INPUT);  // Sets the row back to high impedance
        delayMicroseconds(5);
    }
    fl_state = !digitalRead(FOOTSW_PINL);
    fr_state = !digitalRead(FOOTSW_PINR);
}

int correctFader(int val) {
    int final_val = 0;
    float corrected_val = 0;

    if (val < 512) {
        corrected_val = log10(val + 1) / log10(1.012);
    } else {
        int percX = 540, percY = 7000;
        float logbase = 0.996;

        corrected_val = log10((double)(val - 512) / percX) / log10(logbase);
        corrected_val = 1 / (corrected_val)*percY + 512;
    }

    final_val = (val + corrected_val) / 2;
    return final_val;
}

void readFaders() {
    fadersChanged = false;
    for (byte i = 0; i < NFADERS; i++) {
        smoothed[i] = (alpha)*smoothed[i] + (1 - alpha) * analogRead(FADERPINS[i]);
        //Serial.print("Raw value [");Serial.print(i);Serial.print("]:");Serial.println(faderval_10bit[i]);
        faderval_10bit[i] = correctFader(int(smoothed[i]));
        //Serial.print("CORR value [");Serial.print(i);Serial.print("]:");Serial.println(faderval_10bit[i]);Serial.println();
        //Serial.print(faderval_10bit[i]);Serial.print("\t");
        if (abs(faderval_10bit[i] - faderval_10bit_pre[i]) < 8 && faderval_10bit[i] > 2 && faderval_10bit[i] < 1021) {
            faderval_10bit[i] = faderval_10bit_pre[i];
        } else {
            fadersChanged = true;
            faderval_10bit_pre[i] = faderval_10bit[i];
        }

        if (faderval_10bit[i] <= 512) {
            faderval_7bit[i] = faderval_10bit[i] / 8;
        } else {
            faderval_7bit[i] = (float)faderval_10bit[i] / 8 + 0.5;
        }
    }
    //Serial.println();
}

// ---------- MIDI OUT COMMANDS ----------
void buttonOut() {
    for (byte i = 0; i < nButtonsChanged; i++) {
        if (keystate[keyChanged[i]])
            noteOn(midiChannel, keyChanged[i], 127);
        else
            noteOff(midiChannel, keyChanged[i], 0);
        MidiUSB.flush();
        lastPress = millis();
    }
    if (fl_state != fl_state_pre) {
        fl_state_pre = fl_state;
        if (fl_state == true)
            noteOn(midiChannel, fl_note, 127);
        else
            noteOff(midiChannel, fl_note, 0);
        MidiUSB.flush();
    }
    if (fr_state != fr_state_pre) {
        fr_state_pre = fr_state;
        if (fr_state == true)
            noteOn(midiChannel, fr_note, 127);
        else
            noteOff(midiChannel, fr_note, 0);
        MidiUSB.flush();
    }
}

void faderOut() {
    if (fadersChanged) {
        for (byte i = 0; i < NFADERS; i++) {
            if (faderval_7bit[i] != faderval_7bit_pre[i]) {  // send only if value changed
                faderval_7bit_pre[i] = faderval_7bit[i];
                if (fader_output_note == false)  // send CC values for Faders
                    controlChange(midiChannel, i + 60, faderval_7bit[i]);
                else
                    noteOn(midiChannel, i + 60, faderval_7bit[i]);
                MidiUSB.flush();
                lastPress = millis();
            }
        }
    }
}

// ---------- LED FEEDBACK ----------

void midiIn() {
    midiEventPacket_t rx;
    do {
        rx = MidiUSB.read();
        if (rx.header != 0)
            for (byte i = 0; i < NUM_LEDS; i++)
                if (rx.byte2 == i) {
                    if (rx.byte1 == 0x90) {
                        if (ledFeedback == 0)  // full midi in feedback
                            feedback_color[i] = rx.byte3;
                        else {  // hybrid feedback
                            if (rx.byte3 == 0x7F)
                                feedback_color[i] = hybrid_color[led_hybrid_selector][i];
                            else if (rx.byte3 == 0)
                                feedback_color[i] = hybrid_color[led_hybrid_selector][i] - 2;
                        }
                    } else if (rx.byte1 == 0x80)
                        feedback_color[i] = 0;
                    if (sleeping) lastPress = millis();
                    if (showing) {
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