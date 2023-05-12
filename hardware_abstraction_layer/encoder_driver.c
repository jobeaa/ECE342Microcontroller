/*
 * encoder_driver.c
 *
 *  Created on: May 10, 2023
 *      Author: silas
 */

#include "encoder_driver.h"
#include "gpio.h"

void encoder_driver_open(encoder_driver_t* driver){
    GPIO_setAsInputPinWithPullDownResistor(driver->signal_a_gpio_port, driver->signal_a_gpio_pin);
    GPIO_setAsInputPinWithPullDownResistor(driver->signal_b_gpio_port, driver->signal_b_gpio_pin);

    GPIO_enableInterrupt(driver->signal_a_gpio_port, driver->signal_a_gpio_pin);
    GPIO_selectInterruptEdge(driver->signal_a_gpio_port, driver->signal_a_gpio_pin, GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_clearInterrupt(driver->signal_a_gpio_port, driver->signal_a_gpio_pin);

}

void encoder_driver_close(encoder_driver_t* driver){
    //TODO: how to uninit gpios?
    //      disable interrupt
}

inline void encoder_driver_signal_a_rising_edge_event(encoder_driver_t* driver){
    // get sysclock

    // get sig b state

    // combine data

    // write to buffer
}
