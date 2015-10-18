#include "Gamma/AudioIO.h" 
#include "Gamma/Domain.h" 
#include "Gamma/Oscillator.h" 
#include "Gamma/Envelope.h" 
#include "Gamma/Noise.h"
#include "Gamma/Filter.h"
#include "Gamma/Delay.h"

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
Comb<> apf1(8.0 / 1000.0, 0.3, -0.3);
Comb<> apf2(12.0 / 1000.0, 0.3, -0.3);
Delay<float> delay4(4.0 / 1000.0);

// Tap2
Delay<float> delay17(17.0 / 1000.0);
Delay<float> delay31(31.0 / 1000.0);
Comb<> apf3(62.0 / 1000.0, 0.25, -0.25);
Delay<> Delay87(87.0 / 1000.0);


// Tap3
Delay<float> delay3(3.0 / 1000.0);
Comb<> apf4(76.0 / 1000.0, 0.25, -0.25);
Comb<> apf5(30.0 / 1000.0, 0.35, -0.35);
Delay<> Delay120(120.0 / 1000.0);

// Feedback
Biquad<> lpf;

// Global Variables
float tap1;
float tap2;
float tap3;

void roomReverb(float in, float& out, float decayTime) 
{
	float gain = decayTime;	// Gain value to control decay time
	
	// Reverb implementation goes here 
	//
	float input = lpf(tap3) * gain + in;

	tap1 = delay4(apf2(apf1(input)));

	// float tap2 = delay31(apf3_outer(delay17(tap1), apf3_inner));
	float temp = delay17(tap1);
	tap2 = delay31( temp * 0.5 + apf3( Delay87( temp - 0.5 * tap2) ) );

	// float tap3 = apf45_outer(delay3(tap2), apf45_inner);
	temp = delay3(tap2);
	tap3 = temp * 0.5 + apf5(apf4( Delay120( temp - 0.5 * tap3) ));

	out = (0.34*tap1) + (0.14*tap2) + (0.14*tap3);
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
	lpf.freq(2600);
	lpf.res(sqrt(2.0)/2.0);

	AudioIO audioIO(frameCount, samplingRate, audioCallBack, NULL, channelsOut, channelsIn);
	Sync::master().spu(audioIO.framesPerSecond()); 
	audioIO.start();
	printf("Press 'enter' to quit...\n"); 
	getchar();
	return 0; 
}