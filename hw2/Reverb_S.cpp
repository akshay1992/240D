#include "Gamma/AudioIO.h" 
#include "Gamma/Domain.h" 
#include "Gamma/Oscillator.h" 
#include "Gamma/Envelope.h" 
#include "Gamma/Noise.h"
#include "Gamma/Filter.h"
#include "Gamma/Delay.h"

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
Delay<float> delay24(24.0/1000.0);
Comb<> apf1(22.0 / 1000.0, 0.4, -0.4);
Comb<> apf2(8.3 / 1000.0, 0.6, -0.6);
Delay<> Delay35(35 / 1000.0);

// Tap2
Comb<> apf3(30, 0.4, -0.4);
Delay<> Delay66( 66.0/ 1000.0);

// Feedback
Biquad<> lpf;

// Global variables
float tap1 = 0;
float tap2 = 0;

void roomReverb(float in, float& out, float decayTime) 
{
	float gain = decayTime;	// Gain value to control decay time
	
	// Reverb implementation goes here 
	//
	float input = in + lpf(tap2) * gain;

	float temp = delay24(input);
	tap1 = temp * 0.3 + apf1(apf2( Delay35( temp - 0.3*output) )) ;

	tap2 = tap1 * 0.1 + apf3( Delay66(tap1 - tap2 * 0.1) ); 

	out = (0.5 * tap1) + (0.5 * tap2);
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
	lpf.res(sqrt(2.0)/2.0);

	AudioIO audioIO(frameCount, samplingRate, audioCallBack, NULL, channelsOut, channelsIn);
	Sync::master().spu(audioIO.framesPerSecond()); 
	audioIO.start();
	printf("Press 'enter' to quit...\n"); 
	getchar();
	return 0; 
}