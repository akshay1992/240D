#include <stdio.h>
#include <fstream>
#include <vector>
#include "Gamma/AudioIO.h"
#include "Gamma/Domain.h"
#include "Gamma/Oscillator.h"

#include "VBAP_Utils.hpp"
#include <algorithm>

#define foreach(v) for(int i=0; i<v.size(); i++) 
#define foreachadjacent(v) for(int i=0, j = 1; i<v.size(); i++, j = (i+1)%v.size()) 

using namespace std;
using namespace gam;

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

// Loudspeaker Class
// YOUR NEED TO COMPLETE THIS CLASS

class loudSpeakers {
public:
    loudSpeakers()
    {
        
    }
    
    void computeMatricies(std::vector< std::vector<float> > speaker_coordinates)
    {
        /* 
        For each speaker, calculate the polar co-ordinates. 
        Find all the angles. 
        Order them in increasing order of angles. 
        Compute the matrices and store everything in function prototypes 
        

        // Alt
        Create new struct of loudspeaker pairs (Bisector and half-spread)
        Order them in the correct order.






         */

        // cout << speaker_coordinates.size() << endl;
        foreach(speaker_coordinates)
        {
            speaker_list.push_back(Speaker(speaker_coordinates[i]));
        }

        std::sort(speaker_list.begin(), speaker_list.end(), Speaker::less_than);

        foreachadjacent(speaker_list)
        {
            speaker_pairs.push_back( SpeakerPair(speaker_list[i], speaker_list[j]) );
        }
    }

    vector<Speaker> speaker_list; 
    vector<SpeakerPair> speaker_pairs;
};


// Create loudspeaker object
loudSpeakers loudSpeakersObject;

// VBAP Function
std::vector<float> VBAP(float2 sourcePosition, loudSpeakers loudSpeakersObject)
{
    std::vector<float> out(8,0.0);
    
    // YOUR VBAP CODE GOES HERE
    //
    foreach(loudSpeakersObject.speaker_pairs)
    {
        std::vector<float> srcPosition;
        srcPosition.push_back(sourcePosition[0]);
        srcPosition.push_back(sourcePosition[1]);
        cout << loudSpeakersObject.speaker_pairs[i].ComputeGains(srcPosition) << endl;
    }
    cout << endl << endl;
    
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