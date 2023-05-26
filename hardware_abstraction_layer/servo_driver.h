/*
 * servo_driver.h
 *
 *  Created on: May 7, 2023
 *      Author: Silas
 */

#ifndef SERVO_DRIVER_H_
#define SERVO_DRIVER_H_

#include "pwm_driver.h"

// positive rotation is counter-clock-wise
#define POSITION_MAX_DEGREES 180

typedef struct {
    pwm_driver_t pwm_driver;
} servo_driver_t;

// Initialize driver and underlying peripherals.
void servo_driver_open(servo_driver_t* driver);

// Instructs the servo to move to and hold the passed position.
// Parameters:
//      position_degrees:   position to move to in degrees. values greater than POSIITON_MAX_DEGREES
//                          will be 'truncated' to this upper bound.
void servo_driver_move_to(servo_driver_t* driver, uint16_t position_degrees);

#endif /* SERVO_DRIVER_H_ */
