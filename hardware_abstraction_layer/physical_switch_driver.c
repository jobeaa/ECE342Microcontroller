/*
 * physical_switch_driver.c
 *
 *  Created on: May 26, 2023
 *      Author: silas
 */


#include "physical_switch_driver.h"
#include "gpio.h"

// THIS IMPLEMENTATION IS KINDA GROSS BC ITS VERY HARDWARE DEPENDENT, BUT IT WORKS.
// MSP430FR4133 only has interrupts on ports 1 and 2; 8 pins per port on 64-pin LFQP package.
#define GPIO_PINS_PER_PORT 8

static void (*registered_switch_pressed_events[16])(void);

static int get_registered_switch_pressed_event_index(uint16_t gpio_port, uint16_t gpio_pin) {
    uint16_t registered_event_index = (gpio_port == GPIO_PORT_P1) ? 0 : GPIO_PINS_PER_PORT;

    // GPIO_PIN_X is one-hot encoded
    uint16_t i;
    for(i = 0; i < GPIO_PINS_PER_PORT; i++) {
        if(gpio_pin & (1<<i)) {
            return i;
        }
    }
    return -1;
}

static void get_gpio_port_pin(uint16_t* return_gpio_port, uint16_t* return_gpio_pin,
                              uint16_t registered_event_index) {

}

inline static void register_physical_switch_event(physical_switch_driver_t* driver) {
    // User defined function will never be placed at address 0 by linker, so we can
    // use this address to check whether the entry in the table is empty or a registered
    // event.
    static uint16_t is_register_table_zeroed = 0;
    if(is_register_table_zeroed) {
        uint16_t i;
        for(i = 0; i < GPIO_PINS_PER_PORT*2; i++) {
            registered_switch_pressed_events[i] = 0;
        }
        is_register_table_zeroed = 1;
    }
    registered_switch_pressed_events[get_registered_switch_pressed_event_index(
            driver->gpio_port, driver->gpio_pin)] = driver->switch_pressed_event;
}

void physical_switch_driver_open(physical_switch_driver_t* driver) {
    GPIO_setAsInputPinWithPullUpResistor(driver->gpio_port, driver->gpio_pin);
    GPIO_enableInterrupt(driver->gpio_port, driver->gpio_pin);
    GPIO_selectInterruptEdge(driver->gpio_port, driver->gpio_pin, GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_clearInterrupt(driver->gpio_port, driver->gpio_pin);

    register_physical_switch_event(driver);
}

void ISR_registered_physical_switch_event_dispatcher(uint16_t gpio_port) {
    uint16_t i;
    for(i = 0; i < GPIO_PINS_PER_PORT*2; i++) {
        if(registered_switch_pressed_events[i] != 0) {
            (*registered_switch_pressed_events[i])();
        }
    }
}
