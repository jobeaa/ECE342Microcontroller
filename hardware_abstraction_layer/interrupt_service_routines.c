/*
 * interrupt_service_routines.c
 *
 * Contains all ISRs for the micro controller allowing the HAL to manage details
 * and provide event based interface to application logic
 *
 *  Created on: May 26, 2023
 *      Author: silas
 */

#include "gpio.h"
#include "physical_switch_driver.h"

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT1_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(PORT1_VECTOR)))
#endif
void P1_ISR (void)
{
    uint16_t gpio_pin_interrupt_status = GPIO_getInterruptStatus(GPIO_PORT_P1, GPIO_PIN_ALL8);
    uint16_t i;
    for(i = 0; i < 8; i++) {
        if(gpio_pin_interrupt_status & (1<<i)) {
            // ith pin ISR
            GPIO_clearInterrupt(GPIO_PORT_P1, 1<<i);

            ISR_registered_physical_switch_event_dispatcher(GPIO_PORT_P1, i);
        }
    }

    //encoder_driver_signal_a_rising_edge_event(&encoder1);
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(PORT2_VECTOR)))
#endif
void P2_ISR (void)
{
    uint16_t gpio_pin_interrupt_status = GPIO_getInterruptStatus(GPIO_PORT_P2, GPIO_PIN_ALL8);
    uint16_t i;
    for(i = 0; i < 8; i++) {
        if(gpio_pin_interrupt_status & (1<<i)) {
            // ith pin ISR
            GPIO_clearInterrupt(GPIO_PORT_P2, 1<<i);

            ISR_registered_physical_switch_event_dispatcher(GPIO_PORT_P2, i);
        }
    }
}
