/*
 * encoder_driver.c
 *
 *  Created on: May 10, 2023
 *      Author: silas
 */
#include "encoder_driver.h"
#include "gpio.h"


typedef struct {
    clock_driver_time_t time;
    uint8_t is_signal_b_high;
} encoder_reading_t;

bool encoder_driver_open(encoder_driver_t* driver){
    if(ring_buffer_get_allocated_size(driver->data_buffer) < (DATA_BUFFER_SIZE_MIN)) {
        return false;
    }

    GPIO_enableInterrupt(driver->signal_a_gpio_port, driver->signal_a_gpio_pin);
    GPIO_selectInterruptEdge(driver->signal_a_gpio_port, driver->signal_a_gpio_pin, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_clearInterrupt(driver->signal_a_gpio_port, driver->signal_a_gpio_pin);

    driver->pulse_count = 0;
    driver->instantaneous_velocity_pulse_per_ms = 0;
    driver->has_processed_a_reading = false;

    return true;
}

void encoder_driver_close(encoder_driver_t* driver){
    //TODO: how to uninit gpios?
    //      disable interrupt
}

void encoder_driver_update(encoder_driver_t* driver) {
    driver->delta_t.time_ms = 0;
    driver->delta_t.time_us = 0;

    // Check for new reading
    uint8_t readings_length = ring_buffer_num_items(driver->data_buffer) / sizeof(encoder_reading_t);
    if(!(readings_length - driver->has_processed_a_reading)) {
        return;
    }

    // Get new reading, leave last reading in buffer so subsequent calculations can find velocity
    // ---
    // GROSS: Can't do variable length array allocation. Hard code sensible value.
    encoder_reading_t readings[(DATA_BUFFER_SIZE_MIN/sizeof(encoder_reading_t))];
    ring_buffer_dequeue_arr(driver->data_buffer, (uint8_t* )(readings), ((readings_length - 1)*sizeof(encoder_reading_t)));
    uint8_t i;
    for(i = 0; i < sizeof(encoder_reading_t); i++) {
        ring_buffer_peek(driver->data_buffer, (uint8_t* )(&(readings[readings_length - 1])) + i, i);
    }

    // Update Pulse Count
    // count from the first non-peeked reading to the second to last reading
    for(i = driver->has_processed_a_reading; i < readings_length - 1; i++) {
        driver->pulse_count += (readings[i].is_signal_b_high == GPIO_INPUT_PIN_HIGH) ? -1 : 1;
    }
    int16_t second_last_reading_pulse_count = driver->pulse_count;
    // count the last reading
    driver->pulse_count += (readings[readings_length - 1].is_signal_b_high == GPIO_INPUT_PIN_HIGH) ? -1 : 1;

    // Update Pulse Count Time Delta
    if(readings_length > 1) {
        driver->delta_t.time_ms = readings[readings_length - 1].time.time_ms - readings[readings_length - 2].time.time_ms;
        driver->delta_t.time_us = readings[readings_length - 1].time.time_us - readings[readings_length - 2].time.time_us;
    }

    // Calculate velocity:
    if(driver->has_processed_a_reading) {
        const uint16_t last_index = readings_length - 1;
        const uint16_t second_last_index = last_index - 1;

        // (y2-y1)/(x2-x1)
        int32_t last_two_samples_delta_time_ms = ((int32_t)readings[last_index].time.time_ms - (int32_t)readings[second_last_index].time.time_ms) +
                                                 (((int32_t) (readings[last_index].time.time_us - readings[second_last_index].time.time_us))/1000);

        // BUG: sometimes the latest reading will be older than the second latest reading or the delta will blow up.
        //      This patch fixes the problem by ignoring such readings.
        if((last_two_samples_delta_time_ms < 0) || (last_two_samples_delta_time_ms > ((float)1000*60*60*24*2))) {
            //flush the buffer and return
            ring_buffer_dequeue_arr(driver->data_buffer, (uint8_t* )(readings), ring_buffer_get_allocated_size(driver->data_buffer));
            return;
        }

        driver->instantaneous_velocity_pulse_per_ms = (driver->pulse_count - second_last_reading_pulse_count) / last_two_samples_delta_time_ms;
    }

    driver->has_processed_a_reading = true;
}

inline float encoder_driver_get_position_degrees(encoder_driver_t* driver) {
    return (((float)driver->pulse_count) / PULSE_COUNT_PER_OUTPUT_REVOLUTION) * 360;
}

inline void encoder_driver_signal_a_rising_edge_event(encoder_driver_t* driver){
    // read data
    encoder_reading_t encoder_reading;
    encoder_reading.is_signal_b_high = GPIO_getInputPinValue(driver->signal_b_gpio_port, driver->signal_b_gpio_pin);
    clock_driver_get_rtc_time(&(encoder_reading.time));

    // enqueue data to buffer
    ring_buffer_queue_arr(driver->data_buffer, (uint8_t* )(&encoder_reading), sizeof(encoder_reading_t));
}
