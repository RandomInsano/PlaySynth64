/**************************************************
	Noritake VFD control header (noritake_vfd.h)
	Author:	Edwin Amsler <edwinguy@gmail.com>
	Date:	February 21, 2010

	Made this for my Real-Time systems final
	project.
**************************************************/

#ifndef NOTITAKE_VFD_H
#define NOTITAKE_VFD_H

#define VFD_BS		0x08	// Backspace
#define VFD_HT		0x09	// Horizontal tab
#define VFD_LF		0x0A	// Line feed
#define VFD_FF		0x0C	// Form feed
#define VFD_CR		0x0D	// Carriage return
#define VFD_CLR		0x0E	// Clear display
#define VFD_IWM		0x11	// Incremental write mode
#define VFD_VSM		0x12	// Vertical scroll mode
#define VFD_UCO		0x0D	// Underline cursor on
#define VFD_BCO		0x15	// Block cursor on
#define VFD_CO		0x16	// Cursor off
#define VFD_FI		0x18	// International font
#define VFD_FK		0x18	// Katakana font
#define VFD_ESC		0x1B	// Escape (used for advanced functions)

// Advanced functions (require VFD_ESC to directly preceed it)
#define	VFDA_SUF	0x43	// Send user fond
#define VFDA_PC		0x48	// Position cursor
#define VFDA_SR		0x49	// Software reset
#define VFDA_LUM	0x4C	// Luminance (brightness)
#define VFDA_LUM25	0x01	//    25% Brightness
#define VFDA_LUM50	0x40	//    50% Brightness
#define VFDA_LUM75	0x80	//    75% Brightness
#define VFDA_LUM100	0xFF	//   100% Brightness
#define VFDA_FW		0x53	// Flickerless write
#define VFDA_CBF	0x54	// Cursor blink freqency

// High-level functions
void VFDInit	(unsigned int);

void print		(char*);
void println	(char*);
void printnum	(unsigned int);
void put		(char);
void putnum     (char);

#endif