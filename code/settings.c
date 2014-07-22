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
const unsigned short int demoNotes[NUM_DEMO_NOTES] = {
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
const char *waveformEnum[4] =
	{
			"Triangle",
			"Sawtooth",
			"Square",
			"Noise"
	};

// various configurable options.
// NOTE: If we run out of memory, we can use Flash for swapping these
//       values since we only read on boot, and when we run the config
//       menu. We also only write on menu exit.
unsigned char volume		= 15;
unsigned char brightness	= 3;
unsigned char attack		= 0;
unsigned char decay			= 9;
unsigned char sustain		= 0;
unsigned char release		= 0;
unsigned char waveform		= 0;

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
void configure(void)
{
	CONFIGURE state;
	//static uint16 scrollcredits;		// How far to scroll the credits
	//const char* credits = "Edwin Amsler 2010 to 2012   Edwin Amsler 2010 to 2012";

	Controller control;
	unsigned int oldControl	= 0;
	uint8 changed	= 0;	// If we need to redraw the screen or update the eeprom
	DELTA settingDelta 		= NONE;	// How much to change the current setting

	put(VFD_CLR);
	put(VFD_FF);
	println("Config Mode:");
	print("Use directional pad");

	state = INIT;

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

				// Don't save if nothing changed
				if (0 == (changed & UPDATE_EEPROM))
					return;

				print("Saving settings...");

				if (volume != eeprom_read_byte((uint8_t*)VOLUME))
					eeprom_write_byte((uint8_t*)VOLUME, volume);

				if (brightness != eeprom_read_byte((uint8_t*)BRIGHTNESS))
					eeprom_write_byte((uint8_t*)BRIGHTNESS, brightness);

				if (attack != eeprom_read_byte((uint8_t*)ATTACK))
					eeprom_write_byte((uint8_t*)ATTACK, attack);

				if (decay != eeprom_read_byte((uint8_t*)DECAY))
					eeprom_write_byte((uint8_t*)DECAY, decay);

				if (sustain != eeprom_read_byte((uint8_t*)SUSTAIN))
					eeprom_write_byte((uint8_t*)SUSTAIN, sustain);

				if (release != eeprom_read_byte((uint8_t*)RELEASE))
					eeprom_write_byte((uint8_t*)RELEASE, release);

				if (waveform != eeprom_read_byte((uint8_t*)WAVEFORM))
					eeprom_write_byte((uint8_t*)WAVEFORM, waveform);

				// Mark that we saved data. EEPROM inits to 0xFF on programming
				eeprom_write_byte(INIT, 0);

				put(VFD_CLR);
				put(VFD_FF);
				return;

			case PS_UP:		// Go to previous config option
				state--;
				changed |= UPDATE_SCREEN;
				break;

			case PS_DOWN:	// Go to next config option
				state++;
				changed |= UPDATE_SCREEN;
				break;

			case PS_LEFT:
				settingDelta = DECREMENT;
				changed |= UPDATE_EEPROM;
				break;

			case PS_RIGHT:
				settingDelta = INCREMENT;
				changed |= UPDATE_EEPROM;
				break;

			default:		// If no buttons press, go back to
				continue;	// the top of the loop
		}

		// So we don't navigate to nowhere.
		// As confusing as it is, WAVEFORM is just
		// the last configuration option. When adding
		// new options, change this. Should have used
		// a sentry value
		state %= (uint8)WAVEFORM + 1;

		// Draw our menu
		switch (state)
		{
			case VOLUME:
				if (changed)
				{
					put(VFD_CLR);
					put(VFD_FF);
					print("Volume:          <->");
					changed &= ~UPDATE_SCREEN;
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
					changed &= ~UPDATE_SCREEN;
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
					changed &= ~UPDATE_SCREEN;
				}
				
				// There's something critically wrong here (and in other cases below).
				// Since we don't factor in the other half of the byte, setting this nibble
				// is knocking the decay out of the picture. What needs to happen, is it needs
				// to OR the decay nibble before setting the SID's register.
				
				// Since the SID is mostly read-only, it's the only way to do it.
				attack = changeNumber(attack, 15, &settingDelta);
				//SIDSet(ATK_DECAY, attack << 4);
				
				// EX: (Need to verify this works)
				SIDSet(ATK_DECAY, (attack << 4) | (decay & 0x0f));
				
				break;

			case DECAY:
				if (changed)
				{
					put(VFD_CLR);
					put(VFD_FF);
					print("Note Decay:      <->");
					changed &= ~UPDATE_SCREEN;
				}
				decay = changeNumber(decay, 15, &settingDelta);
				SIDSet(ATK_DECAY, (attack << 4) | (decay & 0x0F));
				break;

			case SUSTAIN:
				if (changed)
				{
					put(VFD_CLR);
					put(VFD_FF);
					print("Note Sustain:    <->");
					changed &= ~UPDATE_SCREEN;
				}
				sustain = changeNumber(sustain, 15, &settingDelta);
				SIDSet(STN_RLS, (sustain << 4) | (release & 0x0F));
				break;

			case RELEASE:
				if (changed)
				{
					put(VFD_CLR);
					put(VFD_FF);
					print("Note Release:    <->");
					changed &= ~UPDATE_SCREEN;
				}
				release = changeNumber(release, 15, &settingDelta);
				SIDSet(STN_RLS, (sustain << 4) | (release & 0x0F));
				break;

			case WAVEFORM:
				if (changed)
				{
					put(VFD_CLR);
					put(VFD_FF);
					print("Waveform Type:   <->");
					changed &= ~UPDATE_SCREEN;
				}
				waveform = changeEnum(waveformEnum, waveform, 3, &settingDelta);
				controlRegister &= 0xF0;
				controlRegister |= 1 << (4 + waveform);
				SIDSet(CONTROL, controlRegister);
				break;

			default:
				put(VFD_CLR);
				put(VFD_FF);
				print("    PlaySynth 64    ");

				// I broke the credits :(

				break;
		}

		// TODO: Another switch statement for setting registers (remove code dupe)
		// TODO: Store text headers in an array that's indexed by state (remove yet more code dupe)
	}
}

