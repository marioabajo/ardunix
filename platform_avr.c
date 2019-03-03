#ifdef __AVR__
#include "platform.h"

//#define F_CPU 16000000UL
#define BAUD 115200

#include <util/setbaud.h>

uint8_t CONSOLE_ECHO=1;

static void uart_init(void)
{
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}

static void uart_putchar(char c, FILE *stream)
{
    if (c == '\n')
        uart_putchar('\r', stream);
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
}

static char uart_getchar(FILE *stream)
{
    loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
    // Echo
    //uart_putchar(UDR0, stream);
    
    return UDR0;
}

void serial_setup(FILE *uart)
{
	// Init serial port
	uart_init();
	// Init streams
	fdev_setup_stream(uart, uart_putchar, uart_getchar, _FDEV_SETUP_RW);
	stdin = stdout = stderr = uart;
}

#endif
