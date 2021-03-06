// P R O B L E M  3
// Write a linear panning function and use it to pan a sine oscillator tuned to 440 Hz from left to right at 0.5 Hz.

#include <stdio.h>
#include "Gamma/AudioIO.h"
#include "Gamma/Domain.h"

#include "Gamma/Noise.h"
#include "Gamma/Oscillator.h"

#include "LinearPanner.h"   // See this for implementation details

using namespace gam;
using namespace std;

int frameCount	= 512;
int samplingRate 	= 44100;
int channels 	= 2;

LFO<> lfo(0.5);   // 0.5Hz triangle wave
Sine<> sine(440);   // 440H sine wave

LinearPanner lin_panner;

void audioCallBack(AudioIOData& io)
{

    while(io())
    {
        float sample = sine();

        lin_panner.pos(lfo.tri());
        lin_panner.pan(sample, io);
    }
}

int main()
{
     AudioIO audioIO(frameCount, samplingRate, audioCallBack, NULL, channels);
    Sync::master().spu(audioIO.framesPerSecond());
    audioIO.start();
    printf("Press 'enter' to quit...\n");
    getchar();
    return 0;
}
