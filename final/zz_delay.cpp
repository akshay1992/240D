/*	Gamma - Generic processing library
	See COPYRIGHT file for authors and license information
	
Example:	Delay-line
Author:		Lance Putnam, 2012

Description:
This demonstrates the multiple uses of a delay-line.
*/

#include "Gamma/../examples/AudioApp.h"
#include "Gamma/Delay.h"
#include "Gamma/Effects.h"
#include "Gamma/SoundFile.h"

#include "MultichannelDelay.hpp"
using namespace gam;

#define loopN(n) for(int i=0; i<n; i++)


SoundFile sf("out.wav");

const double sr = 48000;

class MyApp : public AudioApp{
public:

	Accum<> tmr;
	Burst burst;

    MultichannelDelay spatDelay;

	MyApp()
    :	burst(2e4,2e3, 0.1)
	{
        spatDelay =  MultichannelDelay(0.2, 0.5, 0.4);


        sf.format(SoundFile::WAV);
        sf.channels(spatDelay.get_nchannels());
        sf.encoding(SoundFile::PCM_24);
        sf.frameRate(sr);

        tmr.period(2);
		tmr.phaseMax();

        sf.openWrite();
	}

	void onAudio(AudioIOData& io){

		while(io()){
			if(tmr()) burst.reset();

            float s = burst()*0.5;

            float* y = spatDelay(s);

            sf.write(y, 1);
		}
	}
};

int main(){

	MyApp().start();

    sf.close();
}
