// P R O B L E M   2
// Use the Pan class (Two channel panner) in Gamma to pan the white noise from the previous problem from left to right at 1Hz.

#include <stdio.h>
#include "Gamma/AudioIO.h"
#include "Gamma/Domain.h"

#include "Gamma/Noise.h"
#include "Gamma/Oscillator.h"
#include "Gamma/Effects.h"

using namespace gam;
using namespace std;

int frameCount	= 512;
int samplingRate 	= 44100;
int channels 	= 2;

NoiseWhite<> white_noise(time(NULL));
LFO<> lfo(1);   // 1Hz triangle wave

Pan<> panner;

void audioCallBack(AudioIOData& io)
{

    while(io())
    {
        float sample = white_noise();
        float out[2];

        panner.pos(lfo.tri());
        panner(sample, io.out(0), io.out(1));
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
