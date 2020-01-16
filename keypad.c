#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>

#define UART_TX PB4
#define UART_BAUDRATE (9600)
#define TXDELAY (int)(((F_CPU/UART_BAUDRATE)-7 +1.5)/3)

uint8_t pre_val=0;

void uart_putc(char c) {
  uint8_t sreg;

  sreg = SREG;
  cli();
  /* direct UART
  __asm volatile(
      " cbi %[uart_port], %[uart_pin] \n\t" // start bit
      " in r0, %[uart_port] \n\t"
      " ldi r30, 3 \n\t" // stop bit + idle state
      " ldi r28, %[txdelay] \n\t"
      "TxLoop: \n\t"
      // 8 cycle loop + delay - total = 7 + 3*r22
      " mov r29, r28 \n\t"
      "TxDelay: \n\t"
      // delay (3 cycle * delayCount) - 1
      " dec r29 \n\t"
      " brne TxDelay \n\t"
      " bst %[ch], 0 \n\t"
      " bld r0, %[uart_pin] \n\t"
      " lsr r30 \n\t"
      " ror %[ch] \n\t"
      " out %[uart_port], r0 \n\t"
      " brne TxLoop \n\t"
      :
      : [uart_port] "I" (_SFR_IO_ADDR(PORTB)),
      [uart_pin] "I" (UART_TX),
      [txdelay] "I" (TXDELAY),
      [ch] "r" (c)
        : "r0","r28","r29","r30"
          );
          */
  /* inverted UART */
  __asm volatile(
      " cbi %[uart_port], %[uart_pin] \n\t" // clear
      " in r0, %[uart_port] \n\t"
      " sbi %[uart_port], %[uart_pin] \n\t" // start bit
      " ldi r30, 3 \n\t" // stop bit + idle state
      " ldi r28, %[txdelay] \n\t"
      "TxLoop: \n\t"
      // 8 cycle loop + delay - total = 7 + 3*r22
      " mov r29, r28 \n\t"
      "TxDelay: \n\t"
      // delay (3 cycle * delayCount) - 1
      " dec r29 \n\t"
      " brne TxDelay \n\t"
      " bst %[ch], 0 \n\t"
      " bld r0, %[uart_pin] \n\t"
      " lsr r30 \n\t"
      " ror %[ch] \n\t"
      " out %[uart_port], r0 \n\t"
      " brne TxLoop \n\t"
      " cbi %[uart_port], %[uart_pin] \n\t" // clear
      :
      : [uart_port] "I" (_SFR_IO_ADDR(PORTB)),
      [uart_pin] "I" (UART_TX),
      [txdelay] "I" (TXDELAY),
      [ch] "r" (~c)
        : "r0","r28","r29","r30"
          );
  SREG = sreg;
  sei();
}

/*
void uart_puts(const char *s) {
         while (*s) uart_putc(*(s++));
}
*/

SIGNAL(SIG_OVERFLOW0){
  uint8_t val=(PINB & 0x0f);
  if(val==pre_val)
    return;
  uart_putc(val+32);
  pre_val=val;
}

int main(void)
{

//	TCCR1B = (0<<CS12)|(1<<CS11)|(0<<CS10); //clk/8 = 500Khz = 2uS
  TCCR0A = 0;
  TCCR0B = (0<<CS02)|(1<<CS01)|(1<<CS00); //CLKio/64
  TIMSK0 = (1<<TOIE0);

  //PORTB |= 1 << UART_TX;
  PORTB |= 0x1f;
  DDRB |= 1 << UART_TX;
  PORTB &= ~(1 << UART_TX); //reset uart pin

//  uart_puts("Hello World!\n");

  // DDR  1 - out
  // DDR  0 - in
  // PORT 1 - pullup enable
  // PORT 0 - pullup disable


  sei();

  while(1){
    sleep_mode();
  }

  return 0;
}
