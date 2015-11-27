#ifndef SPAT_DELAY_STAGE
#define SPAT_DELAY_STAGE

#include "Gamma/Delay.h"
#include "allocore/math/al_Random.hpp"
#include "allocore/math/al_Vec.hpp"
#include "allocore/sound/al_AudioScene.hpp"

#include "Utils.hpp"

using namespace gam;

/*
 *
 * To add:
 *  X Randomization algorithm for the position of each delay line.
 *  - Gate for re-computing the random position.
 *  - Parameterizing the bounds!
 *  - A better way to do FX chains?
 *  - Adding a probability field.
 */
using namespace std;
class DelayStage //: public al::SoundSource
{
public:
	Delay<float, ipl::Trunc> delay_line;
        al::Vec3f position;
        bool lock_distance = true;
        float distance = 5.0;   // Allosphere radius
        bool lock_position = false;

        DelayStage(float delay_seconds, float max_delay)
            : position(0.0, 0.0, 0.0 ),
              delay_line(delay_seconds, max_delay),
              bounds_upper(5.0, 360.0, 360.0),
              bounds_lower(0, 0, 0),
              bounds_range(bounds_upper - bounds_lower),
              rand_gen()
	{
	}

	virtual float FX(float in) 
	{ 
		float out;

		out = in;

		return out;
	}

        void lockDistance(float distance)
        {
            // TODO: Parameterize this better.
            lock_distance = true;
            distance = distance;
        }

        void lockPosition(al::Vec3f position)
        {
            lock_position = true;
            position = distance;
        }

        al::Vec3f pos()
        {
            return position;
        }

        void setBounds(al::Vec3f upper, al::Vec3f lower)
        {
            bounds_lower = lower;
            bounds_upper = upper;
            bounds_range = bounds_upper - bounds_lower;
        }

	float operator()(float input_sample)
	{
            float out_sample = FX(delay_line(input_sample));
            if(!lock_position)
            {
                if(true) //TODO:Apply gate here
                   setRandomPosition();
            }
            return out_sample;
	}

private:
        al::rnd::Random<> rand_gen;

        al::Vec3f bounds_upper;
        al::Vec3f bounds_lower;
        al::Vec3f bounds_range;

        void setRandomPosition()
        {
            float r;
            if (lock_distance)
                r = distance;
            else
                r = bounds_lower.x + rand_gen.uniform() *bounds_range.x;

            float az = bounds_lower.y + rand_gen.uniform() *bounds_range.y;
            float el = bounds_lower.z + rand_gen.uniform() *bounds_range.z;

            position = toCartesian( al::Vec3f (r, az, el) );
        }
};




#endif
