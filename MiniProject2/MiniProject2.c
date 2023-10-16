#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// VARIABLES TO BE DISPLAYED
unsigned char sec1 = 0;
unsigned char sec2 = 0;
unsigned char min1 = 0;
unsigned char min2 = 0;
unsigned char hour1 = 0;
unsigned char hour2 = 0;

// TIME VARIABLES
unsigned char seconds = 0;
unsigned char minutes = 0;
unsigned char hours = 0;

// TIMER 1 COMPARE MODE
void Timer1_init_Comp_Mode(void)
{
	TCCR1A = (1<<COM1A1) | (1<<COM1A0) | (1<<FOC1A) ;
	TCCR1B = (1<<CS12) |(1<<CS10) | (1<<WGM12)  ;
	TCNT1 = 0;
	OCR1A = 1000; // Compare register
	TIMSK = (1<<OCIE1A); // Timer compare interrupt Enable
	SREG |= (1<<7);

}

// INTERRUPT 0
void INT0_Init(void){
	DDRD&=~(1<<PD2); // INT0 PIN
	PORTD |= (1<<PD2); // PULL UP
	SREG|=(1<<7); // Enable the global I bit
	GICR|=(1<<INT0); // Enable the external interrupt 0
	MCUCR|=(1<<ISC01); 	MCUCR&=~(1<<ISC00);// Interrupt with falling edge
}

// INTERRUPT 1
void INT1_Init(void){
	DDRD&=~(1<<PD3); // INT1 PIN
	SREG|=(1<<7); // Enable the global I bit
	GICR|=(1<<INT1); // Enable the external interrupt 1
	MCUCR|=(1<<ISC11)|(1<<ISC10); // Interrupt with rising edge

}

// INTERRUPT 2
void INT2_Init(void){
	DDRB&=~(1<<PB2); // INT2 PIN
	PORTB|= (1<<PB2); // PULL UP
	SREG|=(1<<7); // Enable the global I bit
	GICR|=(1<<INT2); // Enable the external interrupt 2
	MCUCSR&=~(1<<ISC2); // Interrupt with falling edge
}


unsigned char counter = 0;

int main(void){

	// INITIALIZATION CODE :
	DDRC|=0x0F; // 4 PINS OUTPUT FOR 7SEGMENT
	PORTC&=0xF0; // COMMON ANODE (NEGATIVE)

	DDRA|=0x3F; // 6 ENABLE PINS OUTPUT
	PORTA&=~(0x3F); // ZERO AT START


	/*ENABLING ALL THE EXTERNAL INTERRUPTS*/
	INT0_Init();
	INT1_Init();
	INT2_Init();


	/*ENABLING THE TIMER*/
	Timer1_init_Comp_Mode();     //TIMER1 ENABLE


	// APPLICATION CODE :
	while(1){
		// DIVIDING THE TIME INTO SINGLE DIGITS
		sec1 = seconds%10;
		sec2 = seconds/10;
		min1 = minutes%10;
		min2 = minutes/10;
		hour1 = hours%10;
		hour2 = hours/10;

		// DISPLAYING THE TIME
		for(counter = 0; counter<6; counter++){
			if(seconds == 60){
				minutes++;
				seconds = 0;
			}
			if(minutes == 60){
				hours++;
				minutes = 0;
			}

			if(counter == 0){
				PORTA = (PORTA&0xC0) |(1&0x3F);   	  // PORTA ENABLES WHICH 7 Segment IS ON
				PORTC = (PORTC&0xF0) |((sec1)&0x0F);  // PORTC DISPLAYS THE NUMBER on the enabled 7 segment
			}
			else if(counter == 1){
				PORTA = (PORTA&0xC0) |(2&0x3F);
				PORTC = (PORTC&0xF0) |((sec2)&0x0F);
			}
			else if(counter == 2){
				PORTA = (PORTA&0xC0) |(4&0x3F);
				PORTC = (PORTC&0xF0) |((min1)&0x0F);
			}
			else if(counter == 3){
				PORTA = (PORTA&0xC0) |(8&0x3F);
				PORTC = (PORTC&0xF0) |((min2)&0x0F);
			}
			else if(counter == 4){
				PORTA = (PORTA&0xC0) |(16&0x3F);
				PORTC = (PORTC&0xF0) |((hour1)&0x0F);
			}
			else if(counter == 5){
				PORTA = (PORTA&0xC0) |(32&0x3F);
				PORTC = (PORTC&0xF0) |((hour2)&0x0F);
			}
			_delay_ms(2);
		}

	} //While

} //Main

ISR(INT0_vect){ //RESETS interrupt 0 occurs

	seconds = 0;     // RESET SECONDS
	minutes = 0; 	 // RESET Minutes
	hours = 0; 		 // RESET Hours
}
ISR(INT1_vect){ //PAUSE when interrupt 1 occurs

	TCCR1B  &=~ (1<<CS12) &~(1<<CS10); // DISABLE TIMER 1 to PAUSE
}
ISR(INT2_vect){ //RESUME when interrupt 2 occurs

	TCCR1B |= (1<<CS12) |(1<<CS10) ; // ENABLE TIMER 1 TO RESUME

}


// TIMER 1 ISR
ISR(TIMER1_COMPA_vect){ // Counts a second each 1 interrupt (1 interrupt each 1000 ms)
	seconds++;
}