// Configure hardware based on EEPROM settings
void LoadConfig(void)
{
	// Load SID registers from EEPROM
	// TODO: Show some sort of progress or loading message

	// Load our settings from EEPROM. Since default is 0xFF we need to look for 0x00
	if (!eeprom_read_byte(INIT))
	{
		volume     = eeprom_read_byte((uint8_t*)VOLUME);
		brightness = eeprom_read_byte((uint8_t*)BRIGHTNESS);
		attack     = eeprom_read_byte((uint8_t*)ATTACK);
		decay      = eeprom_read_byte((uint8_t*)DECAY);
		sustain    = eeprom_read_byte((uint8_t*)SUSTAIN);
		release    = eeprom_read_byte((uint8_t*)RELEASE);
		waveform   = eeprom_read_byte((uint8_t*)WAVEFORM);
	}

	// Configure SID registers
	SIDSet(MODE_VOL,        (0 << 4) | (volume  & 0x0f));	// Filter mode not implemented yet
	SIDSet(ATK_DECAY,  (attack << 4) | (decay   & 0x0f));
	SIDSet(STN_RLS,   (sustain << 4) | (release & 0x0f));

	// Configure duty cycle for the pulse waveform to be 50%. Too lazy to do bitwise math
	SIDSet(PW_LOW, 2048);
	SIDSet(PW_LOW, 2048 >> 8);

	// There's more to configure in this register, I just haven't enabled that yet.
	controlRegister = 0x13;
	controlRegister &= 0xF0;	// Clean out stale waveform settings
	SIDSet(CONTROL, controlRegister | (1 << (4 + waveform)));

	// Set VFD brightness
	put(VFD_ESC);
	put(VFDA_LUM);
	put(brightness * 64);
}

// Plays a single note if it's time
void playNote(unsigned short int time)
{
	static int position;
	static unsigned int pause;
	unsigned int frequency;

	// TODO: Use a timer interrupt for this
	pause++;
	if (pause < 200)
		return;
	else
		pause = 0;

	position %= NUM_DEMO_NOTES;
	frequency = demoNotes[position++];

	SIDSet(FREQ_HIGH, frequency >> 8);
	SIDSet(FREQ_LOW,  frequency);
	SIDSet(CONTROL,   controlRegister | CNT_GATE);	// Gate the note (mute it)
	SIDSet(CONTROL,   controlRegister);				// Release note
}

unsigned char changeEnum(const char **list, unsigned char currvalue, unsigned char maxvalue, DELTA *delta)
{
	unsigned char output = changeValue(currvalue, maxvalue,delta);

	put(VFD_CR);
	print("                   ");
	put(VFD_CR);
	print(list[output]);

	return output;
}

// One day, this'll show a bar. Ran out of time back then
unsigned char changeNumber(unsigned char currvalue, unsigned char maxvalue, DELTA *delta)
{
	unsigned char output = changeValue(currvalue, maxvalue,delta);

	put(VFD_CR);
	printnum(output);
	print("/");
	printnum(maxvalue);
	print("  ");

	return output;
}

unsigned char changeValue(unsigned char currvalue, unsigned char maxvalue, DELTA *delta)
{
	if (*delta == DECREMENT)
	{
		currvalue--;
	}
	else if (*delta == INCREMENT)
	{
		currvalue++;
	}

	currvalue %= maxvalue + 1;

	*delta = NONE;

	return currvalue;
}
