#pragma once

// frequencies: freq = x/10
const uint16_t TWfreq[91]=
{
		327,346,367,389,412,436,463,490,519,550,583,617,
		654,693,734,778,824,873,925,980,1038,1100,1165,1234,
		1308,1385,1468,1556,1648,1745,1850,1960,2076,2200,2330,2469,
		2615,2771,2937,3111,3296,3491,3700,3920,4151,4400,4661,4937,
		5231,5541,5874,6222,6592,6982,7400,7840,8303,8800,9322,9874,
		10462,11083,11748,12444,13184,13964,14800,15680,16605,17600,18643,19749,
		20923,22166,23496,24889,26368,27927,29600,31360,33211,35200,37287,39497,
		41891,44400,47040,49816,52800,55930,59246
};

// amplitudes: amplitude= x/200+0.9
const uint8_t TWamp[91]=
{
		230,236,243,250,254,254,254,254,252,250,247,244,
		241,238,235,230,225,219,213,206,199,191,183,174,
		164,154,143,133,123,112,101,89,81,76,70,64,
		60,57,54,51,47,44,40,36,34,33,31,30,
		29,29,28,27,27,26,25,25,25,25,25,25,
		25,25,25,25,25,25,25,24,24,23,22,21,
		19,16,13,11,7,4,1,
};

// foldback key ranges
const uint8_t rng_key[7]={1,13,44,49,52,56,61};

// Multiplier for foldback of harmonics
const uint8_t rng_mult [7][9]={
		{2,3,2,4,6,8,10,12,16},
		{1,3,2,4,6,8,10,12,16},
		{1,3,2,4,6,8,10,12, 8},
		{1,3,2,4,6,8,10, 6, 8},
		{1,3,2,4,6,8, 5, 6, 8},
		{1,3,2,4,6,4, 5, 6, 4},
		{1,3,2,4,3,4, 5, 6, 4}
};

// Amplitudes
const uint8_t rng_amp [7][9]={
		{31, 31, 31, 29, 25, 22, 20, 19, 17},
		{25, 22, 24, 19, 17, 17, 16, 16, 16},
		{18, 17, 18, 16, 16, 16, 15, 15, 16},
		{17, 16, 17, 16, 16, 15, 15, 16, 15},
		{16, 16, 16, 16, 16, 15, 16, 16, 15},
		{16, 16, 16, 16, 15, 16, 15, 15, 15},
		{16, 16, 16, 16, 16, 16, 16, 15, 15}
};