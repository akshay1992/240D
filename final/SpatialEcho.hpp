#ifndef SPATIAL_ECHO
#define SPATIAL_ECHO

#include <cassert>

#include "DelayStage.hpp"
#include "allocore/math/al_Random.hpp"

class SpatialEcho
{
public:

    SpatialEcho(float delay_length, float feedback, float timing_jitter = 0)
        :
          delay_length(delay_length),
          feedback(feedback),
          timingJitter(timing_jitter)
    {
        configure_delay_lines();
        current_frame = (float*) calloc(nchannels, sizeof(float));
    }

    std::vector<al::SoundSource*> get_sources(void)
    {
        std::vector<al::SoundSource*> srcs;

        for(int i=0; i<nchannels; i++)
        {
            if (al::SoundSource* p = dynamic_cast<al::SoundSource*>(delay_stages[i]))
                srcs.push_back(p);
        }

        return srcs;
    }

    void set_timingJitter(float value)
    {
        timingJitter = value;
        configure_delay_lines();
    }

    int get_nchannels() { return nchannels; }

    float* get_frame() { return current_frame; }

    float * tick(float in)
    {
        current_frame[0] = delay_stages[0]->tick(in);
        for (int i=1; i<nchannels; i++)
        {
            float line = current_frame[i-1] * feedback;
            current_frame[i] = delay_stages[i]->tick(line);
        }
        return current_frame;
    }

private:

    void compute_nchannels()
    {
        assert(feedback < 1.0 && feedback >= 0.0);

//        TODO: Account for gate threshold somehow (automatically removes channels).
//        if (decayThresh_dB < delays[0]->get_gateThresh_dB())
//        {
//            decayThresh_dB = delays[0]->get_gateThresh_dB();
//        }

        nchannels = floor(decayThresh_dB / (20 * log10(feedback)));
        nchannels = nchannels + 1 ; // Account for dry first channel

        if (nchannels > max_nchannels)
                nchannels = max_nchannels;
    }

    void configure_delay_lines()
    {
        compute_nchannels();
        cout << nchannels << endl;

        // First channel is a dry pass-through.
        delay_stages.push_back(new DelayStage(delay_length));
        delay_stages[0]->bypass = true;

        // Configure the rest of the channels normally
        for(int i=1; i<nchannels; i++)
        {
            delay_stages.push_back(new DelayStage(delay_length + al::rnd::uniformS() * timingJitter * delay_length));

            cout << i << ": " << delay_length << "    " << al::rnd::uniformS() * timingJitter * delay_length << endl;

        }
    }

    int nchannels=0; // First channel is always a dry pass-through. There are nchannels-1 delay lines
    const int max_nchannels = 20;
    const float decayThresh_dB = -40;   // An RT60-like measure that is used to determine nchannels needed.

    float *current_frame;

    bool configured_delay_lines = false;
    std::vector<DelayStage*> delay_stages;
    float delay_length;
    float feedback;
    float timingJitter;
};

#endif
