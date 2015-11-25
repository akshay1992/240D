#ifndef MULTICHANNEL_DELAY
#define MULTICHANNEL_DELAY

#include "SpatDelayStage.hpp"

using namespace gam;

class MultichannelDelay
{
public:
    MultichannelDelay()
    {
        // TODO: Safety when non initialized and configured delay lines
    }

    MultichannelDelay(float delay_seconds, float feedback) :
            delay_seconds(delay_seconds),
            feedback(feedback)
    {
            compute_nstages();
            configure_delay_lines();
    }

    void set_delay_seconds(float delay_sec)
    {
    	delay_seconds = delay_sec;
		configure_delay_lines();
    }

    void set_feedback(float fb)
    {
    	feedback = fb;
    	compute_nstages();
    }

    int get_nchannels()
    {
        return nstages;
    }

    float* operator()(float input_sample)
    {
        float *y = (float*) calloc(nstages, sizeof(float));

        y[0] = input_sample;	// Dry

        for(int i=1; i<nstages; i++)
        {
            y[i] = delays[i](y[i-1]) * feedback;
        }

        return y;
    }

    void configure_delay_lines()
    {   
        //TODO:
    	// assert (nstages!=0)

        for(int i=0; i<nstages; i++)
        {
            Delay<float, ipl::Trunc> *d = new Delay<float, ipl::Trunc>();
            d->maxDelay(0.4);
            d->delay(delay_seconds);
            delays.push_back(*d);
        }
    }

    void compute_nstages()
    {
    	// TODO:
    	//assert(feedback <= 1.0 && feedback >= 0.0)

    	nstages = floor(decay_thresh / (20 * log10(feedback)));
    	if (nstages > 100)
    		;// TODO: ADD a safety measure
    }



public:
    float nstages;


private:
    std::vector< Delay<float, ipl::Trunc> > delays;
    float feedback;
    float delay_seconds;
    float decay_thresh = -40;

    
};


#endif
