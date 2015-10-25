#ifndef VBAP_UTILS
#define VBAP_UTILS

#include <vector>

// Helper classes for 2D VBAP

#include <cmath>

class Speaker
{
public:
	Speaker(std::vector<float> cartesian) : cartesian(cartesian) 
	{
		CalculatePolar();
	}


	void CalculatePolar()
	{
			if(polar.size() != 0)
			{
				polar.clear();
			}
			polar.push_back( sqrt( cartesian[0]*cartesian[0] + cartesian[1]*cartesian[1] ) );
			float temp = atan2(cartesian[1], cartesian[0] ) * 180.0 / M_PI;
			polar.push_back( temp < 0.0 ? temp+360 : temp );
	}

	std::vector<float> cartesian;	// Cartesian co-ordinates
	std::vector<float> polar;		// Polar co-ordinates
	float gain;	

	static bool less_than(const Speaker& sp1, const Speaker& sp2) {return sp1.polar[1]<sp2.polar[1];}
	static bool greater_than(const Speaker& sp1, const Speaker& sp2) {return sp1.polar[1]>sp2.polar[1];}
};


class Source : public Speaker
{
public:
	Source(std::vector<float> cartesian) : Speaker(cartesian) {}
	// Identical. This is just for 'syntactic sugar'
};

class SpeakerPair
{
public:
	SpeakerPair(Speaker& sp1, Speaker& sp2) : sp1(sp1), sp2(sp2)
		{
			float temp = (sp1.polar[1]+sp2.polar[1]);
			bisector = (  temp > 360? temp -  360 : temp ) * 0.5;
			temp = fmax( std::abs(sp1.polar[1]-sp2.polar[1]), std::abs(sp1.polar[1]-sp2.polar[1]) );
			half_angular_spread = temp * 0.5;
		}

	bool ComputeGains(std::vector<float> source_position)
	{
		Source source = Source(source_position);
		if( (source.polar[1] - bisector) < half_angular_spread )
			return true;
		else if( source.polar[1] == sp1.polar[1] )
			return true;
		else if (  source.polar[1] == sp2.polar[1] )
			return true;
		else 
			return false;

		// TODO:
	}

	float half_angular_spread; 	// in degrees
	float bisector;			//	in degrees

	Speaker& sp1;
	Speaker& sp2;
};





#endif