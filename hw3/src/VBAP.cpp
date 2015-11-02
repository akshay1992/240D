#include <stdio.h>
#include <fstream>
#include <vector>
#include "Gamma/AudioIO.h"
#include "Gamma/Domain.h"
#include "Gamma/Oscillator.h"

using namespace std;
using namespace gam;

#include "allocore/spatial/al_DistAtten.hpp"
#include <cmath>

#define foreach(v) for(int i=0; i<v.size(); i++) 
#define foreachadjacent(v) for(int i=0, j = 1; i<v.size(); i++, j = (i+1)%v.size()) 


int frameCount      = 512;
int samplingRate 	  = 48000;
int channels        = 2;

// UGens
Domain domain(256.0);
Sine<> sineMod(4.0/256.0, 0.0);
Sine<> sine(1.0/256.0, 0.0);
Sine<> cosine(1.0/256.0,0.25);
Accum<> tmr(samplingRate /256.0);

// Variables
float2 position(0.0, 0.0);
float modulation = 0.0;
std::ofstream trajectory;
std::ofstream gains;

// Vector of gains
std::vector<float> gainOut(8,0.0);    

// Vector of speaker coordinates
std::vector<std::vector<float> > speakerCoordinates(8, std::vector<float>(2,0.0)); 

// ------------------------------------------------------------
// Begin extra stuff
// ------------------------------------------------------------

class Speaker
{
public:

    Speaker(std::vector<float> cartesian) : cartesian(cartesian), actual_index(-1)
    {
        CalculatePolar();
        CalculateNormalized();
    }

    Speaker(std::vector<float> cartesian, float actual_index) : cartesian(cartesian), actual_index(actual_index)
    {
        CalculatePolar();
        CalculateNormalized();
    }

    void CalculatePolar()
    {
            if(polar.size() != 0)
            {
                polar.clear();
            }
            polar.push_back( sqrt( cartesian[0]*cartesian[0] + cartesian[1]*cartesian[1] ) );
            float temp = atan2(cartesian[1], cartesian[0] ) * 180.0 / M_PI;
            polar.push_back( temp < 0.0 ? temp+360.0 : temp );
    }

    void CalculateNormalized()
    {
        if(norm.size() != 0)
        {
            norm.clear();
        }
        std::vector<float> output;
        output.push_back(cartesian[0]);
        output.push_back(cartesian[1]);

        output[0] /= polar[0];
        output[1] /= polar[0];
        norm = output;
    }

    std::vector<float> cartesian;   // Cartesian co-ordinates
    std::vector<float> polar;       // Polar co-ordinates
    std::vector<float> norm;        // Normalized cartesian co-ordinates

    float gain; 
    int sorted_index;
    
    static bool less_than(const Speaker& sp1, const Speaker& sp2) {return sp1.polar[1]<sp2.polar[1];}
    static bool greater_than(const Speaker& sp1, const Speaker& sp2) {return sp1.polar[1]>sp2.polar[1];}

    int actual_index;

};


class Source : public Speaker
{
public:
    Source(std::vector<float> cartesian) : Speaker(cartesian) {}
    // Identical to speaker. This is just for 'syntactic sugar'
};

class SpeakerPair
{
public:
    SpeakerPair(Speaker& sp1, Speaker& sp2) : sp1(sp1), sp2(sp2)
        {
            bisector = atan2( sp1.norm[1] + sp2.norm[1], sp1.norm[0] + sp2.norm[0] ) * 180 / M_PI; 
            bisector = (bisector < 0.0) ? bisector+360.0 : bisector;

            float temp = acos( (sp1.norm[1]*sp2.norm[1] + sp1.norm[0]*sp2.norm[0]) / (sp1.polar[0] * sp2.polar[0]) ) * 180 / M_PI;
            half_angular_spread = temp * 0.5;

            det = sp1.cartesian[0]*sp2.cartesian[1] - sp1.cartesian[1]*sp2.cartesian[0];
        }

    bool isActive(Source source)
    {
        bool active = false;

        if( std::abs(source.polar[1] - bisector) < half_angular_spread )
            active = true;
        else if( source.polar[1] == sp1.polar[1] )
            active = true;
        else if (  source.polar[1] == sp2.polar[1] )
            active = true;
        else 
            {
                active = false;
            }
        return active;
    }

    float half_angular_spread;  // in degrees
    float bisector;         //  in degrees

    Speaker& sp1;
    Speaker& sp2;

    float det;  // Precomputed determinant in the matrix inversion
};

// ------------------------------------------------------------
// -----> End extra stuff
// ------------------------------------------------------------

// Loudspeaker Class 
class loudSpeakers {
public:
    loudSpeakers(): DBA_enabled(true)
    {
        dba.nearClip(1.0);
    }
    
    void computeMatricies(std::vector< std::vector<float> > speaker_coordinates)
    {
        // Make speakers
        foreach(speaker_coordinates)
        {
            speaker_list.push_back(Speaker(speaker_coordinates[i], i));
        }

        // Sort them according to increasing azimuth, 
        std::sort(speaker_list.begin(), speaker_list.end(), Speaker::less_than);
        foreach(speaker_list) speaker_list[i].sorted_index = i; // Book-keeping

        // Make speaker pairs
        foreachadjacent(speaker_list)
        {
            speaker_pairs.push_back( SpeakerPair(speaker_list[i], speaker_list[j]) );
        }
    }

