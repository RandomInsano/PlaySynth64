/**************************************************
	Modify SID instrument header (settings.h)
	Author:	Edwin Amsler <edwinguy@gmail.com>
	Date:	March 31, 2010
**************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include "playstation.h"
#include "mos8580.h"
#include "noritake_vfd.h"

// Not all of these configurations are settable
// yet. Need to crack open my C64 Programmer's
// Reference book to figure out the last three
// options down there...
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

void playNote(unsigned int);
void configure();

#endif
