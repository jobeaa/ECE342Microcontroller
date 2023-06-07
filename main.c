#include "driverlib.h"

#include <stdbool.h>
#include <stdlib.h>

#include "board.h"
#include "clock_driver.h"
#include "servo_driver.h"
#include "motor_controller.h"

motor_controller_t motor_controller_1;
motor_controller_t motor_controller_2;

int main(void)
{
    // Stop Watchdog Timer
    WDT_A_hold(WDT_A_BASE);

    // On-board LEDs
    GPIO_setAsOutputPin(ONBOARD_LED1_GPIO_PORT, ONBOARD_LED1_GPIO_PIN);
    GPIO_setAsOutputPin(ONBOARD_LED2_GPIO_PORT, ONBOARD_LED2_GPIO_PIN);
    GPIO_setOutputLowOnPin(ONBOARD_LED1_GPIO_PORT, ONBOARD_LED1_GPIO_PIN);
    GPIO_setOutputLowOnPin(ONBOARD_LED2_GPIO_PORT, ONBOARD_LED2_GPIO_PIN);

    // Clock Configuration
    clock_driver_open();

    // Motor Controllers
    encoder_driver_t encoder1;
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

    motor_direct_driver_t motor1_direct_driver;
    motor1_direct_driver.motor_magnitude_pwm_driver.gpio_port =
            MOTOR_1_MAGNITUDE_GPIO_PORT;
    motor1_direct_driver.motor_magnitude_pwm_driver.gpio_pin =
            MOTOR_1_MAGNITUDE_GPIO_PIN;
    motor1_direct_driver.motor_magnitude_pwm_driver.gpio_timer_aX_channel_module_function =
            MOTOR_1_MAGNITUDE_GPIO_TIMER_AX_CHANNEL_MODULE_FUNCTION;
    motor1_direct_driver.motor_magnitude_pwm_driver.timer_aX_base_address =
            MOTOR_1_MAGNITUDE_TIMER_AX_BASE_ADDRESS;
    motor1_direct_driver.motor_magnitude_pwm_driver.timer_aX_channel_compare_register =
            MOTOR_1_MAGNITUDE_TIMER_AX_CAPCOMP_REG;
    motor1_direct_driver.motor_magnitude_pwm_driver.timer_aX_clock_source =
            TIMER_A_CLOCKSOURCE_SMCLK;
    motor1_direct_driver.motor_magnitude_pwm_driver.timer_aX_clock_source_divider =
            TIMER_A_CLOCKSOURCE_DIVIDER_1;
    motor1_direct_driver.motor_magnitude_pwm_driver.timer_aX_period = (UINT16_MAX/2);
    pwm_driver_open(&(motor1_direct_driver.motor_magnitude_pwm_driver));
    motor1_direct_driver.motor_direction_enable_1_gpio_port = MOTOR_1_DIRECTION_ENABLE_1_GPIO_PORT;
    motor1_direct_driver.motor_direction_enable_1_gpio_pin = MOTOR_1_DIRECTION_ENABLE_1_GPIO_PIN;
    motor1_direct_driver.motor_direction_enable_2_gpio_port = MOTOR_1_DIRECTION_ENABLE_2_GPIO_PORT;
    motor1_direct_driver.motor_direction_enable_2_gpio_pin = MOTOR_1_DIRECTION_ENABLE_2_GPIO_PIN;
    motor_direct_driver_open(&motor1_direct_driver);

    pid_controller_t motor1_pid_controller;
    motor1_pid_controller.proportion_gain = 12.0f;
    motor1_pid_controller.integration_gain = 5.0f;
    motor1_pid_controller.derivative_gain = 9000.0f;
    motor1_pid_controller.output_limit_max = 1024.0f;
    motor1_pid_controller.output_limit_min = -1024.0f;
    motor1_pid_controller.integral_limit_max = 512.0f;
    motor1_pid_controller.integral_limit_min = -512.0f;
    pid_controller_init(&motor1_pid_controller);

    motor_controller_1.encoder_driver = &encoder1;
    motor_controller_1.motor_driver = &motor1_direct_driver;
    motor_controller_1.pid_controller = &motor1_pid_controller;

    encoder_driver_t encoder2;
    encoder2.signal_a_gpio_port = ENCODER_2_SIGNAL_A_GPIO_PORT;
    encoder2.signal_a_gpio_pin = ENCODER_2_SIGNAL_A_GPIO_PIN;
    encoder2.signal_b_gpio_port = ENCODER_2_SIGNAL_B_GPIO_PORT;
    encoder2.signal_b_gpio_pin = ENCODER_2_SIGNAL_B_GPIO_PIN;
    ring_buffer_t encoder2_ring_buffer;
    uint8_t encoder2_ring_buffer_memory[256];
    ring_buffer_init(&encoder2_ring_buffer,
                     encoder2_ring_buffer_memory,
                     sizeof(encoder2_ring_buffer_memory));
    encoder2.data_buffer = &encoder2_ring_buffer;
    encoder_driver_open(&encoder2);

    motor_direct_driver_t motor2_direct_driver;
    motor2_direct_driver.motor_magnitude_pwm_driver.gpio_port =
            MOTOR_2_MAGNITUDE_GPIO_PORT;
    motor2_direct_driver.motor_magnitude_pwm_driver.gpio_pin =
            MOTOR_2_MAGNITUDE_GPIO_PIN;
    motor2_direct_driver.motor_magnitude_pwm_driver.gpio_timer_aX_channel_module_function =
            MOTOR_2_MAGNITUDE_GPIO_TIMER_AX_CHANNEL_MODULE_FUNCTION;
    motor2_direct_driver.motor_magnitude_pwm_driver.timer_aX_base_address =
            MOTOR_2_MAGNITUDE_TIMER_AX_BASE_ADDRESS;
    motor2_direct_driver.motor_magnitude_pwm_driver.timer_aX_channel_compare_register =
            MOTOR_2_MAGNITUDE_TIMER_AX_CAPCOMP_REG;
    motor2_direct_driver.motor_magnitude_pwm_driver.timer_aX_clock_source =
            TIMER_A_CLOCKSOURCE_SMCLK;
    motor2_direct_driver.motor_magnitude_pwm_driver.timer_aX_clock_source_divider =
            TIMER_A_CLOCKSOURCE_DIVIDER_1;
    motor2_direct_driver.motor_magnitude_pwm_driver.timer_aX_period = (UINT16_MAX/2);
    pwm_driver_open(&(motor2_direct_driver.motor_magnitude_pwm_driver));
    motor2_direct_driver.motor_direction_enable_1_gpio_port = MOTOR_2_DIRECTION_ENABLE_1_GPIO_PORT;
    motor2_direct_driver.motor_direction_enable_1_gpio_pin = MOTOR_2_DIRECTION_ENABLE_1_GPIO_PIN;
    motor2_direct_driver.motor_direction_enable_2_gpio_port = MOTOR_2_DIRECTION_ENABLE_2_GPIO_PORT;
    motor2_direct_driver.motor_direction_enable_2_gpio_pin = MOTOR_2_DIRECTION_ENABLE_2_GPIO_PIN;
    motor_direct_driver_open(&motor2_direct_driver);

    pid_controller_t motor2_pid_controller;
    motor2_pid_controller.proportion_gain = 16.0f;
    motor2_pid_controller.integration_gain = 5.0f;
    motor2_pid_controller.derivative_gain = 10000.0f;
    motor2_pid_controller.output_limit_max = 1024.0f;
    motor2_pid_controller.output_limit_min = -1024.0f;
    motor2_pid_controller.integral_limit_max = 512.0f;
    motor2_pid_controller.integral_limit_min = -512.0f;
    pid_controller_init(&motor2_pid_controller);

    motor_controller_2.encoder_driver = &encoder2;
    motor_controller_2.motor_driver = &motor2_direct_driver;
    motor_controller_2.pid_controller = &motor2_pid_controller;

    // Servo Controller
    servo_driver_t writing_utensil_servo;
    writing_utensil_servo.pwm_driver.gpio_port =
            WRITING_UTENSIL_SERVO_SIGNAL_GPIO_PORT;
    writing_utensil_servo.pwm_driver.gpio_pin =
            WRITING_UTENSIL_SERVO_SIGNAL_GPIO_PIN;
    writing_utensil_servo.pwm_driver.gpio_timer_aX_channel_module_function =
            WRITING_UTENSIL_SERVO_SIGNAL_GPIO_TIMER_AX_CHANNEL_MODULE_FUNCTION;
    writing_utensil_servo.pwm_driver.timer_aX_base_address =
            WRITING_UTENSIL_SERVO_SIGNAL_TIMER_AX_BASE_ADDRESS;
    writing_utensil_servo.pwm_driver.timer_aX_channel_compare_register =
            WRITING_UTENSIL_SERVO_SIGNAL_TIMER_AX_CAPCOMP_REG;
    writing_utensil_servo.pwm_driver.timer_aX_clock_source =
            TIMER_A_CLOCKSOURCE_ACLK;
    writing_utensil_servo.pwm_driver.timer_aX_clock_source_divider =
            TIMER_A_CLOCKSOURCE_DIVIDER_1;
    writing_utensil_servo.pwm_driver.timer_aX_period = 400;
    servo_driver_open(&writing_utensil_servo);

    // Limit Switch Controllers


    // See comment in declaration. Used in all examples.
    PMM_unlockLPM5();

    // Global enable interrupts
    __enable_interrupt();

    GPIO_setOutputHighOnPin(ONBOARD_LED1_GPIO_PORT, ONBOARD_LED1_GPIO_PIN);

    // DEBUG: test servo movement
    servo_driver_move_to(&writing_utensil_servo, 0);

//    uint16_t i = 0;
//    for(i = 1; i < 10; i++) {
//        motor_controller_single_motor_move_to(&motor_controller_2, i*60, 0);
//        float position_degrees = encoder_driver_get_position_degrees(motor_controller_2.encoder_driver);
//    }

//    uint16_t i = 0;
//    for(i = 1; i < 10; i++) {
//        motor_controller_single_motor_move_to(&motor_controller_1, i*60, 0);
//        float position_degrees = encoder_driver_get_position_degrees(motor_controller_1.encoder_driver);
//    }

    uint16_t i = 0;
    for(i = 1; i < 10; i++) {
        motor_controller_move_to(&motor_controller_1, &motor_controller_2, 0, i*10, 0, 0);
    }
    while(1){
        encoder_driver_update(motor_controller_1.encoder_driver);

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

    encoder_driver_signal_a_rising_edge_event(motor_controller_1.encoder_driver);
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(PORT2_VECTOR)))
#endif
void P2_ISR (void)
{
    //S2 IFG cleared
    GPIO_clearInterrupt(GPIO_PORT_P2, GPIO_PIN_ALL16);

    encoder_driver_signal_a_rising_edge_event(motor_controller_2.encoder_driver);
}