    std::vector<float> getVBAPGains(Source source)
    {
        std::vector<float> gains(8, 0.0);

        foreach(speaker_pairs)
        {
            if (speaker_pairs[i].isActive(source))
            {
                gains[speaker_pairs[i].sp1.actual_index] = ( source.cartesian[0] * speaker_pairs[i].sp2.cartesian[1] ) - ( source.cartesian[1] * speaker_pairs[i].sp2.cartesian[0] );
                gains[speaker_pairs[i].sp1.actual_index] /= speaker_pairs[i].det;
       
                gains[speaker_pairs[i].sp2.actual_index] = (-1 * source.cartesian[0] * speaker_pairs[i].sp1.cartesian[1] ) + ( source.cartesian[1] * speaker_pairs[i].sp1.cartesian[0] );
                gains[speaker_pairs[i].sp2.actual_index] /= speaker_pairs[i].det;

                // Normalize gains of active pair
                float mag = sqrt(gains[speaker_pairs[i].sp1.actual_index]*gains[speaker_pairs[i].sp1.actual_index] + gains[speaker_pairs[i].sp2.actual_index]*gains[speaker_pairs[i].sp2.actual_index]);
                gains[speaker_pairs[i].sp1.actual_index] /= mag;
                gains[speaker_pairs[i].sp2.actual_index] /= mag;

                if(DBA_enabled)
                {
                    gains[speaker_pairs[i].sp1.actual_index] *= dba.attenuation(source.polar[0]);
                    gains[speaker_pairs[i].sp2.actual_index] *= dba.attenuation(source.polar[0]);
                }
                break;
            }
        };
        return gains;
    }
    
    bool DBA_enabled;                   // Distance-based attenuation
    al::DistAtten<float> dba;

    std::vector<Speaker> speaker_list; 
    std::vector<SpeakerPair> speaker_pairs;
};

// Create loudspeaker object
loudSpeakers loudSpeakersObject;

// VBAP Function
std::vector<float> VBAP(float2 sourcePosition, loudSpeakers loudSpeakersObject)
{
    std::vector<float> out(8,0.0);
    
    // YOUR VBAP CODE GOES HERE
    //

    vector<float> src_pos;                     // This may be redundant.
    src_pos.push_back(sourcePosition[0]);
    src_pos.push_back(sourcePosition[1]);

    Source source(src_pos);                 // Automatically computes the polar

    out = loudSpeakersObject.getVBAPGains(source);

    return out;
}

void audioCallBack(AudioIOData& io)
{
    while(io())
    {
        
        // Compute position
        modulation = sineMod() + 3.0;
        position[0] = modulation * cosine();
        position[1] = modulation * sine();
        
        // Compute gains
        gainOut = VBAP(position, loudSpeakersObject);
        
        // Write to files
        if(tmr())
        {
            trajectory << position[0] << ", " << position[1] <<"\n";
            
            for (int i = 0; i < 8; i++)
            {
                gains << gainOut.at(i);
                
                if (i == 7) { gains << "\n"; }
                else { gains << ", "; }
            }
            tmr.reset();
        }
        
        // Update Buffers
        for (int i = 0; i < channels; i++)
        {
            io.out(i) = 0.0;        // WRITE SILENCE!
        }
    }
}

int main ()
{
    // Assign trajectory generators to new domain
    domain << sineMod << sine << cosine;
    
    // Speaker Positions
    speakerCoordinates[0][0] = -1.0000;
    speakerCoordinates[0][1] = 0.0000;

    speakerCoordinates[1][0] = -0.7071;
    speakerCoordinates[1][1] = 0.7071;

    speakerCoordinates[2][0] = 0.0000;
    speakerCoordinates[2][1] = 1.0000;
    
    speakerCoordinates[3][0] = 0.7071;
    speakerCoordinates[3][1] = 0.7071;

    speakerCoordinates[4][0] = 1.0000;
    speakerCoordinates[4][1] = 0.0000;
    
    speakerCoordinates[5][0] = 0.7071;
    speakerCoordinates[5][1] = -0.7071;

    speakerCoordinates[6][0] = 0.0000;
    speakerCoordinates[6][1] = -1.0000;
    
    speakerCoordinates[7][0] = -0.7071;
    speakerCoordinates[7][1] = -0.7071;
    
    // LoudSpeaker Computations
    loudSpeakersObject.computeMatricies(speakerCoordinates);
    
    // Create trajectory and gains files
    trajectory.open ("trajectory.csv");
    gains.open ("gains.csv");
    
    // Check if files are open
    if (!gains.is_open())
    {
        printf("Can't open gains.csv...\n");
        return 1;
    }
    
    if (!trajectory.is_open())
    {
        printf("Can't open trajectory.csv...\n");
        return 1;
    }

    // Insert column I=information
    trajectory << "X, Y\n";
    gains << "S0, S1, S2, S3, S4, S5, S6, S7\n";
    
    AudioIO audioIO(frameCount, samplingRate, audioCallBack, NULL, channels);
    Sync::master().spu(audioIO.framesPerSecond());
    audioIO.start();
    printf("Press 'enter' to quit...\n");
    getchar();
    
    // Close trajectory and gains files
    gains.close();
    trajectory.close();
    
    return 0;
}