#include <hardware_abstraction_layer/ti_driverlib/MSP430FR2xx_4xx/driverlib.h>
#include <interpreter/receiver.h>
#include <interpreter/commands.h>
#include <interpreter/lcd_msg.h>

int main(void)
{
    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);
	
    // Motor Controllers

    // Servo Controller

    // Limit Switch Controllers


    // See comment in declaration. Used in all examples.
    PMM_unlockLPM5();

    init_receiver();

    while (1) receiver_main();
}
