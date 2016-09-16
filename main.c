#define F_CPU 9600000UL

#include <avr/io.h>
#include <util/delay.h>

void setup_adc() {
    // PB3 analog input
    DDRB &= ~(1 << PB3);
    PORTB &= ~(1 << PB3);
    DIDR0 |= (1 << ADC3D);

    ADMUX |= (1 << MUX0) | (1 << MUX1); // PB3 as input
    ADMUX |= (1 << REFS0); // internal 1.1V Vref
    ADMUX |= (1 << ADLAR); // left adjust, 8 bit is enough

    // prescaler 128 - not sure if this high really needed
    ADCSRA |= (1 << ADPS1) | (1 << ADPS0) | (1 << ADPS2) | (1 << ADEN);
}

void setup_pwm() {
    // fast PWM non-inverting on both PB0 and PB1
    // unfortunately ATtiny13 has only a single timer
    TCCR0A = ((1 << WGM00) | (1 << WGM01) | (1 << COM0A1) | (1 << COM0B1));
    TCCR0B = (1 << CS00); // no prescaling, freq = 9.2 MHz / 256 = 36 kHz
    DDRB |= (1 << PB0);
    DDRB |= (1 << PB1);
    OCR0A = 10; // ramp up from low duty cycle
    OCR0B = 255; // start with full power - required for fan to start
}

uint8_t adc_read (void) {
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADCH; // just top
}

#define R_DIVIDER_MULTIPLIER 15
#define V_REF 1.1
#define ADC_MAX 256.0

#define MAX_OUTPUT 12
#define MAX_DUTY_CYCLE_PWM 190
#define LOOP_CYCLE_MS 10

int main(void) {

    // force full 9.2 MHz speed
    CLKPR = (1 << CLKPCE);
    CLKPR = 0;

    setup_adc();
    setup_pwm();
    
    // buttons
    DDRB &= ~(1 << PB2);
    DDRB &= ~(1 << PB4);
    PORTB |= (1 << PB2);
    PORTB |= (1 << PB4);

    while (1) {
        float volts = (R_DIVIDER_MULTIPLIER * V_REF * adc_read()) / ADC_MAX;
        if (volts < MAX_OUTPUT && OCR0A < MAX_DUTY_CYCLE_PWM) {
            OCR0A++;
        } else if (OCR0A != 0) {
            OCR0A--;
        }
        _delay_ms(LOOP_CYCLE_MS);
    
        if (!(PINB & (1 << PB2)) && OCR0B != 0) {
            OCR0B--;
        }
        if (!(PINB & (1 << PB4)) && OCR0B != 255) {
            OCR0B++;
        }
    }
}
