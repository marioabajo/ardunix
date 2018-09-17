#ifdef __AVR__
#include "platform.h"

#define F_CPU 16000000UL
#define BAUD 115200

#include <util/setbaud.h>

uint8_t CONSOLE_ECHO=1;

void uart_init(void)
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

void uart_putchar(char c, FILE *stream)
{
    if (c == '\n')
        uart_putchar('\r', stream);
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
}

char uart_getchar(FILE *stream)
{
    loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
    // Echo
    //uart_putchar(UDR0, stream);
    
    return UDR0;
}

/*  WARNING: this is out of warranty as the FILE structure may change
 *           without advise in future arduino versions.
 */
/*FILE *fmemopen(void *buf, int size, const char *mode)
{
  FILE *f;

  if ((f = malloc(sizeof(FILE))) == NULL)
    return NULL;

  f->buf = buf;
  f->size = size;
  f->len = 0;
  f->puf = NULL;   // TODO
  f->get = NULL;   // TODO
  f->udata = NULL; // TODO

  return f;
}*/

FILE *fopen(const char *path, const char *mode)
{
  // TODO
  return NULL;
}

#endif
