#ifndef LOUDSPEAKERS_HPP
#define LOUDSPEAKERS_HPP

/*
This is a generic Loudspeakers class that can currently be used for 2D Vbap.

It places speakers on the unit circle by default.
*/

#include "VBAP_Utils.hpp"

#define foreach(v) for(int i=0; i<v.size(); i++) 
#define foreachadjacent(v) for(int i=0, j = 1; i<v.size(); i++, j = (i+1)%v.size()) 

class LoudSpeakers {
public:
    LoudSpeakers(): DBA_enabled(true)
    {

    }
    
    void computeMatricies(std::vector< std::vector<float> > speaker_coordinates)
    {
        // Make speakers
        foreach(speaker_coordinates)
        {
            speaker_list.push_back(Speaker(speaker_coordinates[i]));
        }

        // Sort them according to increasing azimuth
        std::sort(speaker_list.begin(), speaker_list.end(), Speaker::less_than);

        foreachadjacent(speaker_list)
        {
            // Store index of the sorted speakers
            speaker_list[i].index = i;  

            // Make speaker pairs
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
                gains[speaker_pairs[i].sp1.index] = ( source.cartesian[0] * speaker_pairs[i].sp2.cartesian[1] ) - ( source.cartesian[1] * speaker_pairs[i].sp2.cartesian[0] );
                gains[speaker_pairs[i].sp1.index] /= speaker_pairs[i].det;
       
                gains[speaker_pairs[i].sp2.index] = (-1 * source.cartesian[0] * speaker_pairs[i].sp1.cartesian[1] ) + ( source.cartesian[1] * speaker_pairs[i].sp1.cartesian[0] );
                gains[speaker_pairs[i].sp2.index] /= speaker_pairs[i].det;
                

                float mag = sqrt(gains[speaker_pairs[i].sp1.index]*gains[speaker_pairs[i].sp1.index] + gains[speaker_pairs[i].sp2.index]*gains[speaker_pairs[i].sp2.index]);
                
                gains[speaker_pairs[i].sp1.index] /= mag;
                gains[speaker_pairs[i].sp2.index] /= mag;

                if(DBA_enabled)
                {
                	float distance = source.polar[0];
                }

                break;
            }
        }

        // DEBUG printing
        // foreach(gains)
        // {
        //     printf("%0.2f\t", gains[i]);
        // }
        // printf("\n\n");

        return gains;
    }

    bool DBA_enabled;					// Distance based attenuation
    std::vector<Speaker> speaker_list; 
    std::vector<SpeakerPair> speaker_pairs;
};

#endif