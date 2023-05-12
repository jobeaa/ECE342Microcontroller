/*
 * encoder_driver.c
 *
 *  Created on: May 10, 2023
 *      Author: silas
 */
#include "encoder_driver.h"
#include "gpio.h"
#include "clock_driver.h"

typedef struct {
    clock_driver_time_t time;
    uint8_t is_signal_b_high;
} encoder_reading_t;

void encoder_driver_open(encoder_driver_t* driver){
    GPIO_setAsInputPinWithPullDownResistor(driver->signal_a_gpio_port, driver->signal_a_gpio_pin);
    GPIO_setAsInputPinWithPullDownResistor(driver->signal_b_gpio_port, driver->signal_b_gpio_pin);

    GPIO_enableInterrupt(driver->signal_a_gpio_port, driver->signal_a_gpio_pin);
    GPIO_selectInterruptEdge(driver->signal_a_gpio_port, driver->signal_a_gpio_pin, GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_clearInterrupt(driver->signal_a_gpio_port, driver->signal_a_gpio_pin);

}

void encoder_driver_close(encoder_driver_t* driver){
    //TODO: how to uninit gpios?
    //      disable interrupt
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
