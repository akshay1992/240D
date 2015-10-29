#ifndef LOUDSPEAKERS_HPP
#define LOUDSPEAKERS_HPP

/*
This is a generic Loudspeakers class that can be used for 2D Vbap.

It places speakers on the unit circle by default.
*/

#include "allocore/spatial/al_DistAtten.hpp"
#include "VBAP_Utils.hpp"

class LoudSpeakers {
public:
    LoudSpeakers(): DBA_enabled(true)
    {
    	dba.nearClip(1.0);

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

                // Normalize to unit circle
                float mag = sqrt(gains[speaker_pairs[i].sp1.index]*gains[speaker_pairs[i].sp1.index] + gains[speaker_pairs[i].sp2.index]*gains[speaker_pairs[i].sp2.index]);
                gains[speaker_pairs[i].sp1.index] /= mag;
                gains[speaker_pairs[i].sp2.index] /= mag;

                if(DBA_enabled)
                {
    	            gains[speaker_pairs[i].sp1.index] *= dba.attenuation(source.polar[0]);
	                gains[speaker_pairs[i].sp2.index] *= dba.attenuation(source.polar[0]);
                }

                break;
            }
        };
        return gains;
    }
    
    bool DBA_enabled;					// Distance-based attenuation
	al::DistAtten<float> dba;

    std::vector<Speaker> speaker_list; 
    std::vector<SpeakerPair> speaker_pairs;
};

#endif