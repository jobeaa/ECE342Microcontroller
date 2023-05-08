/*
 * servo_controller.h
 *
 *  Created on: May 7, 2023
 *      Author: Silas
 */

#ifndef SERVO_CONTROLLER_H_
#define SERVO_CONTROLLER_H_

typedef struct {

} servo_controller_t;

// Initialize controller and underlying peripherals.
servo_controller_open(servo_controller_t controller);

// Deinitialize controller and underlying peripherals.
servo_controller_close(servo_controller_t controller);

// Adjusts PWM servo control signal to match passed position; the servo
// will try to go to and hold this position producing the necessary torques
// to do so.
servo_controller_move_to(servo_controller_t position_degrees);

#endif /* SERVO_CONTROLLER_H_ */
