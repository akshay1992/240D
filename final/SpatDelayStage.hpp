#ifndef SPAT_DELAY_STAGE
#define SPAT_DELAY_STAGE

#include "Gamma/Delay.h"

using namespace gam;

class DelayStage 
{
	Delay<float, ipl::Trunc> delay_line;

	DelayStage(float delay_seconds, float max_delay)
	{
		delay_line();
		delay_line.maxDelay(max_delay);
		delay_line.delay(delay_seconds);
	}

	virtual float FX(float in) 
	{ 
		float out;

		out = in;

		return out;
	}

};


#endif