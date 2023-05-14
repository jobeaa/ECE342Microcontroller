/*
 * encoder_driver.c
 *
 *  Created on: May 10, 2023
 *      Author: silas
 */
#include "encoder_driver.h"
#include "gpio.h"
#include "clock_driver.h"

#define PULSES_PER_MOTOR_SHAFT_REVOLUTION   16
#define MOTOR_SHAFT_TO_OUTPUT_SHAFT_RATIO   (131/1)
#define PULSES_PER_OUTPUT_SHAFT_REVOLUTION  (PULSES_PER_MOTOR_SHAFT_REVOLUTION * MOTOR_SHAFT_TO_OUTPUT_SHAFT_RATIO)
#define PULSES_PER_DEGREE_OUTPUT_SHAFT      (PULSES_PER_OUTPUT_SHAFT_REVOLUTION / 360)

typedef struct {
    clock_driver_time_t time;
    uint8_t is_signal_b_high;
} encoder_reading_t;

void encoder_driver_open(encoder_driver_t* driver){
    GPIO_setAsInputPinWithPullDownResistor(driver->signal_a_gpio_port, driver->signal_a_gpio_pin);
    GPIO_setAsInputPinWithPullDownResistor(driver->signal_b_gpio_port, driver->signal_b_gpio_pin);

    //GPIO_enableInterrupt(driver->signal_a_gpio_port, driver->signal_a_gpio_pin);
    //GPIO_selectInterruptEdge(driver->signal_a_gpio_port, driver->signal_a_gpio_pin, GPIO_LOW_TO_HIGH_TRANSITION);
    //GPIO_clearInterrupt(driver->signal_a_gpio_port, driver->signal_a_gpio_pin);

    driver->pulse_count = 0;
    driver->instantaneous_velocity_pulse_per_usec = 0;
    driver->has_processed_a_reading = false;
}

void encoder_driver_close(encoder_driver_t* driver){
    //TODO: how to uninit gpios?
    //      disable interrupt
}

void encoder_driver_calculate_kinematics(encoder_driver_t* driver) {
    uint8_t readings_length = ring_buffer_num_items(driver->data_buffer) / sizeof(encoder_reading_t);
    // return if there isn't a new reading
    if(!(readings_length - driver->has_processed_a_reading)) {
        return;
    }
    encoder_reading_t readings[4];

    // always leave 1 reading at the end of the buffer for calculating velocity on the next call of this function
    ring_buffer_dequeue_arr(driver->data_buffer, (uint8_t* )(readings), (readings_length - 1)*sizeof(encoder_reading_t));
    uint8_t i;
    for(i = 0; i < sizeof(encoder_reading_t); i++) {
        ring_buffer_peek(driver->data_buffer, (uint8_t* )(&(readings[readings_length - 1])) + i, i);
    }


    // count pulses
    // count a reading only once; ensure the most current reading is counted (ie the last reading in the buffer).
    // count from the first non-peeked reading to the second to last reading
    for(i = driver->has_processed_a_reading; i < readings_length - 1; i++) {
        driver->pulse_count += 1 - (2 * readings[i].is_signal_b_high);   // either 1 or -1 depending on signal b high
    }
    uint16_t second_last_reading_pulse_count = driver->pulse_count;
    // count the last reading
    driver->pulse_count += 1 - (2 * readings[readings_length - 1].is_signal_b_high);   // either 1 or -1 depending on signal b high

    // calculate velocity if we have 2 readings to work with.
    if(driver->has_processed_a_reading) {
        const uint16_t last_index = readings_length - 1;
        const uint16_t second_last_index = last_index - 1;
        // (y2-y1)/(x2-x1)
        driver->instantaneous_velocity_pulse_per_usec = (driver->pulse_count - second_last_reading_pulse_count) /
                (
                        1000 * (readings[last_index].time.time_ms - readings[second_last_index].time.time_ms) +
                        readings[last_index].time.time_us - readings[second_last_index].time.time_us
                );
    }

    driver->has_processed_a_reading = true;
}

inline float encoder_driver_get_position_degrees(encoder_driver_t* driver) {
    return driver->pulse_count / PULSES_PER_DEGREE_OUTPUT_SHAFT;
}

inline void encoder_driver_signal_a_rising_edge_event(encoder_driver_t* driver){
    // read data
    encoder_reading_t encoder_reading;
    encoder_reading.is_signal_b_high =GPIO_getInputPinValue(driver->signal_b_gpio_port,
                                                             driver->signal_b_gpio_pin);
    clock_driver_get_rtc_time(&(encoder_reading.time));

    // enqueue data to buffer
    ring_buffer_queue_arr(driver->data_buffer, (uint8_t* )(&encoder_reading), sizeof(encoder_reading_t));
}
