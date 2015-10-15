 #include <stdio.h>
#include "Gamma/AudioIO.h" 
#include "Gamma/Domain.h" 
#include "Gamma/Oscillator.h" 
#include "Gamma/Envelope.h" 
#include "Gamma/Noise.h"
#include "Gamma/Filter.h"
#include "Gamma/Delay.h"

#include "AllPassFilter.hpp"

using namespace gam;
using namespace std;

int frameCount = 512; 
int samplingRate = 44100; 
int channelsIn = 0; 
int channelsOut =2;

float input =0.0; 
float output = 0.0; 
float dryWet = 0.5; // 0.0 for dry signal and 1.0 for wet signal 
float level = 0.1;  // Audio output level

float decayTime = 0.5; // Decay time of the reverb (Play with this)

NoiseWhite<> white; // White Noise
AD<> env;			// Attack/Decay envelope
Accum<> tmr;		// Timer to reset AD envelope

//Declare your delays, lowpass and allpass filters here 
//

// Tap1
Delay<> delay24(24.0/1000.0);
AllPassFilter apf1(22.0, 0.4);
AllPassFilter apf2(8.3, 0.6);
SeriesAllPassFilter apf_inner12(apf1, apf2);
NestedAllPassFilter apf_outer12(35, 0.3);

// Tap2
AllPassFilter apf3_inner(30, 0.4);
NestedAllPassFilter apf4_outer(66, 0.1);

// Feedback
Biquad<> lpf;

void roomReverb(float in, float& out, float decayTime) 
{
	float gain = decayTime;	// Gain value to control decay time
	
	// Reverb implementation goes here 
	//
	static float feedback = 0;

	float input = (in + feedback);

	float tap1 = apf_outer12( delay24(input), apf_inner12);

	float tap2 = apf4_outer(tap1, apf3_inner);

	out = (0.5 * tap1) + (0.0 * tap2);
	
	feedback = lpf(tap2) * gain; 
}

// DO NOT MODIFY THE AUDIO CALLBACK FUNCTION 
void audioCallBack(AudioIOData& io)
{
	while(io()) 
	{
		if(tmr()) env.reset();	// Reset AD envelope
		
		input = white() * env(); // Apply envelope to white noise 
		roomReverb(input, output, decayTime); // Apply reverb

		for (int i = 0; i < channelsOut; i++) 
		{
			io.out(i) = ( (1.0 - dryWet) * input + dryWet * output ) * level; 
		}
	}
}

int main() 
{
	tmr.period(3);	// Reset the envelope every 3 seconds 
	env.attack(0.01);	// Attack time
	env.decay(0.05);	// Decay time


	// Delays, lowpass, and allpass filters setup code goes here 
	//
	lpf.type(LOW_PASS);
	lpf.freq(4200);
	
	AudioIO audioIO(frameCount, samplingRate, audioCallBack, NULL, channelsOut, channelsIn);
	Sync::master().spu(audioIO.framesPerSecond()); 
	audioIO.start();
	printf("Press 'enter' to quit...\n"); 
	getchar();
	return 0; 
}