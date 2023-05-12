/*
 * clock_driver.h
 *
 *  Created on: May 12, 2023
 *      Author: silas
 */

#ifndef CLOCK_DRIVER_H_
#define CLOCK_DRIVER_H_

// Represents a point in time. time_ms + time_us gives
// microsecond precision time from when clock driver was opened.
// Overflows after ~12 days.
typedef struct {
    uint32_t time_ms;
    uint16_t time_us;
} clock_driver_time_t;

// Initialize clock driver underlying peripherals:
//  - Clock System (CS)
//  - Real Time Clock (RTC)
// This function blocks until setting MCLK & SMCLK to target
// frequency is successful.
void clock_driver_open(void);

// gets the current rtc time. does not take into account the
// execution from when the data is read to when it is returned.
// Maximum estimated error is
// atomic function disables interrupts while executing.
void clock_driver_atomic_get_rtc_time(clock_driver_time_t* time);

#endif /* CLOCK_DRIVER_H_ */
