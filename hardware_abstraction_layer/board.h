/*
 *  Pin bindings for the board.
 *
 *  Created on: May 8, 2023
 *      Author: silas
 */

#include "static_assert.h"

#ifndef BOARD_H_
#define BOARD_H_

#define ONBOARD_LED1_GPIO_PORT  GPIO_PORT_P1
#define ONBOARD_LED1_GPIO_PIN   GPIO_PIN0
#define ONBOARD_LED2_GPIO_PORT  GPIO_PORT_P4
#define ONBOARD_LED2_GPIO_PIN   GPIO_PIN0

#define MOTOR_1_MAGNITUDE_GPIO_PORT                             GPIO_PORT_P1
#define MOTOR_1_MAGNITUDE_GPIO_PIN                              GPIO_PIN7
#define MOTOR_1_MAGNITUDE_TIMER_AX_BASE_ADDRESS                 TIMER_A0_BASE
#define MOTOR_1_MAGNITUDE_TIMER_AX_CAPCOMP_REG                  TIMER_A_CAPTURECOMPARE_REGISTER_1
#define MOTOR_1_MAGNITUDE_GPIO_TIMER_AX_CHANNEL_MODULE_FUNCTION GPIO_PRIMARY_MODULE_FUNCTION
#define MOTOR_1_DIRECTION_ENABLE_1_GPIO_PORT                             GPIO_PORT_P5
#define MOTOR_1_DIRECTION_ENABLE_1_GPIO_PIN                              GPIO_PIN0
#define MOTOR_1_DIRECTION_ENABLE_2_GPIO_PORT                             GPIO_PORT_P5
#define MOTOR_1_DIRECTION_ENABLE_2_GPIO_PIN                              GPIO_PIN2

#define MOTOR_2_DIRECTION_PIN -1
#define MOTOR_2_MAGNITUDE_TIMER_AX_BASE_ADDRESS                 0
#define MOTOR_2_MAGNITUDE_PIN -1

#define ENCODER_1_SIGNAL_A_GPIO_PORT        GPIO_PORT_P1
#define ENCODER_1_SIGNAL_A_GPIO_PIN         GPIO_PIN4
#define ENCODER_1_SIGNAL_B_GPIO_PORT        GPIO_PORT_P1
#define ENCODER_1_SIGNAL_B_GPIO_PIN         GPIO_PIN5
#define ENCODER_2_SIGNAL_A_GPIO_PORT -1
#define ENCODER_2_SIGNAL_A_GPIO_PIN -1
#define ENCODER_2_SIGNAL_B_GPIO_PORT -1
#define ENCODER_2_SIGNAL_B_GPIO_PIN -1

#define WRITING_UTENSIL_SERVO_SIGNAL_GPIO_PORT                             GPIO_PORT_P8
#define WRITING_UTENSIL_SERVO_SIGNAL_GPIO_PIN                              GPIO_PIN3
#define WRITING_UTENSIL_SERVO_SIGNAL_TIMER_AX_BASE_ADDRESS                 TIMER_A1_BASE
#define WRITING_UTENSIL_SERVO_SIGNAL_TIMER_AX_CAPCOMP_REG                  TIMER_A_CAPTURECOMPARE_REGISTER_2
#define WRITING_UTENSIL_SERVO_SIGNAL_GPIO_TIMER_AX_CHANNEL_MODULE_FUNCTION GPIO_PRIMARY_MODULE_FUNCTION

#define LIMIT_SWITCH_LINKAGE_1_HOME_PIN -1
#define LIMIT_SWITCH_LINKAGE_1_MAX_PIN -1
#define LIMIT_SWITCH_LINKAGE_2_HOME_PIN -1
#define LIMIT_SWITCH_LINKAGE_2_MAX_PIN -1
#define LIMIT_SWITCH_WRITING_UTENSIL_HOME_PIN -1

CASSERT(WRITING_UTENSIL_SERVO_SIGNAL_TIMER_AX_BASE_ADDRESS != MOTOR_1_MAGNITUDE_TIMER_AX_BASE_ADDRESS, board_h);
CASSERT(WRITING_UTENSIL_SERVO_SIGNAL_TIMER_AX_BASE_ADDRESS != MOTOR_2_MAGNITUDE_TIMER_AX_BASE_ADDRESS, board_h);

#endif /* BOARD_H_ */
