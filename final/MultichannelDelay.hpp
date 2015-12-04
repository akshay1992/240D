#ifndef MULTICHANNEL_DELAY
#define MULTICHANNEL_DELAY

#include "SpatDelayStage.hpp"
#include <assert.h>

using namespace gam;

class MultichannelDelay
{
public:
    MultichannelDelay(float delay_seconds=1, float feedback=0.5, float max_delay=2) :
            delay_seconds(delay_seconds),
            feedback(feedback),
            max_delay(max_delay),
            focal_point( toCartesian(5.0, 0, 0) )
    {
        configure_delay_lines();
    }

    void set_delay_seconds(float delay_sec, float max_delay_seconds)
    {
    	delay_seconds = delay_sec;
        max_delay = max_delay_seconds;
        assert(max_delay > delay_seconds);
        configure_delay_lines();
    }

    void set_feedback(float fb)
    {
    	feedback = fb;
    }

    int get_nchannels()
    {
        return nstages+1;	// +1 for dry
    }

    float* operator()(float input_sample)
    {
        float *y = (float*) calloc(nstages+1, sizeof(float));

        y[0] = input_sample;	// Dry signal

        for(int i=0; i<nstages; i++)
        {
            y[i+1] = delays[i](y[i]) * feedback;
        }

        return y;
    }

    void configure_delay_lines()
    {
        compute_nstages();
        assert(delay_seconds < max_delay);
        assert (nstages>0);
        for(int i=0; i<nstages; i++)
        {
            DelayStage *d = new DelayStage(delay_seconds, delay_seconds+1);
            delays.push_back(*d);
        }
    }

    void compute_nstages()
    {
        assert(feedback <= 1.0 && feedback >= 0.0);

        std::cout << feedback << " " << decay_thresh << " " << decay_thresh / (20 * log10(feedback)) << std::endl;

    	nstages = floor(decay_thresh / (20 * log10(feedback)));
    	if (nstages > 100)
    		;// TODO: ADD a safety measure
    }

public:
    std::vector<DelayStage> delays;
    al::Vec3f focal_point;

private:
    float nstages;
    float feedback;
    float delay_seconds;
    float max_delay;
    float decay_thresh = -30;
};


#endif
