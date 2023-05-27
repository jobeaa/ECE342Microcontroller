/*
 * physical_switch_driver.h
 *
 *  Created on: May 8, 2023
 *      Author: silas
 */

#ifndef PHYSICAL_SWITCH_DRIVER_H
#define PHYSICAL_SWITCH_DRIVER_H

#include <stdint.h>

typedef struct {
    uint16_t gpio_port;
    uint16_t gpio_pin;
    void (*switch_pressed_event)(void);
} physical_switch_driver_t;

// Initialize driver and underlying peripherals.
void physical_switch_driver_open(physical_switch_driver_t* driver);

// "Private". Used within the HAL to abstract away ISRs bound to a specific pin.
void ISR_registered_physical_switch_event_dispatcher(uint16_t gpio_port);

#endif // PHYSICAL_SWITCH_DRIVER_H
