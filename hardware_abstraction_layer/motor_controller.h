/*
 * motor_controller.h
 *
 *  Created on: May 7, 2023
 *      Author: Silas
 */

#ifndef MOTOR_CONTROLLER_H_
#define MOTOR_CONTROLLER_H_

typedef struct {

} motor_controller_t;

// Initialize controller and underlying peripherals.
void motor_controller_open(motor_controller_t controller);

// Deinitialize controller and underlying peripherals.
void motor_controller_close(motor_controller_t controller);

// Sets the controller's internal state. Intended to be called when the motor reaches a known
// position such as when a limit switch is pressed.
void motor_controller_set_angular_position(motor_controller_t controller, float position_degrees);

// Instructs the controller to move the motor towards the passed position. The controller will try
// to move at the speed indicated.
//
// DISCLAIMER:      This function will likely change. There needs to be a way for the caller to
//                  be notified when the motor is at the passed position (within some error). I also
//                  need to think more about this implementation. Aaron and I need to think through
//                  the arm controller to see if this implementation makes sense and is feasible. I have
//                  a feeling currently, any movement would need to be sliced into pieces in time. Since
//                  the speed at which the motor is moving cannot be accurately controlled. So we break a large
//                  move into a bunch of smaller moves. Moves are partitioned in time.
void motor_controller_move_to(motor_controller_t controller, float position_degrees, float speed_degrees_per_second);

#endif /* MOTOR_CONTROLLER_H_ */
