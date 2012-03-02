/**************************************************
	Modify SID instrument settings (settings.c)
	Author:	Edwin Amsler <edwinguy@gmail.com>
	Date:	March 31, 2010
**************************************************/

#include "settings.h"

// Here's out default crumby song. It sounds awful, trust me.
// It plays in a continuous loop in the background as a preview
// for the current ADSR envelope and other settings.
#define NUM_DEMO_NOTES 10
unsigned int demoNotes[NUM_DEMO_NOTES] = {
		0x0F00,
		0x0F00,
		0x2000,
		0x0F00,
		0x0F00,
		0x2000,
		0x0E00,
		0x0D00,
		0x0C00,
		0x0D00
};

// Names of the different SID voices
char *waveformEnum[4] =
	{
			"Triangle",
			"Sawtooth",
			"Square",
			"Noise"
	};


unsigned int changeValue(unsigned int, unsigned int, DELTA*);
unsigned int changeNumber(unsigned int, unsigned int, DELTA*);
unsigned int changeEnum(char**, unsigned int, unsigned int, DELTA*);

// Keeps track of what buttons were pressed.
void buttonDelta(unsigned int *input, unsigned int *history)
{
	unsigned int delta;

	delta    = *input ^ *history;
	delta   &= *history;			// Ignore release as a change
	*history = *input;

	*input = delta;
}

// Run a state machine testing for inputs, which plays
// a simple song as we go.
void configure()
{
	static CONFIGURE state;
	Controller control;
	unsigned int oldControl	= 0;
	unsigned int changed	= 0;	// If the menu needs updating
	DELTA settingDelta 		= NONE;	// How much to change the current setting

	state = INIT;

	// various configureable options
	static unsigned char volume		= 15;
	static unsigned char brightness	= 3;
	static unsigned char attack		= 0;
	static unsigned char decay		= 9;
	static unsigned char sustain	= 0;
	static unsigned char release	= 0;
	static unsigned char waveform	= 0;

	put(VFD_CLR);
	put(VFD_FF);
	println("Config Mode:");
	print("Use directional pad");

	for(;;)
	{
		playNote(0);
		control = readController();

		// Prevents key repeat when the user holds the button down
		buttonDelta(&control.buttons, &oldControl);

		// Increment decrement setting
		settingDelta = NONE;

		// Handle button presses
		switch (control.buttons)
		{
			case PS_SELECT:	// Escape config mode
				put(VFD_CLR);
				put(VFD_FF);
				return;

			case PS_UP:		// Go to previous config option
				state--;
				changed = 1;
				break;

			case PS_DOWN:	// Go to next config option
				state++;
				changed = 1;
				break;

			case PS_LEFT:
				settingDelta = DECREMENT;
				break;

			case PS_RIGHT:
				settingDelta = INCREMENT;
				break;

			default:		// If no buttons press, go back to
				continue;	// the top of the loop
		}

		state %= WAVEFORM + 1;	// So we don't navigate to nowhere.
								// As confusing as it is, WAVEFORM is just
								// the last configuration option. When adding
								// new options, change this. Should have used
								// a sentry val

		// Draw our menu
		switch (state)
		{
			case VOLUME:
				if (changed)
				{
					put(VFD_CLR);
					put(VFD_FF);
					print("Volume:          <->");
					changed = 0;
				}
				volume = changeNumber(volume, 15, &settingDelta);
				SIDSet(MODE_VOL, volume & 0x0F);
				break;

			case BRIGHTNESS:
				if (changed)
				{
					put(VFD_CLR);
					put(VFD_FF);
					print("Brightness:      <->");
					changed = 0;
				}
				brightness = changeNumber(brightness, 3, &settingDelta);
				put(VFD_ESC);
				put(VFDA_LUM);
				put(brightness * 64);	// The hell? *64? Oh! 255/4=64. The Noritake
										// VFD uses 0 to 255 for brightness.
				break;

			case ATTACK:
				if (changed)
				{
					put(VFD_CLR);
					put(VFD_FF);
					print("Note Attack:     <->");
					changed = 0;
				}
				attack = changeNumber(attack, 15, &settingDelta);
				SIDSet(ATK_DECAY, attack << 4);
				break;

			case DECAY:
				if (changed)
				{
					put(VFD_CLR);
					put(VFD_FF);
					print("Note Decay:      <->");
					changed = 0;
				}
				decay = changeNumber(decay, 15, &settingDelta);
				SIDSet(ATK_DECAY, decay & 0x0F);
				break;

			case SUSTAIN:
				if (changed)
				{
					put(VFD_CLR);
					put(VFD_FF);
					print("Note Sustain:    <->");
					changed = 0;
				}
				sustain = changeNumber(sustain, 15, &settingDelta);
				SIDSet(STN_RLS, sustain << 4);
				break;

			case RELEASE:
				if (changed)
				{
					put(VFD_CLR);
					put(VFD_FF);
					print("Note Release:    <->");
					changed = 0;
				}
				release = changeNumber(release, 15, &settingDelta);
				SIDSet(STN_RLS, release & 0x0F);
				break;

			case WAVEFORM:
				if (changed)
				{
					put(VFD_CLR);
					put(VFD_FF);
					print("Waveform Type:   <->");
					changed = 0;
				}
				waveform = changeEnum(waveformEnum, waveform, 3, &settingDelta);
				//controlRegister &= 0xF0;
				controlRegister = 1 << (4 + waveform);
				SIDSet(CONTROL, controlRegister);
				break;

			default:
				if (changed)
				{
					put(VFD_CLR);
					put(VFD_FF);
					print("Synthoscope         ");
					print("Edwin Amsler    2010");
					changed = 0;
				}
				break;
		}
	}
}

// Plays a single note if it's time
void playNote(unsigned int time)
{
	static int position;
	static unsigned int pause;
	unsigned int frequency;

	// Ran out of time. Was going to use
	// a timer + interrupt to play notes at
	// a constant speed. Really ashamed that 
	// I didn't manage it
	pause++;
	if (pause < 75)
		return;
	else
		pause = 0;

	position %= NUM_DEMO_NOTES;
	frequency = demoNotes[position++];

	SIDSet(FREQ_HIGH, frequency >> 8);
	SIDSet(FREQ_LOW,  frequency  & 0xFF);
	SIDSet(CONTROL,   controlRegister);	// Release note
	SIDSet(CONTROL,   controlRegister | CNT_GATE);	// Gate the note (mute it)
}

unsigned int changeEnum(char **list, unsigned int currvalue, unsigned int maxvalue, DELTA *delta)
{
	unsigned int output = changeValue(currvalue, maxvalue,delta);

	put(VFD_CR);
	print("                   ");
	put(VFD_CR);
	print(list[output]);

	return output;
}

// One day, this'll show a bar. Ran out of time back then
unsigned int changeNumber(unsigned int currvalue, unsigned int maxvalue, DELTA *delta)
{
	unsigned int output = changeValue(currvalue, maxvalue,delta);

	put(VFD_CR);
	printnum(output);
	print("/");
	printnum(maxvalue);
	print("  ");

	return output;
}

unsigned int changeValue(unsigned int currvalue, unsigned int maxvalue, DELTA *delta)
{
	if (*delta == DECREMENT)
	{
		if (currvalue - 1 < maxvalue)	// Make sure we don't underflow
			currvalue--;
	}
	else if (*delta == INCREMENT)
	{
		if (currvalue < maxvalue)
			currvalue++;
	}

	*delta = NONE;

	return currvalue;
}