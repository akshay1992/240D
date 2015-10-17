#ifndef NESTED_ALLPASS_FILTER
#define NESTED_ALLPASS_FILTER

// Use this for the outer portion of the nesting, and pass the inner nest as a parameter

class NestedAllPassFilter : public AllPassFilter
{

public:
	NestedAllPassFilter(float d_ms, float k) : AllPassFilter(d_ms, k) {}

	virtual float operator()(float input_sample, AllPassFilter& inner_apf)
	{
		return tick(input_sample, inner_apf);
	}

	virtual float operator()(float input_sample, SeriesAllPassFilter& inner_apf)
	{
		return tick(input_sample, inner_apf);
	}

	virtual float tick(float input_sample, AllPassFilter& inner_apf)
	{
		output_sample = k * input_sample + inner_apf( delay(input_sample - k*output_sample) );

		return output_sample;
	}

	virtual float tick(float input_sample, SeriesAllPassFilter& inner_apf)
	{
		output_sample = k * input_sample + inner_apf( delay(input_sample - k*output_sample) );

		return output_sample;
	}
};

#endif