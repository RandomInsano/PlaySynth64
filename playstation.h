/**************************************************
	Playstation controller header (playstation.h)
	Author:	Edwin Amsler <edwinguy@gmail.com>
	Date:	March 30, 2010

	Made this for my Real-Time systems final
	project.
**************************************************/

#ifndef PLAYSTATION_H
#define PLAYSTATION_H

#define PS_SELECT	0x0001
#define PS_L3		0x0002
#define PS_R3		0x0004
#define PS_START	0x0008

#define PS_UP		0x0010
#define PS_RIGHT	0x0020
#define PS_DOWN		0x0040
#define PS_LEFT		0x0080

#define PS_L2		0x0100
#define PS_R2		0x0200
#define PS_L1		0x0400
#define PS_R1		0x0800

#define PS_TRIANGLE 0x1000
#define PS_CIRCLE	0x2000
#define PS_CROSS	0x4000
#define PS_SQUARE	0x8000

// Going to try and use this structure, but
// may take too much overhead. Will have to see
typedef struct PSController
{
	unsigned int Select		: 1;
	unsigned int L3			: 1;
	unsigned int R3			: 1;
	unsigned int Start		: 1;
	unsigned int Up			: 1;
	unsigned int Right		: 1;
	unsigned int Down		: 1;
	unsigned int Left		: 1;

	unsigned int L2			: 1;
	unsigned int R2			: 1;
	unsigned int L1			: 1;
	unsigned int R1			: 1;
	unsigned int Triangle	: 1;
	unsigned int Circle		: 1;
	unsigned int Cross		: 1;
	unsigned int Square		: 1;

	// Analog sticks (Bytes 6 to 9)
	unsigned char RX;
	unsigned char RY;
	unsigned char LX;
	unsigned char LY;
} PSController;

typedef union Controller
{
	PSController	inputs;
	char			bytes[6];
	unsigned int	buttons;
} Controller;


unsigned char PlayStationComm(unsigned char);
void PlayStationInit();
Controller readController();

#endif
