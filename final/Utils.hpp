#ifndef  SPAT_DELAY_UTILS
#define SPAT_DELAY_UTILS

//! returns [Dist, azimuth, elevation]
static al::Vec3f toEuler(al::Vec3f in)
{
    al::Vec3f out(0,0,0);

    out.x = in.mag();
    out.y = atan( (float) in.y/in.x );
    out.z = atan( (float) sqrt(in.x*in.x+in.y*in.y) / in.z);

    return out;
}

//! returns [Dist, azimuth, elevation]
static al::Vec3f toEuler(float in_x, float in_y, float in_z)
{
    al::Vec3f out(0,0,0);

    out.x = sqrt(in_x*in_x + in_y*in_y + in_z*in_z);
    out.y = atan( (float) in_y/in_x );
    out.z = atan( (float) sqrt(in_x*in_x+in_y*in_y) / in_z);

    return out;
}


//! returns [Dist, azimuth, elevation]
static al::Vec3f toCartesian(al::Vec3f in)
{
    al::Vec3f out(0,0,0);

    out.x = in.x * cos(in.y) * cos(in.z);
    out.y = in.x * sin(in.y) * cos(in.z);
    out.z = in.x * sin(in.z);

    return out;
}

static al::Vec3f toCartesian(float in_x, float in_y, float in_z)
{
    al::Vec3f out(0,0,0);

    out.x = in_x * cos(in_y) * cos(in_z);
    out.y = in_x * sin(in_y) * cos(in_z);
    out.z = in_x * sin(in_z);

    return out;
}

#endif
