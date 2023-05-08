/*
 * limit_switch_controller.h
 *
 *  Created on: May 8, 2023
 *      Author: silas
 */

#ifndef LIMIT_SWITCH_CONTROLLER_H_
#define LIMIT_SWITCH_CONTROLLER_H_

typedef struct {
    // The ISR called when the limit switch is pressed.
    void (*interupt_service_routine)(void);
} limit_switch_controller_t;

// Initialize controller and underlying peripherals.
limit_switch_controller_open(limit_switch_controller_t controller);

// Deinitialize controller and underlying peripherals.
limit_switch_controller_close(limit_switch_controller_t controller);


#endif /* LIMIT_SWITCH_CONTROLLER_H_ */
