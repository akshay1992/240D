#include "allocore/al_Allocore.hpp"
#include "TestSignals.hpp"

#include "allocore/sound/al_AudioScene.hpp"
#include "allocore/sound/al_StereoPanner.hpp"
#include "allocore/sound/al_Speaker.hpp"

#include "MultichannelDelay.hpp"

#define BLOCK_SIZE 128
#define SR 44100

class MyApp : public al::App{
public:
    al::SpeakerLayout speakerLayout = al::HeadsetSpeakerLayout();
    al::StereoPanner* panner = new al::StereoPanner(speakerLayout);

    al::AudioScene* scene = new al::AudioScene(BLOCK_SIZE);
    al::SoundSource source;
    al::Listener* listener;

    MultichannelDelay spatialDelay;

    float pos = 0;

    declareTestSig();
    float t = 0;

    MyApp()
    {

        listener = scene->createListener(panner);
        panner->compile(*listener);

        scene->addSource(source);
        listener->compile();

        gam::Domain::master().spu(SR);

        spatialDelay.set_delay_seconds(0.2, 0.5);

        setupTestSig(0.5);

        initAudio(SR, BLOCK_SIZE, 2, 0);
    }

    void onSound(al::AudioIOData& io) override {
        while(io()){

            pos = sin(t/1000);
            source.pos(pos, 0, 0);

            float s = test_sind();

            float *y = spatialDelay(s);

            float out = 0;

            for(int i=0; i<spatialDelay.get_nchannels(); i++)
            {
                out += y[i];
            }

            source.writeSample(out);
            scene->render(io);
        }
    }
};

int main(){
    MyApp().start();
}
