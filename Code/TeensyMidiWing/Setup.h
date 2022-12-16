/*
  Setup & Settings Header File
  Version: V2.1
*/

// ---------- CONSTANTS ----------
// DEFINES
#define NROWS 4
#define NCOLS 15
#define NFADERS 15

#define MODE_PIN 24 // SETUP/MODE PIN
#define FOOTSW_PINL 11
#define FOOTSW_PINR 12

// FastLED DEFINES
#define NUM_LEDS 60
#define LEDS_DATA_PIN 26
#define COLOR_ORDER GRB
#define CHIPSET WS2812B
#define INITIAL_BRIGHTNESS 255
#define VOLTS 5
CRGB leds[NUM_LEDS];

uint8_t hue = 0;
byte hsv_val = 0;

const byte FADERPINS[NFADERS] = {A13, A14, A15, A16, A17, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9};
const byte ROWPINS[NROWS] = {5, 4, 3, 2};
const byte COLPINS[NCOLS] = {32, 31, 30, 29, 28, 33, 34, 35, 36, 37, 10, 9, 8, 7, 6};

bool show = false;

// ---------- VARIABLES ----------

// SETUP VARIABLES
bool power = false;

byte midi_channel = 0;
bool fader_output_note = false; // false-> CC VALUES, true->NOTE VALUES
byte ledfeedback = 0;			// 0 -> full midi feedback, 1-> hybrid midi feedback, 2-> full standalone feedback
byte led_hybrid_selector = 0;
byte led_standalone_selector = 0;
byte led_brightness_selector = 4;
byte led_brightness = 255;

#define SETUPROWS 3
#define SETUPCOLS 15
bool setup_state[SETUPROWS][SETUPCOLS];
bool setup_state_pre[SETUPROWS][SETUPCOLS];

int lastpress = 0; // used with millis() for sleep mode

// READINGS VARIABLES
bool keystate[NROWS][NCOLS] = {0}; // matrix
bool keystate_pre[NROWS][NCOLS] = {0};
bool fl_state = 0; // Footswitch Left
bool fl_state_pre = 0;
bool fr_state = 0; // Footswitch Right
bool fr_state_pre = 0;
int faderval_10bit[NFADERS] = {0};
int faderval_10bit_pre[NFADERS] = {0};
byte faderval_7bit[NFADERS] = {0};
byte faderval_7bit_pre[NFADERS] = {0};

float value[NFADERS] = {0};
float smoothed[NFADERS] = {0};
float smoothed_b[NFADERS] = {0};
float alpha = 0.93;

// MIDI
byte note[NROWS][NCOLS] = {
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},			  // 1st row
	{15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29}, // 2nd row
	{30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44}, // 3rd row
	{45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59}  // 4th row
};
byte fl_note = 126;
byte fr_note = 127;

// ---------- COLOR PALETTE ----------
byte hue_vel[128] = {0, 0, 0, 0, 6, 6, 6, 12, 12, 12, 18, 18, 18, 24, 24, 24, 30, 30, 30, 36, 36, 36, 42, 42, 42, 48, 48, 48, 54, 54, 54, 60, 60, 60, 66, 66, 66, 72, 72, 72, 78, 78, 78, 84, 84, 84, 90, 90, 90, 96, 96, 96, 102, 102, 102, 108, 108, 108, 114, 114, 114, 120, 120, 120, 126, 126, 126, 132, 132, 132, 138, 138, 138, 144, 144, 144, 150, 150, 150, 156, 156, 156, 162, 162, 162, 168, 168, 168, 174, 174, 174, 180, 180, 180, 186, 186, 186, 192, 192, 192, 198, 198, 198, 204, 204, 204, 210, 210, 210, 216, 216, 216, 222, 222, 222, 228, 228, 228, 234, 234, 234, 240, 240, 240, 0, 0, 0, 0};
byte value_vel[128] = {0, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 160, 255, 64, 128, 192, 255};

byte feedback_color[NUM_LEDS] = {0};
byte hybrid_color[5][NUM_LEDS] = {
	{81, 81, 81, 69, 69, 69, 69, 69, 3, 3, 3, 3, 3, 3, 3, 81, 81, 81, 69, 69, 69, 69, 69, 3, 3, 3, 3, 3, 3, 3, 81, 81, 81, 69, 69, 69, 69, 69, 3, 3, 3, 3, 3, 3, 3, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24},
	{0},
	{0},
	{0},
	{0}};

// ---------- FUNCTIONS ----------
void pin_setup() {
	for (byte r = 0; r < NROWS; r++) // matrix rows
		pinMode(ROWPINS[r], INPUT);

	for (byte c = 0; c < NCOLS; c++) // matrix columns
		pinMode(COLPINS[c], INPUT_PULLUP);

	pinMode(MODE_PIN, INPUT_PULLUP);
	pinMode(FOOTSW_PINL, INPUT_PULLUP);
	pinMode(FOOTSW_PINR, INPUT_PULLUP);
}

void FastLED_InitialSetup() {
	FastLED.addLeds<CHIPSET, LEDS_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
	FastLED.setCorrection(TypicalLEDStrip);
	FastLED.setBrightness(INITIAL_BRIGHTNESS);
	FastLED.clear();
	FastLED.show();
}
