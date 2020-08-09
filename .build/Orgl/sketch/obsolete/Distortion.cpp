
#include "Distortion.h"
#include "utility/dspinst.h"


#define MULTI_UNITYGAIN 65536


static void applyGain(int16_t *data, int32_t mult)
{
	uint32_t *p = (uint32_t *)data;
	const uint32_t *end = (uint32_t *)(data + AUDIO_BLOCK_SAMPLES);

	do {
		uint32_t tmp32 = *p; // read 2 samples from *data
		int32_t val1 = signed_multiply_32x16b(mult, tmp32);
		int32_t val2 = signed_multiply_32x16t(mult, tmp32);
		val1 = signed_saturate_rshift(val1, 16, 0);
		val2 = signed_saturate_rshift(val2, 16, 0);
		*p++ = pack_16b_16b(val2, val1);
	} while (p < end);
}

/*
static void applyGainThenAdd(int16_t *data, const int16_t *in, int32_t mult)
{
	uint32_t *dst = (uint32_t *)data;
	const uint32_t *src = (uint32_t *)in;
	const uint32_t *end = (uint32_t *)(data + AUDIO_BLOCK_SAMPLES);

	if (mult == MULTI_UNITYGAIN) {
		do {
			uint32_t tmp32 = *dst;
			*dst++ = signed_add_16_and_16(tmp32, *src++);
			tmp32 = *dst;
			*dst++ = signed_add_16_and_16(tmp32, *src++);
		} while (dst < end);
	} else {
		do {
			uint32_t tmp32 = *src++; // read 2 samples from *data
			int32_t val1 = signed_multiply_32x16b(mult, tmp32);
			int32_t val2 = signed_multiply_32x16t(mult, tmp32);
			val1 = signed_saturate_rshift(val1, 16, 0);
			val2 = signed_saturate_rshift(val2, 16, 0);
			tmp32 = pack_16b_16b(val2, val1);
			uint32_t tmp32b = *dst;
			*dst++ = signed_add_16_and_16(tmp32, tmp32b);
		} while (dst < end);
	}
}
*/

void Distortion::update(void)
{
	audio_block_t *block;
	int32_t x1;
	int32_t x2;

	block = receiveWritable();
	if (!block) return;

	applyGain(block->data, multiplier_in);
	for (int i=0; i<AUDIO_BLOCK_SAMPLES; i++) {
		x1 = block->data[i];
		x2 = x1 + (x1>>1) - ((((x1 * x1 ) >> 16) * x1) >> 17); // 1.5*x-0.5*x^3
		block->data[i] = x2;
//		block->data[i] = x1;
	}
	applyGain(block->data, multiplier_out);

	transmit(block);
	release(block);

}


