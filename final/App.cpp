#include "allocore/al_Allocore.hpp"
#include "Gamma/SoundFile.h"

#include "allocore/sound/al_AudioScene.hpp"
#include "allocore/sound/al_Vbap.hpp"
#include "allocore/sound/al_Speaker.hpp"
#include "alloutil/al_AlloSphereSpeakerLayout.hpp"

#define BLOCK_SIZE 128
#define SAMPLE_RATE 44100

#include "TestSignals.hpp"
#include "SpatialEcho.hpp"

gam::SoundFile sf("out.wav");

using namespace std;

class MyApp : public al::App, public PositionField
{
public:
    SpatialEcho* echo;
    al::SpeakerLayout speakerLayout = al::HeadsetSpeakerLayout();
    al::StereoPanner* panner = new al::StereoPanner(speakerLayout);
    al::Listener* listener;

    al::AudioScene* scene = new al::AudioScene(BLOCK_SIZE);

    declareTestSig();
    MyApp()
    {
        listener = scene->createListener(panner);
        panner->compile(*listener);


        echo = new SpatialEcho(0.04, 0.3);

        sf.format(gam::SoundFile::WAV);
        sf.channels(echo->get_nchannels());
        sf.encoding(gam::SoundFile::PCM_16);
        sf.frameRate(44100);
        sf.openWrite();

        gam::Domain::master().spu(44100);

        setupTestSig(1);
        initAudio(SAMPLE_RATE, BLOCK_SIZE, 2, 0);

        al::Vec3f focus(toCartesian(1, 60, 0));
        setBounds(focus, 0.01);

        std::vector<al::SoundSource*> sources = echo->get_sources();
        for(int i=0; i<echo->get_nchannels(); i++)
            scene->addSource(*sources[i]);

        listener->compile();


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
            scene->render(io);
        }
    }
};

int main(){
    MyApp().start();
    sf.close();
}

