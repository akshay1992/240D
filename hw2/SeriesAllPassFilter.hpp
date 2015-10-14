#ifndef SERIES_ALL_PASS_FILTER
#define SERIES_ALL_PASS_FILTERs

// Use this to pass a series connected set of all pass filters to the inner part of a nested structure. 

class SeriesAllPassFilter : public AllPassFilter
{
public:
	SeriesAllPassFilter(AllPassFilter& apf1, AllPassFilter& apf2) : apf1(apf1), apf2(apf2) {}

	virtual float tick(float input_sample)
	{
		return apf1(apf2(input_sample));
	}

private:
	AllPassFilter& apf1;
	AllPassFilter& apf2;
};

#endif