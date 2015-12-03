#include "Gamma/../examples/AudioApp.h"

#include "TestSignals.hpp"

class MyApp : public gam::AudioApp{
public:

    declareTestSig();
    MyApp(){

        setupTestSig();
    }

    void onAudio(gam::AudioIOData& io){
        while(io()){

            float s = test_burst();

            io.out(0) = io.out(1) = s;
        }
    }
};

int main(){
    MyApp().start();
}
