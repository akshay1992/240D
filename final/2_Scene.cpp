#include "Gamma/../examples/AudioApp.h"
#include "TestSignals.hpp"

#include "allocore/sound/al_AudioScene.hpp"
#include "allocore/sound/al_StereoPanner.hpp"
#include "allocore/sound/al_Speaker.hpp"

#define BLOCK_SIZE 128

class MyApp : public gam::AudioApp{
public:
    al::SpeakerLayout speakerLayout = al::HeadsetSpeakerLayout();
    al::StereoPanner* panner = new al::StereoPanner(speakerLayout);

    al::AudioScene* scene = new al::AudioScene(BLOCK_SIZE);
    al::SoundSource source;
    al::Listener* listener;

    float pos = 0;

    declareTestSig();

    MyApp()
    {
        listener = scene->createListener(panner);
        panner->compile(*listener);

        scene->addSource(source);
        listener->compile();

        setupTestSig();
    }

    void onAudio(gam::AudioIOData& io){
        while(io()){
            source.pos(pos, 0, 0);

            float s = test_burst();

            io.out(0) = io.out(1) = s;
            source.writeSample(s);
//            scene->render(io);
        }
    }
};

int main(){
    MyApp().start();
}
