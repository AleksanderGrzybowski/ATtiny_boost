#define F_CPU 9600000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdbool.h>

void setup_adc() {
    DDRB &= ~(1 << PB3);
    PORTB &= ~(1 << PB3);
    // Set the ADC input to 
    ADMUX |= (1 << MUX0) | (1 << MUX1); // PB3 as input
    ADMUX |= (1 << REFS0); // internal Vref
    ADMUX |= (1 << ADLAR);

    ADCSRA |= (1 << ADPS1) | (1 << ADPS0) | (1 << ADPS2) | (1 << ADEN);

}

uint8_t adc_read (void) {
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADCH;
}


int main(void) {

    // force full 9.2 MHz speed
    CLKPR = (1 << CLKPCE);
    CLKPR = 0;

    TCCR0A  = ((1<<WGM00) | (1<<WGM01) | (1 << COM0A1));
    TCCR0A |= (1 << COM0B1);
    TCCR0B  = (1<<CS00); // no div
    DDRB |= (1 << PB0);
    DDRB |= (1 << PB1);
    OCR0A = 10;
    OCR0B = 255;
    setup_adc();
    
    DIDR0 |= (1 << ADC3D);

    DDRB &= ~(1 << PB2);
    DDRB &= ~(1 << PB4);
    PORTB |= (1 << PB2);
    PORTB |= (1 << PB4);

    while (1) {
        float volts = (15.0 * 1.1 * adc_read())/256.0  ;
        if (volts < 12 && OCR0A < 190) {
            OCR0A++;
        } else if (OCR0A != 0) {
            OCR0A--;
        }
        _delay_ms(10);
    

        if (!(PINB & (1 << PB2)) && OCR0B != 0) {
            OCR0B--;
        }
        if (!(PINB & (1 << PB4)) && OCR0B != 255) {
            OCR0B++;
        }
    }
    


}
