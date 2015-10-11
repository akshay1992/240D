#ifndef ALLPASS_FILTER
#define ALLPASS_FILTER

#include "Gamma/Containers.h"

class AllPassFilter
{
public:

	AllPassFilter(int d, float k) : d(d), k(k) {}

	float operator()(float input_sample)
	{
		return tick(input_sample);
	}

	float tick(float input_sample)
	{
		static gam::DelayN<float> D(d, 0);

		static float output_sample = 0;

		// Difference equation implementation
		output_sample = k * input_sample + D(input_sample - k*output_sample);

		return output_sample;
	}
	
private:
	int d; 	// Delay (in samples)
	float k;		// Feedback and feedforward gain

};

#endif