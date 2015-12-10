#include "Gamma/Delay.h"
#include "Gamma/Envelope.h"
#include "allocore/math/al_Random.hpp"
#include "allocore/math/al_Vec.hpp"

#include "allocore/sound/al_AudioScene.hpp"

class DelayChannel : /*public al::SoundSource,*/ public gam::Comb<>
{
public:

    DelayChannel(float delay_length) :
        gam::Comb<>(delay_length)
    {
        ffd(0.0);
        fbk(0.0);

        std::cout << mDelayLength << " " << mMaxDelay << std::endl;
    }

    float operator()(float in)
    {
        // Delays the signal. Calls the same operator in the base class
        return static_cast<gam::Comb<>>(*this)(in);
    }


    ~DelayChannel() {}

//private:
//    gam::Delay<> delay_line;

};
