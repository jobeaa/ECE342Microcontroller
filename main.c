#include "driverlib.h"

#include "board.h"
#include "encoder_driver.h"

// Target frequency and actual frequency differ. Set target to just under 16Mhz for
// MCLK AND SMCLK
#define TARGET_SYS_CLK_FREQ_KHZ     15600
#define XT1_FREQ_HZ                 32768
#define FLL_REFERENCE_FREQ_KHZ      (uint32_t)(XT1_FREQ_HZ/1000)

uint32_t rtc_overflow_count = 0;

encoder_driver_t encoder1;

int main(void)
{
    //Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);

    // On-board LEDs
    GPIO_setAsOutputPin(ONBOARD_LED1_GPIO_PORT, ONBOARD_LED1_GPIO_PIN);
    GPIO_setAsOutputPin(ONBOARD_LED2_GPIO_PORT, ONBOARD_LED2_GPIO_PIN);
    GPIO_setOutputLowOnPin(ONBOARD_LED1_GPIO_PORT, ONBOARD_LED1_GPIO_PIN);
    GPIO_setOutputLowOnPin(ONBOARD_LED2_GPIO_PORT, ONBOARD_LED2_GPIO_PIN);

    // Clock Configuration
    // ---
    // Clock System
    // call this function so that we can use CS get_<NAME>CLK APIs
    CS_setExternalClockSource(XT1_FREQ_HZ);
    // Using DCO w/ FLL for stability. FLL reference is XT1 by default.
    // Block until setting MCLK & SMCLK to target frequency is successful;
    // program's timing requires operation at or close to target frequency.
    while(!CS_initFLL(TARGET_SYS_CLK_FREQ_KHZ,
                  (TARGET_SYS_CLK_FREQ_KHZ/FLL_REFERENCE_FREQ_KHZ))){

    }
    // Real Time Clock (RTC) Configuration
    // configure clock's modulus "overflow condition" to be as large as possible
    RTC_init(RTC_BASE, 0xFFFF, RTC_CLOCKPREDIVIDER_1);
    RTC_clearInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT);
    RTC_enableInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT);
    RTC_start(RTC_BASE, RTC_CLOCKSOURCE_SMCLK);

    // Motor Controllers
    encoder1.signal_a_gpio_port = ENCODER_1_SIGNAL_A_GPIO_PORT;
    encoder1.signal_a_gpio_pin = ENCODER_1_SIGNAL_A_GPIO_PIN;
    encoder1.signal_b_gpio_port = ENCODER_1_SIGNAL_B_GPIO_PORT;
    encoder1.signal_b_gpio_pin = ENCODER_1_SIGNAL_B_GPIO_PIN;
    ring_buffer_t encoder1_ring_buffer;
    char encoder1_ring_buffer_memory[128];
    ring_buffer_init(&encoder1_ring_buffer,
                     encoder1_ring_buffer_memory,
                     sizeof(encoder1_ring_buffer_memory));
    encoder1.data_buffer = &encoder1_ring_buffer;
    encoder_driver_open(&encoder1);

    // Servo Controller

    // Limit Switch Controllers


    // See comment in declaration. Used in all examples.
    PMM_unlockLPM5();

    // Global enable interrupts
    __enable_interrupt();


    // DEBUGGING RTC
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN3);
    // Main Loop
    uint32_t state = 0;
    while(1){
        if (state != rtc_overflow_count) {
            GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN3);
            state = rtc_overflow_count;
        }
    }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT1_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(PORT1_VECTOR)))
#endif
void P1_ISR (void)
{
    //LED1 = toggle
    GPIO_toggleOutputOnPin(ONBOARD_LED1_GPIO_PORT, ONBOARD_LED1_GPIO_PIN);

    //S1 IFG cleared
    GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN_ALL16);

    encoder_driver_signal_a_rising_edge_event(&encoder1);
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

