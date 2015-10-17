#ifndef ALLPASS_FILTER
#define ALLPASS_FILTER

#include "Gamma/Containers.h"
#include "Gamma/Delay.h"

// All pass filter implementation

using namespace gam;

class AllPassFilter
{
public:

	AllPassFilter(float d_ms, float k) : 
		d_ms(d_ms), 
		k(k), 
		delay(d_ms / 1000.0), 		// Initialize the delay line. (This was verified to work)
		output_sample(0) {}

	AllPassFilter() {}

	virtual float operator()(float input_sample)
	{
		return tick(input_sample);
	}

	virtual float tick(float input_sample)
	{
		output_sample = k * input_sample + delay(input_sample - k*output_sample);

		return output_sample;
	}
	
protected:
	float d_ms; 	// Delay in ms
	float k;		// Feedback and feedforward gain
	Delay<float> delay;
	float output_sample;
};

#include "SeriesAllPassFilter.hpp"
#include "NestedAllPassFilter.hpp"

#endif