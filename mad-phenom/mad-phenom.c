/*
This file is part of mad-phenom.

mad-phenom is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

mad-phenom is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with mad-phenom.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <stdbool.h>

volatile uint32_t millis = 0;
uint8_t counter = 0;

// This interrupt should occur approx. 3906 times per second
// divide by 4 to get an approx millisecond
ISR(TIM0_COMPA_vect) {
	TCNT0 = 0;
	counter++;
	if (counter == 4) {
		millis++;
		counter = 0;
	}
}

void redOff() {
	PORTA &= ~(1 << PINA1); // RED
}

void greenOff() {
	PORTA &= ~(1 << PINA2); // GREEN
}

void redOn() {
	PORTA |= (1 << PINA1); // RED
}

void greenOn() {
	PORTA |= (1 << PINA2); // RED
}

void solenoidOn() {
	PORTA |= (1 << PINA7);
}

void solenoidOff() {
	PORTA &= ~(1 << PINA7);
}

int main(void) {
	TCCR0B |= (1 << CS01);  // Enable timer with 1/8th prescale
	TIMSK0 |= 1 << OCIE0A; // Configure Timer0 for Compare Match
	OCR0A = 255; // Match at 200
	
	sei();  // Enable global interrupts
	
	// outputs
	DDRA |= (1 << PINA1); // Pin 12 - Red LED
	DDRA |= (1 << PINA2); // Pin 11 - Green LED
	DDRA |= (1 << PINA7); // Pin 6  - Solenoid
	
	// inputs
	DDRB &= ~(1 << PINB1); // Pin 3 - Push button
	DDRB &= ~(1 << PINB2); // Pin 5 - Trigger Pin 1
	DDRA &= ~(1 << PINA6); // Pin 7 - Trigger Pin 2

	// Set LOW
	//PORTA &= ~(1 << PINA4);	// 9 - LOW
	//PORTA &= ~(1 << PINA5);	// 8 - LOW
	
	// Set Triggers HIGH
	PORTB |= (1 << PINB2);
	PORTA |= (1 << PINA6);
	
	// Set Pushbutton HIGH
	PORTB |= (1 << PINB1);

	uint32_t lastUpdated = millis;
	int mode = 0;

	// If the button is held during startup, enter config mode.
	while (true) {
		if (mode == 0 && (millis - lastUpdated) >= 1000) {
			redOff();
			greenOn();
			solenoidOn();
			lastUpdated = millis;
			mode = 1;
		}

		if (mode == 1 && (millis - lastUpdated) >= 20) {
			greenOff();
			redOn();
			solenoidOff();
			lastUpdated = millis;
			mode = 0;
		}
	}	
}

