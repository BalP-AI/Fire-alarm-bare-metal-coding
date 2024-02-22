/*
 * main.c
 *
 * Created: 5/30/2023 12:39:42 PM
 *  Author: icsd18174
 */ 

#include <xc.h>
#include <avr/interrupt.h>
#include <avr/io.h>

float temp;
float vout;
int is_on = 0;

volatile uint16_t adc_result_A0;
volatile uint16_t adc_result_A1;

void setup()
{
//pinMode(A0,INPUT);
//pinMode(A1, INPUT);
//pinMode(piezo,OUTPUT);
cli(); // stop interrupts
/* Initialization */
DDRD = 0b11111100; /* Set pins 2--7 as outputs */
DDRB = 0b11111111;
ADMUX = 0b00100000; /* bits 7,6: select AREF as reference */
/* bit 5: left adjust A/D result */
/* bits 3-0: select ADC0 as input */
ADCSRA = 0b11001111; /* bit 7: A/D enable */
/* bit 6: start conversion */
/* bit 5: auto trigger enable */
/* bit 3: interrupt enable */
/* bits 2-0: prescaler clock/128 */
ADCSRB = 0;

sei(); // allow interrupts
}
//ISR(TIMER1_COMPA_vect){
void buzzer_ON()
{
TCCR1A = 0b01000000; // set entire TCCR1A register to 0

TCCR1B = 0; // same for TCCR1B
TCNT1  = 0; // initialize counter value to 0
// set compare match register for 5000 Hz increments
// turn on CTC mode
// Set CS12, CS11 and CS10 bits for 1 prescaler 64
TCCR1B = 0b00001011;

OCR1A = 500 ; // = 16000000 / (1 * 5000) - 1 (must be <65536)

// enable timer compare interrupt
//TIMSK1 |= (1 << OCIE1A);
}
void buzzer_OFF()
{
TCCR1A = 0; // set entire TCCR1A register to 0
TCCR1B = 0; // same for TCCR1B
TCNT1  = 0; // initialize counter value to 0
// set compare match register for 5000 Hz increments
OCR1A = 500 ; // = 16000000 / (1 * 5000) - 1 (must be <65536)
// turn on CTC mode
TCCR1B |= (1 << WGM12);
// Set CS12, CS11 and CS10 bits for 1 prescaler 64
TCCR1B |= (0 << CS12) | (0 << CS11) | (0 << CS10);
// enable timer compare interrupt
//TIMSK1 |= (1 << OCIE1A);
} 
//}
ISR(ADC_vect)
{
if (ADMUX == 0b00100001)
{
adc_result_A1 = ADCH;
ADMUX =  0b00100000; // Switch to channel ADC0 (A0) for the next conversion
}
else
{
adc_result_A0 = ADCH;
ADMUX = 0b00100001; // Switch to channel ADC1 (A1) for the next conversion
}
ADCSRA =  0b11001111; /* start new conversion */
}

void loop()
{
vout = adc_result_A1;
temp = vout * 5000 /1024;
temp = (temp -500) / 10; //lab reading was 158 translating to 27 celcius

if (temp>10 && adc_result_A0 < 100)
{
PORTD |= (1 << PORTD4);
PORTD &= ~(1 << PORTD5);
PORTD &= ~(1 << PORTD6);
if (is_on == 0){is_on =1;
buzzer_ON();}}
else if ( temp>30 && temp<=80 )
{
PORTD |= (1 << PORTD5);
PORTD &= ~(1 << PORTD6);
PORTD &= ~(1 << PORTD4);
if(is_on == 1){
is_on = 0;
buzzer_OFF();
}}

else if (temp>10  && temp <=30 && adc_result_A0 > 150)
{
PORTD |= (1 << PORTD6); //set PD6 to logic 1
PORTD &= ~(1 << PORTD5); //set PD5 to logic 0
PORTD &= ~(1 << PORTD4);
if(is_on == 1){
is_on = 0;
buzzer_OFF();
}}}

int main(void)
{
setup();
while(1){
    loop();
}}