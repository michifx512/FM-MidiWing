# FM-MidiWing

#### My custom-built USB-MIDI Controller, made especially for lighing programming, with layout based on MA grandMA2, MA dot2, etc.

<p>
  <img src="photos/top 1.png" width="24%">
  <img src="photos/top 2.png" width="24%">
  <img src="photos/low right angle.png" width="24%">
  <img src="photos/inside low rows.png" width="24%">
</p>

## Features
 - 60 mechanical buttons, RGB Backlit
 - 15 75mm faders
 - Layout made for lighting
 - Setup Menu
 - 2key footswitch port
 - Customizable 
 - 3 led feedback options
    - Fully external (midi with velocity mapped to color)
    - Hybrid ( midi velocity only controls the brightness of some predefined colors)
    - Fully internal

## TO-DO LIST
 
### Physical mods:
- [ ] Add ESP32 alongside Teensy 4.1
- [ ] Connect WS2812B to ESP32
- [ ] [optional] Add 6pin DIN for external device

### Code Mods:
- [x] Better Fader Correction (my pots are some shitty ones from Aliexpress, they have a strange resistance curve, something like the "4B" graph: https://islproducts.com/wp-content/uploads/potentiometer-resistance-taper-b-series-table.png)
- [ ] Communication between Teensy 4.1 and ESP32 
- [ ] Save Options on Flash
- [ ] MultiCore Programming on Esp32
- [ ] ESP32 WebServer Interface for WS2812B Feedback
- [ ] MIDI via BLE with ESP32
