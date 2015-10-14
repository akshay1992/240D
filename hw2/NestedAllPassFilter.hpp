#ifndef NESTED_ALLPASS_FILTER
#define NESTED_ALLPASS_FILTER

// Use this for the outer portion of the nesting, and pass the inner nest as a parameter

class NestedAllPassFilter : public AllPassFilter
{

public:
	NestedAllPassFilter(float d_ms, float k) : d_ms(d_ms), k(k) {}

	virtual float operator()(float input_sample, AllPassFilter& inner_apf)
	{
		return tick(input_sample, inner_apf) * SCALE;
	}

	virtual float operator()(float input_sample, SeriesAllPassFilter& inner_apf)
	{
		return tick(input_sample, inner_apf);
	}

	virtual float tick(float input_sample, AllPassFilter& inner_apf)
	{
		static gam::Delay<float> delay(d_ms / 1000.0, 0);

		static float output_sample = 0;

		output_sample = k * input_sample + inner_apf( delay(input_sample - k*output_sample) );

		output_sample *= SCALE;

		return output_sample;
	}

	virtual float tick(float input_sample, SeriesAllPassFilter& inner_apf)
	{
		static gam::Delay<float> delay(d_ms / 1000.0, 0);

		static float output_sample = 0;

		output_sample = k * input_sample + inner_apf( delay(input_sample - k*output_sample) );

		output_sample *= SCALE;


		return output_sample;
	}
	
private:
	float d_ms; // Delay in ms
	float k;		// Feedback and feedforward gain

};



#endif