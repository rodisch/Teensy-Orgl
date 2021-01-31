# T-ORGL

A Tonewheel organ emulation for Teensy 3.2 + Audio board
based on ideas and code of 'roto2' of Peter Teichmann, https://github.com/pteichman/roto2

- 2 parts as upper- and lower manual, with independent drawbar settings
- 8 note polyphony per part
- A (flimsy) rotary and distortion emulator
- Vibrato and Chorus 
- Comprehensive MIDI-control 
- dynamic generation of wavetables with regard to fold over of tonewheels on high- and sub-registers
- introducing a 9:1 audio-mixer (see AudioSum.h)

![Block diagram](/doc/TORGL.png)

## Default MIDI setup

Used MIDI channels: 1, 2 

Function| CC-No.| Values
---|---|---
Drawbar 16"...1" | 41..48, 28 | CC values 0..127 are mapped to drawbar position 0..8
Vibrato/Chorus*| 21| 0..127 mapped to V3...V1, Off, C1..C3
Perc. volume*| 22| 0..127 mapped to Vol 2nd, Off, Vol 3rd
Perc. decay| 23| 0..127: fast..slow
Rotary speed| 01 (Modulation)| 0..127: slow..whirl (no rampup/rampdown)
Rotary balance| 24| 0..127: dry(direct)...wet(effect only)
Distortion|25| 0..127: off...insane
Keyclick volume| 26| 0..127: off...clicky
Release|72| Tone envelope release
Attack|73| Tone envelope attack
DSP Volume|07 (Volume)| Volume of upper/lower tonegenerator (DSP)
Audio volume|11 (Expression)| Volume of audio board 0..127 mapped to 40..90%
EQ low|75| Audio board EQ low
EQ high|76| Audio board EQ high

Settings are **not** stored im EEPROM-memory.
CC-numbers can by taylored to your own liking in MidiSetup.h

## Memory footprint
Lot of space for improvements
```
RAM:   [=====     ]  46.4% (used 30388 bytes from 65536 bytes)
Flash: [==        ]  22.8% (used 59848 bytes from 262144 bytes)
```

## Usage in Arduino IDE (Teensyduino)
This repository is a copy of my PlatformIO workspace. 

### Transfer to Arduin:
- copy the 'src' folder to an appropriate place, e.g. your Arduino projects folder
- rename directory to 'TORGL'
- open TORGL.ino in Arduino IDE

```
Board: "Teensy 3.1/3.2"
USB-Type: "Serial + MIDI"
```

