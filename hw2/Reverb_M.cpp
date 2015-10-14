#include <stdio.h>
#include "Gamma/AudioIO.h" 
#include "Gamma/Domain.h" 
#include "Gamma/Oscillator.h" 
#include "Gamma/Envelope.h" 
#include "Gamma/Noise.h"
#include "Gamma/Filter.h"

#include "AllPassFilter.hpp"

using namespace gam;

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
AllPassFilter apf1(8.3, 0.7);
AllPassFilter apf2(22, 0.5);
SeriesAllPassFilter apf12_inner(apf1, apf2);
NestedAllPassFilter apf12_outer(35, 0.3);

// Tap2
Delay<float> delay5(5.0/1000.0);
Delay<float> delay67(67.0/1000.0);
AllPassFilter apf3(30, 0.5);

// Tap3
Delay<float> delay15(15.0/1000.0);
AllPassFilter apf4_inner(9.8, 0.6);
NestedAllPassFilter apf4_outer(39, 0.3);

// Feedback
Delay<float> delay108(108.0/1000);
Biquad<> lpf;

void roomReverb(float in, float& out, float decayTime) 
{
	float gain = decayTime;	// Gain value to control decay time
	
	// Reverb implementation goes here 
	//
	static float feedback =0;
	float input = in + feedback * POOPY;

	float tap1 = apf12_outer(in, apf12_inner);

	float tap2 = delay67(apf3(delay5(tap1)));

	float tap3 =  ( input + delay15(tap2)*gain ) * POOPY;
	tap3 = apf4_outer(tap3, apf4_inner);

	out = tap1*0.5 + tap2*0.5 + tap3*0.5; 

	feedback = lpf( delay108(tap3) ) * gain;
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
	lpf.freq(2500);
	
	AudioIO audioIO(frameCount, samplingRate, audioCallBack, NULL, channelsOut, channelsIn);
	Sync::master().spu(audioIO.framesPerSecond()); 
	audioIO.start();
	printf("Press 'enter' to quit...\n"); 
	getchar();
	return 0; 
}