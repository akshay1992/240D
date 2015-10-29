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

	std::vector<float> cartesian;	// Cartesian co-ordinates
	std::vector<float> polar;		// Polar co-ordinates
	std::vector<float> norm;		// Normalized cartesian co-ordinates

	float gain;	
	int index;

	static bool less_than(const Speaker& sp1, const Speaker& sp2) {return sp1.polar[1]<sp2.polar[1];}
	static bool greater_than(const Speaker& sp1, const Speaker& sp2) {return sp1.polar[1]>sp2.polar[1];}
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

	float half_angular_spread; 	// in degrees
	float bisector;			//	in degrees

	Speaker& sp1;
	Speaker& sp2;

	float det;	// Precomputed determinant in the matrix inversion
};

#endif