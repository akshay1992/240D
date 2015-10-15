#ifndef ALLPASS_FILTER
#define ALLPASS_FILTER

#include "Gamma/Containers.h"
#include "Gamma/Delay.h"

// All pass filter implementation
// NOTE: Assumes a 44100 sample rate

class AllPassFilter
{
public:

	AllPassFilter(float d_ms, float k) : d_ms(d_ms), k(k) {}

	AllPassFilter() {}

	virtual float operator()(float input_sample)
	{
		return tick(input_sample);
	}

	virtual float tick(float input_sample)
	{
		static 	gam::Delay<> delay(d_ms / 1000.0);

		static float output_sample = 0;

		output_sample = k * input_sample + delay(input_sample - k*output_sample);

		return output_sample;
	}
	
private:
	float d_ms; // Delay in ms
	float k;		// Feedback and feedforward gain
};

#include "SeriesAllPassFilter.hpp"
#include "NestedAllPassFilter.hpp"

#endif