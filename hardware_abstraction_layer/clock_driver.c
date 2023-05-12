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
#define TARGET_SYS_CLK_FREQ_KHZ     15600
#define XT1_FREQ_HZ                 32768
#define FLL_REFERENCE_FREQ_KHZ      (uint32_t)(XT1_FREQ_HZ/1000)
#define RTC_MODULUS                 0xFFFF      // set the modulus as large as possible
                                                // to minimize RTC interrupts.

static uint32_t rtc_overflow_count = 0;
static uint32_t smclk_freq_khz = 0;

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
    RTC_clearInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT);
    RTC_enableInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT);
    RTC_start(RTC_BASE, RTC_CLOCKSOURCE_SMCLK);

    smclk_freq_khz = CS_getSMCLK() / 1000;
}

void clock_driver_atomic_get_rtc_time(clock_driver_time_t* time) {
    // prevent rtc_overflow_count from incrementing while reading time
    do {
        time->time_ms = rtc_overflow_count;
        time->time_us = RTCCNT;
    } while(time->time_ms != rtc_overflow_count);
    // NOTE this causes RTC to accumulate an error of <6 clock cycles
    // (time it takes to copy rtc_overflow_count) (<0.3us) (<10/65535
    // or <0.015%) every time this function is called.
    //__disable_interrupt();
    //time->time_us = RTCCNT;
    //time->time_ms = rtc_overflow_count;
    //__enable_interrupt();

    time->time_ms = ((RTC_MODULUS)/smclk_freq_khz) * time->time_ms;
    time->time_ms += time->time_us / 1000;
    time->time_us = time->time_us % 1000;
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
