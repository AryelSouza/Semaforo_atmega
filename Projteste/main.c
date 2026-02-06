#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

/* ===== PINOS ===== */
#define LED_VERDE      PB0
#define LED_VERMELHO   PB7
#define LED_PEDESTRE   PB3
#define LED_AMARELO    PD6   // OC0A (PWM)
#define BUZZER         PB2

#define BOTAO          PD2   // INT0
#define ADC_PED        0     // PC0 / ADC0

/* ===== TEMPOS (segundos) ===== */
#define TEMPO_VERMELHO 8
#define TEMPO_PISCA    2

/* ===== ESTADOS ===== */
typedef enum {
	VERDE,
	AMARELO_FADE,
	VERMELHO
} estado_t;

volatile estado_t estado = VERDE;
volatile uint8_t segundos = 0;
volatile uint8_t tick = 0;
volatile uint8_t botao = 0;

uint8_t brilho = 0;
uint8_t pisca = 0;
uint8_t ped_on = 1;

/* ===== INTERRUPÇÕES ===== */
ISR(INT0_vect) {
	botao = 1;   // apenas sinaliza evento
}

ISR(TIMER1_COMPA_vect) {
	segundos++;
	tick++;
}

/* ===== ADC ===== */
void adc_init(void) {
	ADMUX  = (1<<REFS0);   // AVcc
	ADCSRA = (1<<ADEN)    // Enable ADC
	| (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); // Prescaler 128
	DIDR0  = (1<<ADC0D);  // Desabilita buffer digital do PC0
}

uint16_t adc_read(uint8_t ch) {
	ADMUX = (ADMUX & 0xF0) | ch;
	ADCSRA |= (1<<ADSC);
	while (ADCSRA & (1<<ADSC));
	return ADC;
}

/* ===== FILTRO DO SENSOR ===== */
uint8_t pedestre_presente_filtrado(void) {
	uint32_t soma = 0;

	for (uint8_t i = 0; i < 16; i++) {
		soma += adc_read(ADC_PED);
	}

	uint16_t media = soma / 16;

	// 0.3 V ? ADC < 61
	return (media < 61);
}

/* ===== MAIN ===== */
int main(void) {

	/* GPIO */
	DDRB |= (1<<LED_VERDE)
	| (1<<LED_VERMELHO)
	| (1<<LED_PEDESTRE)
	| (1<<BUZZER);

	DDRD |= (1<<LED_AMARELO);

	DDRD &= ~(1<<BOTAO);
	PORTD |= (1<<BOTAO); // pull-up

	adc_init();

	/* PWM – Timer0 */
	TCCR0A = (1<<COM0A1) | (1<<WGM01) | (1<<WGM00);
	TCCR0B = (1<<CS01) | (1<<CS00);
	OCR0A  = 0;

	/* Timer1 – base 1s */
	TCCR1B = (1<<WGM12);
	OCR1A  = 15624;
	TCCR1B |= (1<<CS12) | (1<<CS10);
	TIMSK1 = (1<<OCIE1A);

	/* INT0 */
	EICRA = (1<<ISC01);
	EIMSK = (1<<INT0);

	sei();

	while (1) {

		switch (estado) {

			/* ===== VERDE ===== */
			case VERDE:
			PORTB = (1<<LED_VERDE);
			OCR0A = 0;

			if (botao) {
				botao = 0; // consome evento

				if (pedestre_presente_filtrado()) {
					brilho = 0;
					tick = 0;
					segundos = 0;
					estado = AMARELO_FADE;
				}
			}
			break;

			/* ===== AMARELO COM FADE ===== */
			case AMARELO_FADE:
			PORTB = 0;

			if (tick >= 1) {
				tick = 0;

				if (brilho < 240) {
					brilho += 10;
					OCR0A = brilho;
					} else {
					OCR0A = 0;
					segundos = 0;
					pisca = 0;
					ped_on = 1;
					estado = VERMELHO;
				}
			}
			break;

			/* ===== VERMELHO + PEDESTRE ===== */
			case VERMELHO:
			PORTB = (1<<LED_VERMELHO);

			if (segundos < (TEMPO_VERMELHO - TEMPO_PISCA)) {
				PORTB |= (1<<LED_PEDESTRE);
				PORTB &= ~(1<<BUZZER);
				} else {
				pisca++;
				if (pisca >= 20) {
					pisca = 0;
					ped_on ^= 1;
				}

				if (ped_on) {
					PORTB |= (1<<LED_PEDESTRE);
					PORTB |= (1<<BUZZER);
					} else {
					PORTB &= ~(1<<LED_PEDESTRE);
					PORTB &= ~(1<<BUZZER);
				}
			}

			if (segundos >= TEMPO_VERMELHO) {
				segundos = 0;
				estado = VERDE;
			}
			break;
		}
	}
}
