/*
 * mini_project2.c
 *
 * Created on: Sep 17, 2023
 * Author: Salma Mizar
 */
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

int second=0;
int minute =0;
int hour=0 ;

void Timer1_CTC_Init(void)
{
	//enable the interrupts//
	SREG |=(1<<7);

	//initial value for the timer to start counting from//
	TCNT1=0;


	//enable compare mode//
	TIMSK|=(1<<OCIE1A);


	//setting the compare value to make compare matches //
	OCR1A=15625;

	//register TCCR1A//
	//non PWM//
	//mode 4 CTC //
	//normal port operation//
	TCCR1A=(1<<FOC1A) | (1<<FOC1B); //set selected bits to ones and others remain the same//

	//register TCCR1B//
	//mode 4 CTC//
	//prescaler 64//
	TCCR1B=(1<<WGM12) | (1<<CS10) | (1<<CS11); //set selected bits to ones and others remain the same//


}

ISR (TIMER1_COMPA_vect)
{
	//default action is incrementing seconds //
	second++;
	//as minute is 60 seconds ,when seconds counter reaches 60 it will reset and increment minutes counter by 1//
	if (second ==60)
	{
		second = 0;
		minute ++;
	}
	//as hour is 60 minutes ,when minutes counter reaches 60 it will reset and increment hours counter by 1//
	if (minute==60)
	{
		second=0;
		minute =0;
		hour ++;
	}
}

void INT0_Init_RESET (void)
{
	DDRD &= ~(1<<PD2); //input pin//
	PORTD |=(1<<PD2); //pull up resistor//
	MCUCR |=(1<<ISC01);//falling edge with INT0//
	GICR |=(1<<INT0);//enable external interrupt//
}

//ISR for INT0 to make it reset Stop Watch time //
ISR (INT0_vect)
{
	second =0;
	minute =0;
	hour = 0;
	//all the factors are zero to reset the counter//
}

void INT1_Init_PAUSE (void)
{
	DDRD &= ~(1<<PD3); //input pin//
    MCUCR |=(1<<ISC10) | (1<<ISC11);//rising edge with INT1//
    GICR |=(1<<INT1);//enable external interrupt//
}

//ISR for INT1 to make it pause Stop Watch time //
ISR(INT1_vect)
{
	//to stop counting we need to clear the clock source//
	TCCR1B&= ~(1 << CS10) & ~(1 << CS11);
}

void INT2_Init_RESUME(void)
{
	DDRB &= ~(1<<PB2); //input pin//
	PORTB |=(1 << PB2);//pull up resistance//
	MCUCR &= ~(1<<ISC2);//falling edge with INT2//
	GICR |=(1<<INT2);//enable external interrupt//

}

//ISR for INT2 to make it resume Stop Watch time //
ISR(INT2_vect)
{
	//to resume counting we need to set the clock source//
	TCCR1B|= (1 << CS10) | (1 << CS11);
}


int main (void)
{
	DDRC|=0x0F; //first four input pins(for decoder)//
	PORTC &= 0xF0;//initial value for first four (0)//

	DDRA |=0xFF;//first six input pins(for seven segments)//
	PORTA =0xFF;//initial value for first six (1)//

	//setting timer and enabling interrupts INT0 , INT1 ,and INT2//
	Timer1_CTC_Init();
	INT0_Init_RESET();
	INT1_Init_PAUSE();
	INT2_Init_RESUME();

	//super loop//
	while(1)
	{
		PORTA = ( 1 << PA5 );                               //Enable 1st seven segment//
		PORTC = ( PORTC & 0xF0 ) | ( second % 10 );			// Show the first digit of seconds//
		_delay_ms(3);

		PORTA = ( 1 << PA4 );								// Enable 2nd seven segment//
		PORTC = ( PORTC & 0xF0 )  |  ( second / 10 );		// Show the second digit of seconds//
		_delay_ms(3);

		PORTA = ( 1 << PA3 );								// Enable 3rd seven segment//
		PORTC = ( PORTC & 0xF0 ) | ( minute % 10 );			// Show the first digit of minutes//
		_delay_ms(3);

		PORTA = ( 1 << PA2 );								// Enable 4th seven segment//
		PORTC = ( PORTC & 0xF0 ) | ( minute / 10 );			// Show the second digit of minutes//
		_delay_ms(3);

		PORTA = ( 1 << PA1 );								// Enable 5th seven segment//
	    PORTC = ( PORTC & 0xF0 ) | ( hour % 10 );			// Show the first digit of hours//
	    _delay_ms(3);

	    PORTA = ( 1 << PA0 );								// Enable 6th seven segment//
		PORTC = ( PORTC & 0xF0 ) | ( hour / 10 );			// Show the second digit of hours//
		_delay_ms(3);
	}

}
