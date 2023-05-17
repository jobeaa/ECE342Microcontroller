/*
 *  Pin bindings for the board.
 *
 *  Created on: May 8, 2023
 *      Author: silas
 */

#ifndef BOARD_H_
#define BOARD_H_

#define ONBOARD_LED1_GPIO_PORT GPIO_PORT_P1
#define ONBOARD_LED1_GPIO_PIN GPIO_PIN0
#define ONBOARD_LED2_GPIO_PORT GPIO_PORT_P4
#define ONBOARD_LED2_GPIO_PIN GPIO_PIN0

#define MOTOR_1_DIRECTION_PIN -1
#define MOTOR_1_MAGNITUDE_PIN -1
#define MOTOR_2_DIRECTION_PIN -1
#define MOTOR_2_MAGNITUDE_PIN -1

#define ENCODER_1_SIGNAL_A_GPIO_PORT GPIO_PORT_P1
#define ENCODER_1_SIGNAL_A_GPIO_PIN GPIO_PIN4
#define ENCODER_1_SIGNAL_B_GPIO_PORT GPIO_PORT_P1
#define ENCODER_1_SIGNAL_B_GPIO_PIN GPIO_PIN5
#define ENCODER_2_SIGNAL_A_GPIO_PORT -1
#define ENCODER_2_SIGNAL_A_GPIO_PIN -1
#define ENCODER_2_SIGNAL_B_GPIO_PORT -1
#define ENCODER_2_SIGNAL_B_GPIO_PIN -1


#define WRITING_UTENSIL_SERVO_SIGNAL_PIN -1

#define LIMIT_SWITCH_LINKAGE_1_HOME_PIN -1
#define LIMIT_SWITCH_LINKAGE_1_MAX_PIN -1
#define LIMIT_SWITCH_LINKAGE_2_HOME_PIN -1
#define LIMIT_SWITCH_LINKAGE_2_MAX_PIN -1
#define LIMIT_SWITCH_WRITING_UTENSIL_HOME_PIN -1

#endif /* BOARD_H_ */
