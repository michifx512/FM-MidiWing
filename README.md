# FM-MidiWing

#### My custom-built USB-MIDI Controller, made especially for lighing programming, with softwares such as MA grandMA2, MA dot2, etc.

## FEATURES
 - 60 mechanical buttons, RGB Backlit
 - 15 75mm faders
 - Layout made for lighting
 - Setup Menu
 - 2key footswitch port
 - Customizable
 - 3 ways of led feedback
    - Fully external (midi with velocity mapped to color)
    - Hybrid ( midi velocity only controls the brightness of some predefined colors)
    - Fully internal


## TO-DO LIST
 - [ ] Test Fader Correction with ESP32 12bit ADC
 - [ ] Physical mods:
    - [ ] Add ESP32 alongside Teensy 4.1
      - [ ] Make them communicate with each other
    - [ ] Connect Faders to ESP32
    - [ ] Connect WS2812B to ESP32
    - [ ] [optional] Add 6pin DIN for External Device
 - [ ] Code Mods:
   - [ ] Communication between Teensy 4.1 and ESP32 
   - [ ] \b Better Fader correction with 12bit ADC
   - [ ] Better WS2812B management
   - [ ] ESP32 WebServer Interface for WS2812B Feedback
   - [ ] MIDI via BLE with ESP32