/**************************************************
	Noritake VFD control (noritake_vfd.c)
	Author:	Edwin Amsler <edwinguy@gmail.com>
	Date:	February 21, 2010

	Made this for my Real-Time systems final
	project.
**************************************************/

#include <avr/io.h>
#include "noritake_vfd.h"

// From ATMEGA16 data sheet
void USART_Init(unsigned int ubrr)
{
	/* Set baud rate */
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)ubrr;
	UCSRB = (1 <<  TXEN);					// VFD has no output
	UCSRC = (1 << URSEL) | (3 << UCSZ0);	// Port config: 8-N-1
}

// From ATMEGA16 data sheet
void USART_Transmit(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while (!(UCSRA & (1<<UDRE)));
	/* Put data into buffer, sends the data */
	UDR = data;
}

void VFDInit(unsigned int ubrr)
{
	USART_Init(ubrr);

	// Reset the VFD
	put(VFD_ESC);
	put(VFDA_SR);
}

void put(char out)
{
	USART_Transmit(out);
}

void print(char* message)
{
	while(*message)
	{
		USART_Transmit(*message++);
	}
}

void println(char* message)
{
	print(message);
	put(VFD_CR);
	put(VFD_LF);
}

// Print a single digit. Undefined if > 9
inline void putnum(char num)
{
	put(num + 48); // 48 is ASCII code for 0
}

void printnum(unsigned int number)
{
	uint32_t	power = 1000000000; // 10 ^ 10
	uint32_t	tempNum;
	char		found = 0;

	int			loop;

	// Wow... What was I smoking when I wrote this?
	for (loop = 10; loop > 2; loop--)
	{
		// Trim off higher numbers
		tempNum	 = number % power;
		power	/= 10;
		// Trim off lower numbers;
		tempNum	/= (power);

		// Don't print leading zeros.
		if (found)
		{
			putnum(tempNum);
		}
		else if (tempNum != 0)
		{
			// This elseif seems odd, but should save some cycles.
			// If the comparison happened before 'if (found)',
			// it would be executed every loop. This way, our
			// test happens only if the first number hasn't been
			// found.

			found = 1;
			putnum(tempNum);
		}
	}

	putnum(number % 10);
}
