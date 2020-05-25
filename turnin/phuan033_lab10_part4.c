/*	Author: lab
 *  Partner(s) Name: karen
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr ==0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum States_one{start, first_led, second_led, third_led} state_one;
enum States_two{Start, blink, blink2, } state_two;
enum States_three{START, output} state_three;
enum States_four{begin, init, off, on} state_four;

unsigned char threeLEDs = 0x00;
unsigned char led = 0x00;
unsigned short count_one = 0x00;
unsigned short count_two = 0x00;
unsigned char out = 0x00;
unsigned char button = 0x00;
unsigned char speak = 0x00;
void ThreeLEDsSM() //0-1-2 one second
{
	switch(state_one)
	{
		case start:
			count_one = 0x00;
			//threeLEDS = 0x00;
			state_one = first_led;
			break;

		case first_led:
			if(count_one >= 300)
			{
				count_one = 0x00;
				state_one = second_led;
			}
			count_one++;
			break;

		case second_led:
			if(count_one >= 300)
			{
				count_one = 0x00;
				state_one = third_led;
			}
			count_one++;
			break;

		case third_led:
			if(count_one >= 300)
			{
				count_one = 0x00;
				state_one = first_led;
			}
			count_one++;
			break;

		default:
			state_one = start;
			break;
	}

	switch(state_one)
	{
		case start:
			break;

		case first_led:
			threeLEDs = 0x01;
			break;

		case second_led:
			threeLEDs = 0x02;
			break;

		case third_led:
			threeLEDs = 0x04;
			break;

	}
}


void BlinkingLEDSM()
{
	switch(state_two)
	{
		case Start:
			//PORTB = 0x00;
			count_two = 0x00;
			state_two = blink;
			break;

		case blink:
			if(count_two < 1000)
			{
				count_two++;
				state_two = blink;
			}
			else
			{
				count_two = 0x00;
				state_two = blink2;
			}
			//count_two++;
			break;


		case blink2:
			if(count_two >= 1000)
			{
				count_two = 0x00;
				state_two = blink;
			}
			count_two++;
			break;



		default:
			state_two = Start;
			break;
	}

	switch(state_two)
	{
		case Start:
		break;

		case blink:
			led = 0x08;
			break;

		

		case blink2:
			led = 0x00;
			break;

	}

}

unsigned char freq = 0x01;
unsigned char count_three = 0x00;

void Speaker()
{
	button = ~PINA & 0x04;
	switch(state_four)
	{
		case begin:
			state_four = init;
			break;

		case init:
			if(button == 0x04)
			{
				state_four = on;
			}
			else if(button == 0x01)
			{
				if(freq < 9) 
				{
					freq++;
				}
			}
			else if(button == 0x02)
			{
				if(freq > 1)
				{
					freq--;
				}
			}
			else
			{
				state_four = init;
			}
			break;

		case on:
			if(count_three >= freq)
			{
				count_three = 0x00;
				state_four = off;
			}
			count_three++;
			break;

		case off:	
			if(count_three >= freq)
			{
				count_three = 0x00;
				state_four = init;
			}
			count_three++;
			break;

		default:
			state_four = begin;
			break;
	}

	switch(state_four)
	{
		case begin:
			speak = 0x00;
			break;

		case init:
			break;

		case on:
			speak = 0x10;
			break;

		case off:
			speak = 0x00;
			break;

		
	}
}

void CombineLEDsSM()
{
        switch(state_three)
        {
                case START:
                        state_three = output;
                        break;

                case output:
//			Speaker();
                        break;

                default:
                        state_three = START;
                        break;
        }

        switch(state_three)
        {
                case START:
                break;

                case output:
                        PORTB = threeLEDs | led | speak;
                        break;

        }
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
	TimerSet(1);
	TimerOn();
//	button = ~PINA & 0x04;
    /* Insert your solution below */
    while (1) {
	    //	button = ~PINA & 0x04;
	    	ThreeLEDsSM();
		BlinkingLEDSM();
		Speaker();
		CombineLEDsSM();
		while(!TimerFlag){}
		TimerFlag = 0;
    }
    return 1;
   }
