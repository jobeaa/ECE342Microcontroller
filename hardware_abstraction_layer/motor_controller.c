/*
 * motor_controller.c
 *
 *  Created on: Jun 6, 2023
 *      Author: silas
 */

#include <stdlib.h>

#include "motor_controller.h"

void motor_controller_set_angular_position(motor_controller_t *controller, float position_degrees) {

}

void motor_controller_single_motor_move_to(motor_controller_t *controller, int16_t set_point, float speed_motor_degrees_per_second) {
    const uint16_t acceptable_position_error_pulses = 6;

    uint16_t current_position_within_error_consecutive_count = 0;
    while(1) {
        encoder_driver_update(controller->encoder_driver);

        controller->pid_controller->sample_time_ms = (((float)controller->encoder_driver->delta_t.time_ms) +
                ((float)controller->encoder_driver->delta_t.time_us)/1000.0f);

        float pid_output = pid_controller_update(controller->pid_controller, set_point, controller->encoder_driver->pulse_count);
        motor_direct_driver_set_output(controller->motor_driver, pid_output);

        // TODO: Experiement with exit conditionnnnnnn
        int16_t position_error_pulses = abs(set_point - controller->encoder_driver->pulse_count);
        if(position_error_pulses < acceptable_position_error_pulses) {
            current_position_within_error_consecutive_count++;
        }
        else {
            current_position_within_error_consecutive_count = 0;
        }
        if(current_position_within_error_consecutive_count > 10) {
            motor_direct_driver_set_output(controller->motor_driver, 0.0f);
            return;
        }
    }
}


void motor_controller_move_to(motor_controller_t *controller1, motor_controller_t *controller2,
                              float position_degrees1, float position_degrees2,
                              float speed_motor1_degrees_per_second, float speed_motor2_degrees_per_second) {
    float set_point1 = position_degrees1 * (PULSE_COUNT_PER_OUTPUT_REVOLUTION / 360),
            set_point2 = position_degrees2 * (PULSE_COUNT_PER_OUTPUT_REVOLUTION / 360);

    const uint16_t acceptable_position_error_pulses = 15;

    uint16_t current_position_within_error_consecutive_count1 = 0,
            current_position_within_error_consecutive_count2 = 0;

    while(1) {
        encoder_driver_update(controller1->encoder_driver);
        encoder_driver_update(controller2->encoder_driver);


        controller1->pid_controller->sample_time_ms = (((float)controller1->encoder_driver->delta_t.time_ms) +
                ((float)controller1->encoder_driver->delta_t.time_us)/1000.0f);
        controller2->pid_controller->sample_time_ms = (((float)controller2->encoder_driver->delta_t.time_ms) +
                ((float)controller2->encoder_driver->delta_t.time_us)/1000.0f);

        float pid_output = pid_controller_update(controller1->pid_controller, set_point1, controller1->encoder_driver->pulse_count);
        motor_direct_driver_set_output(controller1->motor_driver, pid_output);

        pid_output = pid_controller_update(controller2->pid_controller, set_point2, controller2->encoder_driver->pulse_count);
        motor_direct_driver_set_output(controller2->motor_driver, pid_output);

        // TODO: Experiement with exit conditionnnnnnn
        int16_t position_error_pulses1 = abs(set_point1 - controller1->encoder_driver->pulse_count),
          position_error_pulses2 = abs(set_point2 - controller2->encoder_driver->pulse_count);
        if(position_error_pulses1 < acceptable_position_error_pulses) {
            current_position_within_error_consecutive_count1++;
        }
        else {
            current_position_within_error_consecutive_count1 = 0;
        }
        if(position_error_pulses2 < acceptable_position_error_pulses) {
            current_position_within_error_consecutive_count2++;
        }
        else {
            current_position_within_error_consecutive_count2 = 0;
        }
        if(current_position_within_error_consecutive_count1 > 10 && current_position_within_error_consecutive_count2 > 10) {
            motor_direct_driver_set_output(controller1->motor_driver, 0.0f);
            motor_direct_driver_set_output(controller2->motor_driver, 0.0f);
            return;
        }
    }

}
