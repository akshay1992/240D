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
Comb<> apf1(8.3 / 1000.0, 0.7, -0.7);
Comb<> apf2(22.0 / 1000.0, 0.5, -0.5);
Delay<> Delay35(35.0 / 1000.0);

// Tap2
Delay<float> delay5(5.0/1000.0);
Delay<float> delay67(67.0/1000.0);

Comb<> apf3(30.0 / 1000.0, 0.5, -0.5);

// Tap3
Delay<float> delay15(15.0/1000.0);
Comb<> apf4(9.8 / 1000.0, 0.6, -0.6);
Delay<> Delay39(39.0 / 1000.0);

// Feedback
Delay<float> delay108(108.0/1000);
Biquad<> lpf;

// Global variables
float tap1;
float tap2;
float tap3;

void roomReverb(float in, float& out, float decayTime) 
{
	float gain = decayTime;	// Gain value to control decay time
	
	// Reverb implementation goes here 
	//
	float input = (in + lpf( delay108(tap3) ) * gain);

	tap1 = input * 0.3 + apf2(apf1( Delay35(input - 0.3* tap1) ));

	tap2 = delay67(apf3(delay5(tap1)));

	float temp = (input+delay15(tap2)*gain);
	tap3 = (temp) * 0.3 + apf4( Delay39((temp) - 0.3 * tap3) );

	out = (tap1*0.5) + (tap2*0.5) + (tap3*0.5); 
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
	lpf.res(sqrt(2.0)/2.0);
	
	AudioIO audioIO(frameCount, samplingRate, audioCallBack, NULL, channelsOut, channelsIn);
	Sync::master().spu(audioIO.framesPerSecond()); 
	audioIO.start();
	printf("Press 'enter' to quit...\n"); 
	getchar();
	return 0; 
}