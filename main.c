#include <hardware_abstraction_layer/ti_driverlib/MSP430FR2xx_4xx/driverlib.h>

int main(void)
{
    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);
	
    // Motor Controllers

    // Servo Controller

    // Limit Switch Controllers


    // See comment in declaration. Used in all examples.
    PMM_unlockLPM5();

    // Main Loop
    while(1){

    }
}
