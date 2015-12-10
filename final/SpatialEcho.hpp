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
          timingJitter(timing_jitter),
          focus(toCartesian(al::Vec3f(5.0, 0, 0)))
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

    void set_FocusSpread(al::Vec3f focus_cartesian, float in_spread)
    {
        focus = focus_cartesian;
        spread = in_spread;

        configure_positions();
    }

    void set_FocusSpread(float x, float y, float z, float spread)
    {
        al::Vec3f focus_cartesian(x, y, z);
        set_FocusSpread(focus_cartesian, spread);
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

#ifdef DEBUG_PRINT_POSITION
    void debug_positions_cartesian()
    {
        cout << "Positions: " << endl;
        for (int i=0; i<nchannels ; i++)
        {
            cout << "\t[" << i << "] : " << delay_stages[i]->currentPosition() ;
            if (i==0)
                cout << " ---> (focus)" ;
            cout << endl;
        }
        cout << endl;
    }

    void debug_positions_spherical()
    {
        cout << "Positions: " << endl;
        for (int i=0; i<nchannels ; i++)
        {
            cout << "\t[" << i << "] : " << toSpherical(delay_stages[i]->currentPosition()) ;
            if (i==0)
                cout << " ---> (focus)" ;
            cout << endl;
        }
        cout << endl;
    }

#endif

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

    void debug_print()
    {

        cout << "SpatialEcho" << endl << "-------------------" << endl;
        cout << "Configured number of channels: " << nchannels << endl;
        cout << "Timing Jitter: " << timingJitter << endl;
        cout << "Feedback: " << feedback << endl;
        cout << "Delay "
                "times for each channel (s) " << endl;
        for(int i=0; i<nchannels; i++)
        {
            cout << "\t[" << i << "] : " << delay_stages[i]->delayLine->delay();
            if(i==0)
                cout << " --> (Dry signal pass-through)";

            cout << endl;
        }

    }

    void configure_positions()
    {
        //Lock position of dry signal.
        delay_stages[0]->setBounds(focus, 0.0, true);

        for(int i=1; i<nchannels; i++)
        {
            delay_stages[i]->setBounds(focus, spread);
        }

    }

    void configure_delay_lines()
    {
        compute_nchannels();

        // First channel is a dry pass-through.
        delay_stages.push_back(new DelayStage(delay_length));
        delay_stages[0]->bypass = true;

        // Configure the rest of the channels normally
        for(int i=1; i<nchannels; i++)
        {
            delay_stages.push_back(new DelayStage(delay_length + al::rnd::uniformS() * timingJitter * delay_length));
        }

        configure_positions();

        debug_print();
#ifdef DEBUG_PRINT_POSITION
        debug_positions_cartesian();
#endif
    }

    int nchannels=0; // First channel is always a dry pass-through. There are nchannels-1 delay lines
    const int max_nchannels = 20;
    const float decayThresh_dB = -40;   // An RT60-like measure that is used to determine nchannels needed.

    float *current_frame;

    bool configured_delay_lines = false;
    std::vector<DelayStage*> delay_stages;
    al::Vec3f focus;
    float spread;
    float delay_length;
    float feedback;
    float timingJitter;
};

#endif
