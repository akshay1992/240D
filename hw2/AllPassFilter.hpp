#ifndef ALLPASS_FILTER
#define ALLPASS_FILTER

#include "Gamma/Containers.h"
#include "Gamma/Delay.h"

#define SCALE 0.5

// All pass filter implementation
// NOTE: Assumes a 44100 sample rate

class AllPassFilter
{
public:

	AllPassFilter(float d_ms, float k) : d_ms(d_ms), k(k) {}

	AllPassFilter() {}			// Use ONLY for composite filters

	virtual float operator()(float input_sample)
	{
		return tick(input_sample);
	}

	virtual float tick(float input_sample)
	{
		static gam::Delay<float> delay(d_ms / 1000.0, 0);

		static float output_sample = 0;

		output_sample = k * input_sample + delay(input_sample - k*output_sample);

		output_sample *= SCALE;

		return output_sample;
	}
	
private:
	float d_ms; // Delay in ms
	float k;		// Feedback and feedforward gain
};

#include "SeriesAllPassFilter.hpp"
#include "NestedAllPassFilter.hpp"

#endif