// Copyright (c) 2015 Peter Teichman
//
// Modifications of depth and speed by Roman Dischler 2018

#include "Vibrato.h"

uint32_t triangle(uint32_t phase) {
    // Convert 32-bit phase into signed 31-bit triangle.
    if (phase & 0x80000000) {
        return 0x80000000 + (0x80000000 - phase);
    } else {
        return phase;
    }
}

int32_t lerp(int32_t a, int32_t b, uint16_t scale) {
    return ((0xFFFF - scale)*a + scale*b) >> 16;
}

void Vibrato::update(void) {
    audio_block_t *in = receiveReadOnly(0);
    if (in == NULL) {
        return;
    }

    audio_block_t *out = allocate();
    if (out == NULL) {
        release(in);
        return;
    }

    uint8_t _wp = wp;  //local write pointer
    uint32_t _phase = scan_phase;

    for (int i=0; i<AUDIO_BLOCK_SAMPLES; i++) {
        // Write the input audio to our delay line.
        buf[_wp] = in->data[i];

        // Read the delay line into the output buffer.

        // 7 bits of _wp to 1..8; subtract triangle. This gives 5
        // bits of sway on a 7 bit counter.
        int32_t _rp = (_wp << 24) - (int32_t)(triangle(_phase) >> depth);

        int16_t pos = _rp >> 24;
        int16_t a = buf[pos & 0x7f];
        int16_t b = buf[++pos & 0x7f];
        int16_t val = lerp(a, b, (_rp >> 8) & 0xFFFF);
        if (mix) {
          // val = ((int32_t)val + buf[loc_wp] ) >> 1 ;
          val = ((int32_t)val + (buf[_wp]>>1) ) ; //RD changed balance wet/dry
        }

        out->data[i] = val;

        // _phase += 679632; // speed of vibrato ~7Hz
        _phase += 579632;    // RD: speed ~6Hz

        // Increment and wrap loc_wp.
        _wp++;
        _wp &= 0x7f;
    }

    wp = _wp;
    scan_phase = _phase;

    transmit(out, 0);
    release(out);
    release(in);
}
