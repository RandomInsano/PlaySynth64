/**************************************************
	Commodore SID chip register defs. (mos8580.h)
	Author:	Edwin Amsler <edwinguy@gmail.com>
	Date:	March 01, 2010

	Man, this sid chip makes me wish we were
	using an AVR had extended memory addressing
	capabilities. Could have mapped these
	registers to memory locations :(

	If there are collisions with other constants,
	I'll be adding some sort of prefix. For now,
	it should be fine. I should have added a
	prefix anyway >_<
**************************************************/

#ifndef MOS8580_H
#define MOS8580_H

// There are three voices, each with indentical register offsets 
#define	FREQ_LOW	0		// Low portion of the inital frequency.
#define	FREQ_HIGH	1		// High portion.
#define	PW_LOW		2		// Low part of pulse width of waveform.
#define	PW_HIGH		3		// High part. First four bits only.
#define CONTROL		4		// See 'Control register mapping' below.
#define ATK_DECAY	5		// 4 bits for attack,  4 for decay time.
#define STN_RLS		6		// 4 bits for sustain, 4 for release.

// Control register mapping: The first four bits
// are for what sort of waveform to create. Only
// one type can be set at a time per voice.
#define CNT_NOISE	0x80	// Noise	waveform
#define CNT_SQUARE	0x40	// Square	waveform
#define CNT_SAW		0x20	// Sawtooth	waveform
#define CNT_TRI		0x10	// Triangle	waveform
#define	CNT_TEST	0x08	// Test mode. Lock voice's oscillator clock
#define CNT_RING	0x04	// Ring modulated output. Result depends on voice
#define CNT_SYNC	0x02	// Lock OSC1 with OSC3. Not sure about voices 2 & 3
#define CNT_GATE	0x01	// Hold a note. 1 = Play, 0 = Hold.

// Start addresses of the voices
#define	VOICE1		0
#define VOICE2		7
#define VOICE3		14

/*
	Quick example of SID addressing:
	//set voice 2 to output noise
	PORTC = VOICE2 + CONTROL;
	PORTA = CNT_NOISE;
 */

// Global SID configuration registers
#define FILTER_LOW	21		// Programmable frequency filter. First three bits
#define FILTER_HIGH	22		// High part.
#define RES_FILT	23		// [B:7-4] Resonance of the filter.	[B:3-0] Filter enable
#define	MODE_VOL	24		// [B:7-4] Filter mode.				[B:3-0] Volume control

// Filter enable flags
#define FILT_EN_EX	0x08	// Filter external input
#define FILT_EN_3	0x04	// Filter voice three
#define	FILT_EN_2	0x02	// Filter voice two
#define FILT_EN_1	0x01	// Filter voice one

// Filter mode control flags
#define FILT_M_LP	0x10	// Low pass filter enable. Think of it like bass-boost.
#define FILT_M_BP	0x20	// High pass filter enable. High treble. 'Thin sound'.
#define FILT_M_HP	0x40	// Higer pass filter. Ultra-high treble. 'Tinny/buzzy sound'.
#define OSC3_BYPASS 0x80	// Disconnect voice 3 from output. Reason explained below.

// Outputs. These registers are read only.
#define ADC1		25		// Value of analog to digital converter 1 (POTX Pin)
#define ADC2		26		// Value of analog to digital converter 2 (POTY Pin)
#define OSC3		27		// Oscillator 3 value. Reads output from voice 3 oscillator
#define ENV3		28		// Voice 3 envelope generator value.

/* OSC3 and ENV3 allow for some neat effects. Depending on how voice 3 is configured, this
 * can work like a PWM counter (sawtooth, triangle waves) or a random number generator
 * (noise). More examples are available from the my 1987 Programmer's Reference Guide */

// Define the AVR data ports we use
#define SID_DATA		PORTA
#define SID_ADDR		PORTD
#define SID_DATA_DIR	DDRA
#define SID_ADDR_DIR	DDRD

// Used for SID control register
// Sadly, most all registers on the SID are write only
unsigned char controlRegister;

void SIDInit();
void SIDSet(unsigned char, unsigned char);

#endif
