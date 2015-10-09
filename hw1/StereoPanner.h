// P R O B L E M  4
// Write an equal power panning function and use it to pan a sine oscillator tuned to 440 Hz from left to right at 0.5 Hz.

#ifndef STEREO_PANNER
#define STEREO_PANNER

#include "Gamma/AudioIO.h"
#include "math.h"
#include "alloaudio/al_Decorrelation.hpp"

class StereoPanner
{
public:
    StereoPanner(): pan_pos(0), gainL(1), gainR(1) {}

    //! Sets the position of the panned sound. Range [-1, 1] where -1 = Left, +1 = Right. 
    void pos(float position)
    {
        pan_pos = position;
    }

    //! Pans a mono sample to two channels within io.
    void pan(float input[2], gam::AudioIOData &io)
    {
        float out[2] = {0, 0};

        pan(input, out);

        io.out(0) = out[0];
        io.out(1) = out[1];
    }

    void pan(float input[2], float output[2])
    {
        calculate_gains();

        output[0] = input[0] * gainL;
        output[1] = input[1] * gainR;
    }

    //! Re-calculates gains based on pan_pos
    void calculate_gains()
    {
        float temp = (pan_pos + 1) * M_PI * 0.25;
        gainR = sin(temp);
        gainL = cos(temp);
    }

private:
    float pan_pos;
    float gainL, gainR;
};

#endif