// P R O B L E M   1
// Generate white noise using gamma

#include <stdio.h>
#include "Gamma/AudioIO.h"
#include "Gamma/Domain.h"
#include "Gamma/Noise.h"

using namespace gam;

int frameCount	= 512;
int samplingRate 	= 44100;
int channels 	= 2;

NoiseWhite<> white_noise(time(NULL));

void audioCallBack(AudioIOData& io)
{

    while(io())
    {   
      for (int i = 0; i < channels; i++)
        {
            io.out(i) = white_noise();
        }
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
