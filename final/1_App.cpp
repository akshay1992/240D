#include "Gamma/../examples/AudioApp.h"
#include "allocore/al_Allocore.hpp"

#include "TestSignals.hpp"

class MyApp : public al::App{
public:

    declareTestSig();
    MyApp(){

        setupTestSig();
        gam::Domain::master().spu(44100);
        initAudio(44100, 128, 2, 0);
    }

    void onSound(al::AudioIOData& io) override{
        while(io()){

            float s = test_sind();

            io.out(0) = io.out(1) = s;
        }
    }
};

int main(){
    MyApp().start();
}
