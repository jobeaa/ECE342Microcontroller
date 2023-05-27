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
    //S1 IFG cleared
    GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN_ALL16);

    ISR_registered_physical_switch_event_dispatcher(GPIO_PORT_P1);

    //encoder_driver_signal_a_rising_edge_event(&encoder1);
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(PORT8_VECTOR)))
#endif
void P2_ISR (void)
{
    //S1 IFG cleared
    GPIO_clearInterrupt(GPIO_PORT_P2, GPIO_PIN_ALL16);

    ISR_registered_physical_switch_event_dispatcher(GPIO_PORT_P2);
}
