/*
 * servo_driver.h
 *
 *  Created on: May 7, 2023
 *      Author: Silas
 */

#ifndef SERVO_DRIVER_H_
#define SERVO_DRIVER_H_

typedef struct {
    motor_controller_t motor_controller;
} servo_driver_t;

// Initialize driver and underlying peripherals.
void servo_driver_open(servo_driver_t* driver);

// Adjusts PWM servo control signal to match passed position; the servo
// will try to go to and hold this position producing the necessary torques
// to do so.
// Position_degrees <= 200; higher values will be truncated to this upper bound.
void servo_driver_move_to(servo_driver_t* driver, uint16_t position_degrees);

#endif /* SERVO_DRIVER_H_ */
