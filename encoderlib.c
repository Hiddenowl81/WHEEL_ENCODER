/*
Copyright (c) 2015, Jasper Dupuis and Nicholas Campbell
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and
the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
and the following disclaimer in the documentation and/or other materials provided
with the
distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to
endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGH
T HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS B
E LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEO
RY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "encoderlib.h"
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

uint8_t left_encoder_count(void)
{
	return(lefttick);
}

uint8_t right_encoder_count(void)
{
	return(righttick);
}

void reset_encoders(void) // This is the requirement for the Open Source project. A simple modification can enable modification of only one of the counters.
{
	lefttick = 0;
	righttick = 0;
	return;
}

void encoder_setup(void)
{
	cli();									//disables interrupts.
	reset_encoders();
	DDRD &= ~((1<<PD2)|(1<<PD3));			// Sets PD2 and PD3 to input and leaves the rest of PORTD/PIND alone. May not be necessary.
	EIMSK |= (1<<INT0)|(1<<INT1);			// Enables INT0 and INT1 but leaves the rest of this register unchanged.
	uint8_t temporary=EICRA;				// Necessary as ISC are two bit for each of INT0, INT1, INT2 and do not wish to overwrite INT2.
	temporary &= 0b11110000;				// leaves bits 7-4 unchanged, bits 3-0 all set to 0 for setting up INT0 and INT1.
	EICRA |= (1<<ISC00)|(1<<ISC10);			// Sets INT0 and INT 1 to trigger on either a rising or falling edge. For N holes/notches in the encoder wheel, there will be 2N ticks on the counter variables.
	EICRA |= temporary|EICRA;				// This preserves the original configuration of EICRA.
	sei();									// enables interrupts.
}

ISR(INT0_vect)								// PD2, left wheel.
{
	lefttick++;
	
	//printf_P(PSTR("The left count is %d\n"),lefttick); // For testing with PuTTy.
	EIFR |= (1<<INTF0);						// Clears this interrupt.
}

ISR(INT1_vect)								// PD3, Right wheel.
{
	righttick++;
	
	//printf_P(PSTR("The right count is %d\n"),righttick); // For testing with PuTTy.
	EIFR |= (1<<INTF1);						// Clears this interrupt.
}

ISR(__vector_default)
{
											// If an unexpected interrupt occurs this will toggle.
}
