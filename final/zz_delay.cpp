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
using namespace gam;

#define loopN(n) for(int i=0; i<n; i++)


#include "BasicDelay.hpp"

SoundFile sf("out.wav");

const double sr = 48000;

class MyApp : public AudioApp{
public:

    std::vector< Delay<float, ipl::Trunc> > delays;
    float feedback = 0.5;
    float length = 0.2;
    float n = 3;

	Accum<> tmr;
	Burst burst;

	MyApp()
    :	burst(2e4,2e3, 0.1)
	{
        sf.format(SoundFile::WAV);
        sf.channels(4);
        sf.encoding(SoundFile::PCM_24);
        sf.frameRate(sr);

        loopN(n)
        {
            Delay<float, ipl::Trunc> *d = new Delay<float, ipl::Trunc>();
            d->maxDelay(0.4);
            d->delay(length);
            delays.push_back(*d);
        }

//        Delay<float, ipl::Trunc> d1;
//        Delay<float, ipl::Trunc> d2;
//        Delay<float, ipl::Trunc> d3;


//        d1.maxDelay(0.4);
//        d1.delay(length);

//        d2.maxDelay(0.4);
//        d2.delay(length);

//        d3.maxDelay(0.4);
//        d3.delay(length);

//        delays.push_back(d1);
//        delays.push_back(d2);
//        delays.push_back(d3);

        tmr.period(2);
		tmr.phaseMax();

        sf.openWrite();
	}

	void onAudio(AudioIOData& io){

		while(io()){
			if(tmr()) burst.reset();

            float s = burst()*0.5;

            float *y = (float*) calloc(n, sizeof(float));

            loopN(n)
            {
                if(i==0)
                {
                    y[i] = s;
                }
                else
                {
                    y[i] = delays[i](y[i-1]) * feedback;
                }
            }

//            float y1 = s;
//            float y2 = delays[0](y1) * feedback;
//            float y3 = delays[1](y2) * feedback;
//            float y4 = delays[2](y3) * feedback;

            sf.write(y, 1);
		}
	}
};

int main(){

	MyApp().start();

    sf.close();
}
