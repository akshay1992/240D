#ifndef  SPAT_DELAY_UTILS
#define SPAT_DELAY_UTILS

#include <math.h>

#include <iostream>
using namespace std;

//! returns [Dist, azimuth, elevation]
static al::Vec3f toSpherical(al::Vec3f in)
{
    al::Vec3f out(0,0,0);

    out.x = in.mag();
    out.y = atan2( in.y, in.x );
    out.z = atan2( in.z, sqrt(in.x*in.x+in.y*in.y) );


    out.y *= 180 / M_PI;
    out.z *= 180 / M_PI;

    return out;
}

//! returns [Dist, azimuth, elevation]
static al::Vec3f toSpherical(float in_x, float in_y, float in_z)
{
    al::Vec3f out(0,0,0);

    out.x = sqrt(in_x*in_x + in_y*in_y + in_z*in_z);
    out.y = atan2( in_y, in_x );
    out.z = atan2( in_z, sqrt(in_x*in_x+in_y*in_y));

    out.y *= 180 / M_PI;
    out.z *= 180 / M_PI;

    return out;
}


//! returns [Dist, azimuth, elevation]
static al::Vec3f toCartesian(al::Vec3f in)
{
    al::Vec3f out(0,0,0);

    out.x = in.x * cos(in.y * M_PI / 180) * cos(in.z * M_PI / 180);
    out.y = in.x * sin(in.y * M_PI / 180) * cos(in.z * M_PI / 180);
    out.z = in.x * sin(in.z * M_PI / 180);

    return out;
}

static al::Vec3f toCartesian(float in_x, float in_y, float in_z)
{
    al::Vec3f out(0,0,0);

    out.x = in_x * cos(in_y * M_PI / 180) * cos(in_z * M_PI / 180);
    out.y = in_x * sin(in_y * M_PI / 180) * cos(in_z * M_PI / 180);
    out.z = in_x * sin(in_z * M_PI / 180);

    return out;
}

#endif
