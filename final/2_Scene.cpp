#include "allocore/al_Allocore.hpp"
#include "TestSignals.hpp"

#include "allocore/sound/al_AudioScene.hpp"
#include "allocore/sound/al_StereoPanner.hpp"
#include "allocore/sound/al_Speaker.hpp"

#define BLOCK_SIZE 128
#define SR 44100

class MyApp : public al::App{
public:
    al::SpeakerLayout speakerLayout = al::HeadsetSpeakerLayout();
    al::StereoPanner* panner = new al::StereoPanner(speakerLayout);

    al::AudioScene* scene = new al::AudioScene(BLOCK_SIZE);
    al::SoundSource source;
    al::Listener* listener;

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

        setupTestSig();

        initAudio(SR, BLOCK_SIZE, 2, 0);
    }

    void onSound(al::AudioIOData& io) override {
        while(io()){

            pos = sin(t/1000);
            source.pos(pos, 0, 0);

            float s = test_sind();

            source.writeSample(s);
            scene->render(io);
        }
    }
};

int main(){
    MyApp().start();
}
