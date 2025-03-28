/*
* Lab6.c
*
* Created: 3/28/2025 11:30:37 PM
* Author : Alex Cooper

748
x = 7, Z = 8

timer interrupt period = (7+1) = 8ms
PWM frequency = (8+1)*100 = 900 Hz = 1.111ms
*/ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#define LED_ON PORTB |= (1 << 5)
#define LED_OFF PORTB &= ~(1 << 5)

short int duty_cycle = 0;
 
const unsigned int keypad[4][4] = {
    {1, 2, 3, 0},
    {4, 5, 6, 0},
    {7, 8, 9, 0},
    {0, 0, 0, 0},
};
 
void Init() {
    // Initialize keypad
    DDRD |= (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7); // Set registers PD4-7 to high (output) // Acts as rows
    PORTD |=  (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7); // Set rows to high
    DDRB &= ~(1 << 0) & ~(1 << 1) & ~(1 << 2) & ~(1 << 3); // Set registers PB0-3 to low (input) // Acts as columns
    PORTB |=  (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3); // enable pullup resistors for the columns

    DDRB |= (1 << 5); // Set register PD5 to high (output) for LED   
}

void Timer0Init() {
    TCCR0A |= (1 << WGM01); // CTC
    TCCR0B |= (1 << CS02); // 1024 prescale = 16.384ms
    TIMSK0 |= (1 << OCIE0A); // OVF interrupt
    OCR0A = 124; // need 8ms; 256*8/16.384 = 125
}

void Timer2Init() {
    TCCR2A |= (1 << WGM21); // CTC
    TCCR2B |= (1 << CS21) | (1 << CS22); // 256 prescale = 4.096ms
    TIMSK2 |= (1 << OCIE2A) | (1 << OCIE2B); // enable a and b interrupts

    OCR2A = 68; // need 1.11ms; 256*1.11/4.096 ~= 69
}

int main(void) {
    Init(); // Set up register flags
    Timer0Init();
    Timer2Init();
    
    while (1) {}
    sei(); // enable global interrupts
    return 0;
}

// Timer/Counter 2 Compare Match A
ISR(TIMER2_COMPA_vect) {
    LEDON;
}
// Timer/Counter 2 Compare Match B
ISR(TIMER2_COMPB_vect) {
    LEDOFF;
}

ISR(TIMER0_COMPA_vect) {
    for (int i = 4; i < 8; i++)  {
        PORTD &= ~(1 << i); // Disable row pullup resistor
        for (int j = 0; j < 4; j++) {
            if (!(PINB & (1 << j))) {
                PORTD |= (1 << i); // reenable row pullup resistor
                duty_cycle = keypad[i-4][j];
                OCR2B = OCR2A/10 * duty_cycle; // calculate LED brightness
            }
        }
        PORTD |= (1 << i); // reenable row pullup resistor
    }
}