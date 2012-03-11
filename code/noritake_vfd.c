/**************************************************
	Noritake VFD control (noritake_vfd.c)
	Author:	Edwin Amsler <edwinguy@gmail.com>
	Date:	February 21, 2010
**************************************************/

#include <avr/io.h>
#include "noritake_vfd.h"
#include "main.h"

// From ATMEGA16 data sheet
void USART_Init(unsigned short int ubrr)
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

void VFDInit(unsigned short int ubrr)
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

void print(const char* message)
{
	while(*message)
	{
		USART_Transmit(*message++);
	}
}

void println(const char* message)
{
	print(message);
	put(VFD_CR);
	put(VFD_LF);
}

// Print a single digit. Undefined if > 9
void putnum(unsigned char num)
{
	put(num + '0');
}

// Only print up to four digits for now.
void printnuml(uint16 number, uint8 showleadingzeros)
{
	uint8  found = showleadingzeros;	// Have we found a leading digit yet?
	uint16 temp;
	uint16 power;

	power = VFD_MAX_DIGITS;	// Consider this pow(10, x) where x is the numbers we can show
	while (power >= 10)
	{
		// This odd four line block slices out a digit in from the input in
		// order. Each iteration it will take the next number to the right
		temp   = number % power;
		power /= 10;
		temp  /= power;

		if (found)
		{
			putnum(temp);
		}
		else
		{
			if (temp)
			{
				found = 1;
				putnum(temp);
			}
		}
	}

	if (!found)
		putnum(0);
}

inline void printnum(uint16 number)
{
	printnuml(number, 0);
}
