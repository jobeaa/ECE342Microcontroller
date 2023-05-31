/*
 * physical_switch_driver.c
 *
 *  Created on: May 26, 2023
 *      Author: silas
 */
#include <stdbool.h>

#include "physical_switch_driver.h"
#include "gpio.h"
#include "clock_driver.h"

#define GPIO_ISR_DEBOUNCING_TIME_MS 5

// THIS IMPLEMENTATION IS KINDA GROSS BC ITS VERY HARDWARE DEPENDENT, BUT IT WORKS.
// MSP430FR4133 only has interrupts on ports 1 and 2; 8 pins per port on 64-pin LFQP package.
#define GPIO_PINS_PER_PORT 8

static void (*registered_switch_pressed_events[16])(void);
static void (*registered_switch_released_events[16])(void);
static bool is_next_transition_high_to_low_switches[16];
static clock_driver_time_t last_event_timestamps[16];

static int get_registered_event_index(uint16_t gpio_port, uint16_t gpio_pin) {
    uint16_t registered_event_index = (gpio_port == GPIO_PORT_P1) ? 0 : GPIO_PINS_PER_PORT;

    // GPIO_PIN_X is one-hot encoded
    uint16_t i;
    for(i = 0; i < GPIO_PINS_PER_PORT; i++) {
        if(gpio_pin & (1<<i)) {
            return registered_event_index + i;
        }
    }
    return -1;
}

inline static void register_physical_switch_event(physical_switch_driver_t* driver) {
    // User defined function will never be placed at address 0 by linker, so we can
    // use this address to check whether the entry in the table is empty or a registered
    // event.
    static bool is_register_table_zeroed = false;
    if(is_register_table_zeroed) {
        uint16_t i;
        for(i = 0; i < GPIO_PINS_PER_PORT*2; i++) {
            registered_switch_pressed_events[i] = SWITCH_EVENT_DO_NOTHING;
            registered_switch_released_events[i] = SWITCH_EVENT_DO_NOTHING;
            last_event_timestamps[i].time_ms = 0;
            is_next_transition_high_to_low_switches[i] = true;
        }
        is_register_table_zeroed = true;
    }
    uint16_t index = get_registered_event_index(driver->gpio_port, driver->gpio_pin);
    registered_switch_pressed_events[index] = driver->switch_pressed_event;
    registered_switch_released_events[index] = driver->switch_released_event;
}

void physical_switch_driver_open(physical_switch_driver_t* driver) {
    GPIO_setAsInputPinWithPullUpResistor(driver->gpio_port, driver->gpio_pin);
    GPIO_enableInterrupt(driver->gpio_port, driver->gpio_pin);
    GPIO_selectInterruptEdge(driver->gpio_port, driver->gpio_pin, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_clearInterrupt(driver->gpio_port, driver->gpio_pin);

    register_physical_switch_event(driver);
}

void ISR_registered_physical_switch_event_dispatcher(uint16_t gpio_port, uint16_t gpio_ith_pin) {
    uint16_t event_index = (gpio_port == GPIO_PORT_P1) ? gpio_ith_pin : gpio_ith_pin + GPIO_PINS_PER_PORT;
    void (*event)(void) = SWITCH_EVENT_DO_NOTHING;
    if(is_next_transition_high_to_low_switches[event_index]) {
        if(registered_switch_released_events[event_index] != SWITCH_EVENT_DO_NOTHING) {
            event = registered_switch_released_events[event_index];
            GPIO_selectInterruptEdge(gpio_port, (1<<gpio_ith_pin), GPIO_HIGH_TO_LOW_TRANSITION);
        }
    }
    else {
        if(registered_switch_pressed_events[event_index] != SWITCH_EVENT_DO_NOTHING) {
            event = registered_switch_pressed_events[event_index];
            GPIO_selectInterruptEdge(gpio_port, (1<<gpio_ith_pin), GPIO_LOW_TO_HIGH_TRANSITION);
        }
    }
    is_next_transition_high_to_low_switches[event_index] =
            !is_next_transition_high_to_low_switches[event_index];
    if(event != SWITCH_EVENT_DO_NOTHING) {
        // Switch De-bouncing
        clock_driver_time_t current_timestamp;
        clock_driver_get_rtc_time(&(current_timestamp));
        if(((int16_t)(current_timestamp.time_ms - last_event_timestamps[gpio_ith_pin].time_ms))
                >= GPIO_ISR_DEBOUNCING_TIME_MS) {

            // only care about ms
            last_event_timestamps[gpio_ith_pin].time_ms = current_timestamp.time_ms;

            (*event)();
        }
    }
}
