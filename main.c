#include "driverlib.h"

#include "board.h"
#include "encoder_driver.h"
#include "clock_driver.h"
#include "motor_direct_driver.h"
#include "servo_driver.h"

encoder_driver_t encoder1;
motor_direct_driver_t motor1;

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
    writing_utensil_servo.motor_controller.motor_magnitude_gpio_port =
            WRITING_UTENSIL_SERVO_SIGNAL_GPIO_PORT;
    writing_utensil_servo.motor_controller.motor_magnitude_gpio_pin =
            WRITING_UTENSIL_SERVO_SIGNAL_GPIO_PIN;
    writing_utensil_servo.motor_controller.timer_aX_base_address =
            WRITING_UTENSIL_SERVO_SIGNAL_TIMER_AX_BASE_ADDRESS;
    writing_utensil_servo.motor_controller.timer_aX_channel_compare_register =
            WRITING_UTENSIL_SERVO_SIGNAL_TIMER_AX_CAPCOMP_REG;
    writing_utensil_servo.motor_controller.motor_magnitude_gpio_timer_ax_channel_module_function =
            WRITING_UTENSIL_SERVO_SIGNAL_GPIO_TIMER_AX_CHANNEL_MODULE_FUNCTION;
    // GROSS: use LCD-tied pin which won't be used.
    writing_utensil_servo.motor_controller.motor_direction_gpio_port =
            GPIO_PORT_P7;
    writing_utensil_servo.motor_controller.motor_direction_gpio_pin =
            GPIO_PORT_P1;
    servo_driver_open(&writing_utensil_servo);


    servo_driver_move_to(&writing_utensil_servo, 150);




    // Limit Switch Controllers

    // See comment in declaration. Used in all examples.
    PMM_unlockLPM5();

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

    encoder_driver_signal_a_rising_edge_event(&encoder1);
}
