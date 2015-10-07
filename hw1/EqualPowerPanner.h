// P R O B L E M  4
// Write an equal power panning function and use it to pan a sine oscillator tuned to 440 Hz from left to right at 0.5 Hz.

#ifndef EQUAL_POWER_PANNER
#define EQUAL_POWER_PANNER

#include "Gamma/AudioIO.h"
#include "math.h"

class EqualPowerPanner
{
public:
    EqualPowerPanner(): pan_pos(0), gainL(1), gainR(1) {}

    //! Sets the position of the panned sound. Range [-1, 1] where -1 = Left, +1 = Right. 
    void pos(float position)
    {
        pan_pos = position;
        calculate_gains();
    }

    //! Pans a mono sample to two channels within io.
    void pan(float input, gam::AudioIOData &io)
    {
        io.out(0) = input * gainL;
        io.out(1) = input * gainR;
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