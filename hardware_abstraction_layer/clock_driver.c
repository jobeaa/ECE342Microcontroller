/*
 * clock_driver.c
 *
 *  Created on: May 12, 2023
 *      Author: silas
 */
#include "cs.h"
#include "rtc.h"

#include "clock_driver.h"

// Target frequency and actual frequency differ. Set target to just under 16Mhz for
// MCLK AND SMCLK
#define TARGET_SYS_CLK_FREQ_KHZ     7810
#define XT1_FREQ_HZ                 32768
#define FLL_REFERENCE_FREQ_KHZ      (uint32_t)(XT1_FREQ_HZ/1000)
#define RTC_MODULUS                 0xFFFF      // set the modulus as large as possible
                                                // to minimize RTC interrupts.

static volatile uint32_t rtc_overflow_count = 0;
static uint16_t smclk_freq_khz = 0;
static uint32_t overflow_period_us = 0;

void clock_driver_open(void) {
    // call this function so that we can use CS get_<NAME>CLK APIs
    CS_setExternalClockSource(XT1_FREQ_HZ);
    // Using DCO w/ FLL for stability. FLL reference is XT1 by default.
    // Block until setting MCLK & SMCLK to target frequency is successful;
    // program's timing requires operation at or close to target frequency.
    while(!CS_initFLL(TARGET_SYS_CLK_FREQ_KHZ,
                  (TARGET_SYS_CLK_FREQ_KHZ/FLL_REFERENCE_FREQ_KHZ))){

    }
    // Real Time Clock (RTC) Configuration
    RTC_init(RTC_BASE, RTC_MODULUS, RTC_CLOCKPREDIVIDER_1);
    RTC_clearInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT_FLAG);
    RTC_enableInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT);
    RTC_start(RTC_BASE, RTC_CLOCKSOURCE_SMCLK);

    smclk_freq_khz = CS_getSMCLK() / 1000;
    overflow_period_us = (((float) (RTC_MODULUS)) / ((float) smclk_freq_khz)) * 1000;
}

inline void clock_driver_get_rtc_time(clock_driver_time_t* time) {
    uint32_t current_rtc_overflow_count = rtc_overflow_count;
    uint32_t current_rtc_count = RTCCNT;

    // If rtc overflow ISR occured between now and when the overflow counter was read
    if(rtc_overflow_count > current_rtc_overflow_count) {
        // check if the overflow counter should be decremented.
        if(current_rtc_count < (0.5 * RTC_MODULUS)) {
            current_rtc_overflow_count--;
        }
    }

    // add overflowed-counter time [ms]
    time->time_ms = (current_rtc_overflow_count * overflow_period_us) / 1000;
    // add unoverflowed-counter time [ms]
    time->time_ms += (current_rtc_count / smclk_freq_khz);

    // get unoverflowed-counter time [us], but reduce it to less than 1000, because we've already
    // counted such values in time_ms.
    time->time_us = ((current_rtc_count * 1000) / smclk_freq_khz) % 1000;
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=RTC_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(RTC_VECTOR)))
#endif
void RTC_ISR (void)
{
    switch (RTCIV){
        // No interrupt
        case 0:
            break;
        // RTC overflow
        case 2:
            rtc_overflow_count++;
            break;
        default:
            break;
    }
}
