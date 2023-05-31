#include <hardware_abstraction_layer/ti_driverlib/MSP430FR2xx_4xx/driverlib.h>
#include <interpreter/receiver.h>
#include <interpreter/commands.h>
#include <interpreter/lcd_msg.h>

#include "driverlib.h"

#include "board.h"
#include "encoder_driver.h"
#include "clock_driver.h"
#include "motor_direct_driver.h"
#include "servo_driver.h"
#include "physical_switch_driver.h"

encoder_driver_t encoder1;
motor_direct_driver_t motor1;

void limit_switch1_event(void) {
    GPIO_toggleOutputOnPin(ONBOARD_LED1_GPIO_PORT, ONBOARD_LED1_GPIO_PIN);
}

void pushbutton_s2_event(void) {
    GPIO_toggleOutputOnPin(ONBOARD_LED2_GPIO_PORT, ONBOARD_LED2_GPIO_PIN);
}

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

    motor1.motor_magnitude_gpio_port = MOTOR_1_MAGNITUDE_GPIO_PORT;
    motor1.motor_magnitude_gpio_pin = MOTOR_1_MAGNITUDE_GPIO_PIN;
    motor1.timer_aX_base_address = MOTOR_1_MAGNITUDE_TIMER_AX_BASE_ADDRESS;
    motor1.timer_aX_channel_compare_register = MOTOR_1_MAGNITUDE_TIMER_AX_CHANNEL;
    motor1.motor_magnitude_gpio_timer_ax_channel_module_function =
            MOTOR_1_MAGNITUDE_GPIO_TIMER_AX_CHANNEL_MODULE_FUNCTION;
    motor1.motor_direction_gpio_port = MOTOR_1_DIRECTION_GPIO_PORT;
    motor1.motor_direction_gpio_pin = MOTOR_1_DIRECTION_GPIO_PIN;
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

    // Physical Switches
    physical_switch_driver_t linkage_1_home_limit_switch_driver;
    linkage_1_home_limit_switch_driver.gpio_port =
            LIMIT_SWITCH_LINKAGE_1_HOME_GPIO_PORT;
    linkage_1_home_limit_switch_driver.gpio_pin =
            LIMIT_SWITCH_LINKAGE_1_HOME_GPIO_PIN;
    linkage_1_home_limit_switch_driver.switch_pressed_event = &limit_switch1_event;
    linkage_1_home_limit_switch_driver.switch_released_event = &limit_switch1_event;
    physical_switch_driver_open(&linkage_1_home_limit_switch_driver);

    physical_switch_driver_t pushbutton_s2_switch_driver;
    pushbutton_s2_switch_driver.gpio_port =
            PUSHBUTTON_S2_GPIO_PORT;
    pushbutton_s2_switch_driver.gpio_pin =
            PUSHBUTTON_S2_GPIO_PIN;
    pushbutton_s2_switch_driver.switch_pressed_event = &pushbutton_s2_event;
    pushbutton_s2_switch_driver.switch_released_event = SWITCH_EVENT_DO_NOTHING;
    physical_switch_driver_open(&pushbutton_s2_switch_driver);

    // See comment in declaration. Used in all examples.
    PMM_unlockLPM5();

    // Global enable interrupts
    __enable_interrupt();

    init_receiver();

    while (1) receiver_main();

}
