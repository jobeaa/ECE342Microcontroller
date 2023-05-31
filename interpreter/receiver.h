#ifndef RECEIVER_H_
#define RECEIVER_H_


void slow_clock(void);

void init_receiver(void);

void receiver_main(void);

// Interrupt vector, clears the tool change bool and the LCD
void port2_pin6_isr(void);

#endif
