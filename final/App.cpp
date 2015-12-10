#include "allocore/al_Allocore.hpp"
#include "Gamma/SoundFile.h"

#include "TestSignals.hpp"
#include "SpatialEcho.hpp"

gam::SoundFile sf("out.wav");

using namespace std;

class MyApp : public al::App, public PositionField
{
public:
    SpatialEcho* echo;

    declareTestSig();
    MyApp()
    {
        echo = new SpatialEcho(0.04, 0.3);

        sf.format(gam::SoundFile::WAV);
        sf.channels(echo->get_nchannels());
        sf.encoding(gam::SoundFile::PCM_16);
        sf.frameRate(44100);
        sf.openWrite();

        gam::Domain::master().spu(44100);

        setupTestSig(1);
        initAudio(44100, 128, 2, 0);

        al::Vec3f focus(toCartesian(1, 60, 0));
        setBounds(focus, 0.01);

    }

    void onSound(al::AudioIOData& io) override{

        while(io()){

            float s = test_sind();

            float * y = echo->tick(s);

            float echoes = 0;
            for(int i=1; i<echo->get_nchannels(); i++)
            {
                echoes += y[i];
            }

            float out[2] ={s, echoes } ;
            io.out(0) = out[0];
            io.out(1) = out[1];

            sf.write(y, 1);
        }
    }
};

int main(){
    MyApp().start();
}

