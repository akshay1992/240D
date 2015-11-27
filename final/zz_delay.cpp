#include "Gamma/../examples/AudioApp.h"
#include "allocore/sound/al_Reverb.hpp"
#include "Gamma/SoundFile.h"
#include "Gamma/Effects.h"

#include "MultichannelDelay.hpp"
using namespace gam;
using namespace std;

#define loopN(n) for(int i=0; i<n; i++)


SoundFile sf("out.wav");

const double sr = 44100;

class MyApp : public AudioApp{
public:

	Accum<> tmr;
	Burst burst;

    MultichannelDelay spatDelay;

	MyApp()
    :	burst(2e4,2e3, 0.1)
	{
        spatDelay = MultichannelDelay(0.2, 0.5, 0.4);

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

        //DEBUGGING

//        cout << spatDelay.get_nchannels() << " CHANNELS " << spatDelay.delays.size() << " NSTAGES" <<endl;

//        loopN(spatDelay.delays.size())
//            cout << i << ": " << toEuler(spatDelay.delays[i].pos() )<< endl;
//        cout << endl << endl;
	}
};

int main(){

	MyApp().start();

    sf.close();
}
