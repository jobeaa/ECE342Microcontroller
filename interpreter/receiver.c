#include <eusci_a_uart.h>
#include <msp430.h>
#include <interpreter/commands.h>
#include <interpreter/lcd_msg.h>
#include <driverlib.h>


#ifdef DEBUG
#include <interpreter/test.h>
uint16_t test_idx = 0;
#endif

// Initializes the UART device, the button (GPIO PORT_P2, PIN6)
// Also enables interrupts and sets the LCD blank
void init_receiver(void) {
    EUSCI_A_UART_initParam init_param = {.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK, .clockPrescalar = 52, .secondModReg = 49, .firstModReg = 1, .overSampling = 1};


    __bis_SR_register(SCG0);                 // disable FLL
    CSCTL3 |= SELREF__REFOCLK;               // Set REFO as FLL reference source
    CSCTL0 = 0;                              // clear DCO and MOD registers
    CSCTL1 &= ~(DCORSEL_7);                  // Clear DCO frequency select bits first
    CSCTL1 |= DCORSEL_3;                     // Set DCO = 8MHz
    CSCTL2 = FLLD_0 + 243;                   // DCODIV = 8MHz
    __delay_cycles(3);
    __bic_SR_register(SCG0);                 // enable FLL
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1)); // Poll until FLL is locked

    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK; // set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
                                             // default DCODIV as MCLK and SMCLK source

    P1SEL0 |= BIT0 | BIT1;                    // set 2-UART pin as second function
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
    return EUSCI_A_UART_receiveData(__MSP430_BASEADDRESS_EUSCI_A0__);
#else
    return TEST_STR[test_idx++];
#endif
}


// The main switch that reads a byte from the stream and executes the correct function
void receiver_main(void) {
   uint8_t recv_byte = 0, arg1, arg2;
   int16_t nargs, stream_size = 0, i;

   uint8_t byte_stream[50];

   write_msg("INPUT ", "CMDS  ");
   while (recv_byte != M02_NUM) {
       recv_byte = get_byte();
       byte_stream[stream_size++] = recv_byte;

       if ((nargs = cmd_to_nargs(recv_byte)) < 0) {
           write_msg("REEEEE", "REEEEE");
           while (1);
       }

       for (i = 0; i < nargs; i++) {
           byte_stream[stream_size++] = get_byte();
       }
   }
   write_msg("      ", "      ");

   for (i = 0; i < stream_size; i++) {
       // Read the command byte and determine how many arguments it has
       recv_byte = byte_stream[i];
       nargs = cmd_to_nargs(recv_byte);

       // Check if this is a valid command
       if (nargs < 0) {
           write_msg("FUCKFU", "FUCKFU");
           while (1);
       }

       // Determine which function to call
       switch (recv_byte) {
       case G00_NUM:
           arg1 = byte_stream[i++];
           arg2 = byte_stream[i++];
           G00(arg1, arg2);
           break;
       case G01_NUM:
           arg1 = byte_stream[i++];
           arg2 = byte_stream[i++];
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
           M02();
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
