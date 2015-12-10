#ifndef POSITION_FIELD
#define POSITION_FIELD

#include "Utils.hpp"
#include "allocore/math/al_Vec.hpp"
#include "allocore/math/al_Random.hpp"

using namespace std;

class PositionField
{
public:
        al::rnd::Random<> rng;
        bool position_lock;
	float r, r_lower, r_upper, r_range;
        float az, az_lower, az_upper, az_range;
        float el, el_lower, el_upper, el_range;

	void set_lowerBound(float r, float az, float el)
	{
		r_lower = r;
		az_lower = az;
		el_lower = el;
                recalculate_range();
	}

	void set_upperBound(float r, float az, float el)
	{
		r_upper = r;
		az_upper = az;
		el_upper = el;
                recalculate_range();
	}

	void refreshPosition()
	{
            if (!position_lock)
            {
                r = r_lower + rng.uniform() * r_range;
                az = az_lower + rng.uniform() * az_range;
                el = el_lower + rng.uniform() * el_range;
            }
	}

        void recalculate_range()
        {
            r_range = r_upper-r_lower;
            az_range = az_upper-az_lower;
            el_range = el_upper-el_lower;
        }

	al::Vec3f currentPosition()
        {
            return toCartesian(r, az, el);
	}

        /// Sets the spread of bounds (0 to 1) from the focal point (given in cartesian).
        void setBounds(al::Vec3f focus, float spread, bool lock_position = false)
        {
            position_lock = lock_position;
            al::Vec3f focus_polar = toSpherical(focus);

            cout << "FOCUS POLAR " << focus_polar << endl;

            r = focus_polar.x;
            az = focus_polar.y;
            el = focus_polar.z;

            float az_spread, el_spread;

            az_spread = spread * 180;
            el_spread = spread * 180;

            cout  << "SPREADKS" << az_spread << " " << el_spread << endl;

            set_upperBound( focus_polar.x , focus_polar.y + az_spread, focus_polar.z + el_spread);
            set_lowerBound( focus_polar.x , focus_polar.y - az_spread, focus_polar.z - el_spread);
        }

};


#endif
