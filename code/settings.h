/**************************************************
	Modify SID instrument header (settings.h)
	Author:	Edwin Amsler <edwinguy@gmail.com>
	Date:	March 31, 2010
**************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include "avr/eeprom.h"

#include "playstation.h"
#include "mos8580.h"
#include "noritake_vfd.h"

#define UPDATE_SCREEN 0x01
#define UPDATE_EEPROM 0x02

// Not all of these configurations are settable
// yet. Need to crack open my C64 Programmer's
// Reference book to figure out the last three
// options down there...
// This enum pulls double duty for EEPROM setting addresses
typedef enum CONFIGURE
{
	INIT,		// No properties
	VOLUME,		// 4  bit value
	BRIGHTNESS,	// 2  bit value
	ATTACK,		// 4  bit value
	DECAY,		// 4  bit value
	SUSTAIN,	// 4  bit value
	RELEASE,	// 4  bit value
	WAVEFORM,	// Enumeration
	PULSEWIDTH,	// 12 bit value
	SYNC,		// yes/no
	RING		// yes/no
} CONFIGURE;

typedef enum DELTA
{
	INCREMENT,
	DECREMENT,
	NONE
} DELTA;

void playNote(unsigned short int);
void configure(void);

unsigned char changeValue(unsigned char, unsigned char, DELTA*);
unsigned char changeNumber(unsigned char, unsigned char, DELTA*);
unsigned char changeEnum(const char**, unsigned char, unsigned char, DELTA*);

void LoadConfig(void);

#endif
