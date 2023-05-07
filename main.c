#include "driverlib.h"

/**
 * main.c
 */
int main(void)
{
    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);
	
    //PA.x output
    GPIO_setAsOutputPin(
        GPIO_PORT_PA,
        GPIO_PIN0 + GPIO_PIN1 + GPIO_PIN2 + GPIO_PIN3 +
        GPIO_PIN4 + GPIO_PIN5 + GPIO_PIN6 + GPIO_PIN7 +
        GPIO_PIN8 + GPIO_PIN9 + GPIO_PIN10 + GPIO_PIN11 +
        GPIO_PIN12 + GPIO_PIN13 + GPIO_PIN14 + GPIO_PIN15
        );


    //Set all PA pins HI
    GPIO_setOutputHighOnPin(
        GPIO_PORT_PA,
        GPIO_PIN0 + GPIO_PIN1 + GPIO_PIN2 + GPIO_PIN3 +
        GPIO_PIN4 + GPIO_PIN5 + GPIO_PIN6 + GPIO_PIN7 +
        GPIO_PIN8 + GPIO_PIN9 + GPIO_PIN10 + GPIO_PIN11 +
        GPIO_PIN12 + GPIO_PIN13 + GPIO_PIN14 + GPIO_PIN15
        );

    // See comment in declaration. Used in all examples.
    PMM_unlockLPM5();

	return 0;
}
