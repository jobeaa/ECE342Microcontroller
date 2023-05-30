#include <eusci_a_uart.h>
#include <msp430.h>
#include <interpreter/commands.h>
#include <interpreter/lcd_msg.h>
#include <driverlib.h>

#define DEBUG

#ifdef DEBUG
#include <interpreter/test.h>
uint16_t test_idx = 0;
#endif

// Initializes the UART device, the button (GPIO PORT_P2, PIN6)
// Also enables interrupts and sets the LCD blank
void init_receiver(void) {
    EUSCI_A_UART_initParam init_param = {.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK};

    EUSCI_A_UART_init(__MSP430_BASEADDRESS_EUSCI_A0__, &init_param);

    EUSCI_A_UART_enable(__MSP430_BASEADDRESS_EUSCI_A0__);

    // Set up the button to trigger an interrupt when pressed
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_selectInterruptEdge(GPIO_PORT_P2, GPIO_PIN6, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P2, GPIO_PIN6);
    GPIO_clearInterrupt(GPIO_PORT_P2, GPIO_PIN6);
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN6);
    GPIO_clearInterrupt(GPIO_PORT_P2, GPIO_PIN6);
    __enable_interrupt();

    LCD_init();
    write_msg("      ", "      ");
}


// Gets a byte from the command source
// This is TEST_STR if in DEBUG mode, and is UART otherwise.
uint8_t get_byte(void) {
#ifndef DEBUG
    EUSCI_A_UART_receiveData(__MSP430_BASEADDRESS_EUSCI_A0__);
#else
    return TEST_STR[test_idx++];
#endif
}


// The main switch that reads a byte from the stream and executes the correct function
void receiver_main(void) {
   uint8_t recv_byte;
   int16_t nargs;
   uint8_t arg1, arg2;

   while (1) {
       // Read the command byte and determine how many arguments it has
       recv_byte = get_byte();
       nargs = cmd_to_nargs(recv_byte);

       // Check if this is a valid command
       if (nargs < 0) return;

       // Determine which functino to call
       switch (recv_byte) {
       case G00_NUM:
           arg1 = get_byte();
           arg2 = get_byte();
           G00(arg1, arg2);
           break;
       case G01_NUM:
           arg1 = get_byte();
           arg2 = get_byte();
           G01(arg1, arg2);
           break;
       case G20_NUM:
           G20();
           break;
       case G21_NUM:
           G21();
           break;
       case G90_NUM:
           G90();
           break;
       case G91_NUM:
           G91();
           break;
       case M02_NUM:
           return;
       case M06_NUM:
           M06();
           break;
       case M72_NUM:
           M72();
           break;
       default:
           return;
       }
   }
}
