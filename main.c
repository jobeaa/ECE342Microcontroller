#include "driverlib.h"

#include "board.h"
#include "encoder_driver.h"
#include "clock_driver.h"
#include "motor_direct_driver.h"
#include "servo_driver.h"
#include "pid_controller.h"

encoder_driver_t encoder1;
motor_direct_driver_t motor1;

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

    motor1.motor_magnitude_pwm_driver.gpio_port =
            MOTOR_1_MAGNITUDE_GPIO_PORT;
    motor1.motor_magnitude_pwm_driver.gpio_pin =
            MOTOR_1_MAGNITUDE_GPIO_PIN;
    motor1.motor_magnitude_pwm_driver.gpio_timer_aX_channel_module_function =
            MOTOR_1_MAGNITUDE_GPIO_TIMER_AX_CHANNEL_MODULE_FUNCTION;
    motor1.motor_magnitude_pwm_driver.timer_aX_base_address =
            MOTOR_1_MAGNITUDE_TIMER_AX_BASE_ADDRESS;
    motor1.motor_magnitude_pwm_driver.timer_aX_channel_compare_register =
            MOTOR_1_MAGNITUDE_TIMER_AX_CAPCOMP_REG;
    motor1.motor_magnitude_pwm_driver.timer_aX_clock_source =
            TIMER_A_CLOCKSOURCE_SMCLK;
    motor1.motor_magnitude_pwm_driver.timer_aX_clock_source_divider =
            TIMER_A_CLOCKSOURCE_DIVIDER_1;
    motor1.motor_magnitude_pwm_driver.timer_aX_period = (UINT16_MAX/2);
    pwm_driver_open(&(motor1.motor_magnitude_pwm_driver));
    motor1.motor_direction_enable_1_gpio_port = MOTOR_1_DIRECTION_ENABLE_1_GPIO_PORT;
    motor1.motor_direction_enable_1_gpio_pin = MOTOR_1_DIRECTION_ENABLE_1_GPIO_PIN;
    motor1.motor_direction_enable_2_gpio_port = MOTOR_1_DIRECTION_ENABLE_2_GPIO_PORT;
    motor1.motor_direction_enable_2_gpio_pin = MOTOR_1_DIRECTION_ENABLE_2_GPIO_PIN;
    motor_direct_driver_open(&motor1);

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

    // DEBUG: test servo movement
    servo_driver_move_to(&writing_utensil_servo, 0);


    // TODO: move to motor controller module
    pid_controller_t motor1_pid_controller;
    motor1_pid_controller.proportion_gain = 0.011f;
    motor1_pid_controller.integration_gain = 0.011f;
    motor1_pid_controller.derivative_gain = 0.01f;
    motor1_pid_controller.output_limit_max = 1024.0f;
    motor1_pid_controller.output_limit_min = -1024.0f;
    motor1_pid_controller.integral_limit_max = 20.0f;
    motor1_pid_controller.integral_limit_min = 20.0f;
    pid_controller_init(&motor1_pid_controller);

    // wait for some time to stablize power?
    clock_driver_time_t current_time, initial_time;
    clock_driver_get_rtc_time(&current_time);
    clock_driver_get_rtc_time(&initial_time);
    while((current_time.time_ms - initial_time.time_ms) < 10000) {
        clock_driver_get_rtc_time(&current_time);
    }
    GPIO_setOutputHighOnPin(ONBOARD_LED1_GPIO_PORT, ONBOARD_LED1_GPIO_PIN);


    while(1){
        encoder_driver_calculate_kinematics(&encoder1);
        // inject derivative from encoder driver into pid controller
        motor1_pid_controller.differentiator = encoder1.instantaneous_velocity_pulse_per_ms;
        motor1_pid_controller.sample_time_seconds = encoder1.delta_t.time_ms + (encoder1.delta_t.time_us/1000);
        float output = pid_controller_update(&motor1_pid_controller, 500, encoder1.pulse_count);
        motor_direct_driver_set_output(&motor1, output);
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

    //encoder_driver_signal_a_rising_edge_event(&encoder1);
}
