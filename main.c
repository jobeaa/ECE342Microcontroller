#include "driverlib.h"

#include "board.h"
#include "encoder_driver.h"
#include "clock_driver.h"

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
    clock_driver_open();

    // Motor Controllers
    encoder1.signal_a_gpio_port = ENCODER_1_SIGNAL_A_GPIO_PORT;
    encoder1.signal_a_gpio_pin = ENCODER_1_SIGNAL_A_GPIO_PIN;
    encoder1.signal_b_gpio_port = ENCODER_1_SIGNAL_B_GPIO_PORT;
    encoder1.signal_b_gpio_pin = ENCODER_1_SIGNAL_B_GPIO_PIN;
    ring_buffer_t encoder1_ring_buffer;
    uint8_t encoder1_ring_buffer_memory[64];
    ring_buffer_init(&encoder1_ring_buffer,
                     encoder1_ring_buffer_memory,
                     sizeof(encoder1_ring_buffer_memory));
    encoder1.data_buffer = &encoder1_ring_buffer;
    encoder_driver_open(&encoder1);

    // Servo Controller

    // Limit Switch Controllers

    // See comment in declaration. Used in all examples.
    PMM_unlockLPM5();

    // Test with single pulse:
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN6);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);

    // Global enable interrupts
    __enable_interrupt();

    while(1){
        encoder_driver_calculate_kinematics(&encoder1);
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
    //S1 IFG cleared
    GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN_ALL16);

    //LED1 = toggle
    GPIO_toggleOutputOnPin(ONBOARD_LED1_GPIO_PORT, ONBOARD_LED1_GPIO_PIN);

    encoder_driver_signal_a_rising_edge_event(&encoder1);
}
