// Unit Test for Panner

#include <iostream>
#include "StereoPanner.h"

#include <Gamma/AudioIO.h>
#include "Gamma/Domain.h"

using namespace std;
using namespace gam;
int main()
{
	cout << "Running test: Stereo Panner" << endl;
	StereoPanner panner;

	bool fail = false;
	
	for (float pos=-1; pos<=1; pos+=0.01)
	{
		panner.pos(pos);

		float temp = (pos + 1) * M_PI * 0.25;
        float gainR = sin(temp);
        float gainL = cos(temp);

        float test_signal[2] = {1, 1};
        float out_signal[2];

        panner.pan(test_signal, out_signal);

        if( out_signal[0] != gainL && out_signal[1] != gainR )
        {
        	fail = true;
        }
	}

	if (fail == false)
	{
		cout << "Test Passed" << endl;
	}
	else
	{
    	cout << "Test Failed" << endl;
	}
	return 0;
}