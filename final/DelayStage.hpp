#ifndef DELAY_STAGE
#define DELAY_STAGE

#include "Gamma/Delay.h"
#include "Gamma/Envelope.h"
#include "PositionField.hpp"
#include "allocore/sound/al_AudioScene.hpp"

using namespace gam;

class DelayStage : public PositionField, public al::SoundSource
{
public:
    gam::Gate<> gate;
    gam::Delay<float>* delayLine;

    bool bypass = false;

    DelayStage(float delay_length) :
        PositionField(),
        gate()
    {
        delayLine = new gam::Delay<float>(delay_length);

    }

    float tick(float in)
    {
        if (bypass)
        {
            return in;
        }
        else
        {
            float delayed_val = (*delayLine)(in);

            if (gate.done())
            {
                refreshPosition();
            }

            al::Vec3f p = currentPosition();
            pos(p.x, p.y, p.z);

            float out_sample = delayed_val * gate(delayed_val);
            writeSample(out_sample);

            return (out_sample);
        }
    }
};

#endif
