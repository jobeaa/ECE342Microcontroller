#include <stdint.h>
#include <interpreter/commands.h>
#include <interpreter/lcd_msg.h>
#include <hardware_abstraction_layer/motor_controller.h>
#include <hardware_abstraction_layer/servo_controller.h>
#include <driverlib.h>
#include <stdbool.h>
#include <math.h>


float MULTIPLIER = 1.0;
bool ABSOLUTE = true;
bool TOOL_CHANGE = false;

const int LENGTH_1 = 6;
const int LENGTH_2 = 6;

int CURRENT_X = LENGTH_1 + LENGTH_2;
int CURRENT_Y = 0;

vec2 CURRENT_THETA = {
        0,
        0
};


// See documentation, calculates the Jacobian of the position function
// and calculates an incremental change in position that is required
void get_new_delta_theta(vec2 *theta_0, vec2 *delta_theta, vec2 *theta_ret) {
    vec2 point0, point1, theta_1;

    // Get the "next" value for theta
    theta_1.x = theta_0 -> x + delta_theta -> x;
    theta_1.y = theta_0 -> y + delta_theta -> y;

    // Translate the angle vector to a rectangular position vector
    get_point_from_theta(theta_0, &point0);
    get_point_from_theta(&theta_1, &point1);

    // Find the difference between the components of the "current" and "next" position vector
    point1.x -= point0.x;
    point1.y -= point0.y;

    // Multiply the Jacobian matrix by the difference in "current" and "next" posiition vector
    theta_ret -> x = (/*-1*LENGTH_1*sin(theta_0 -> x) +*/ LENGTH_2*(theta_0 -> x)) * point1.x;
    theta_ret -> y = (/*-1*LENGTH_1*sin(theta_0 -> y) +*/ LENGTH_2*(theta_0 -> y)) * point1.y;
}


// Convert theta vector to rectangular position vector
void get_point_from_theta(vec2 *theta, vec2 *ret_point) {
    ret_point -> x = (LENGTH_1 * (theta -> x)) + (LENGTH_2 * (theta -> y));
    ret_point -> y = (/*LENGTH_1 * sin(theta -> x)) + (LENGTH_2 * sin(theta -> y)*/ 0);
}


// Pythagorean theorem!
float distance (int x1, int x2, int y1, int y2) {
    int diffx = x2 - x1;
    int diffy = y2 - y1;
    return sqrt((diffx*diffx) + (diffy*diffy));
}


// Converts a command to the number of arguments that command takes
int cmd_to_nargs(uint8_t cmd) {
    switch (cmd) {
        case G00_NUM:
            return 2;
        case G01_NUM:
            return 2;
        case G20_NUM:
            return 0;
        case G21_NUM:
            return 0;
        case G90_NUM:
            return 0;
        case G91_NUM:
            return 0;
        case M02_NUM:
            return 0;
        case M06_NUM:
            return 0;
        case M72_NUM:
            return 0;
        default:
            return -1;
    }
}


void G00(uint8_t arg1, uint8_t arg2) {
    // Wait for the tool change to happen
    while (TOOL_CHANGE);

    // Raise the servo
    servo_controller_t servo_cmd = {};
    servo_controller_move_to(servo_cmd);

    int target_x, target_y, current_x, current_y;
    int dist;
    vec2 theta_0, theta_ret, delta_theta;

    // Determine the target position
    if (!ABSOLUTE) {
        target_x = CURRENT_X + arg1;
        target_y = CURRENT_Y + arg2;
    } else {
        target_x = arg1;
        target_y = arg2;
    }

    // Convert to metric if necessary
    target_x *= MULTIPLIER;
    target_y *= MULTIPLIER;
    current_x = CURRENT_X * MULTIPLIER;
    current_y = CURRENT_Y * MULTIPLIER;

    // Find the distance that must be traveled
    dist = distance(current_x, current_y, target_x, target_y);

    // Initial value for delta theta
    delta_theta.x = 1;
    delta_theta.y = 1;

    theta_0.x = CURRENT_THETA.x;
    theta_0.y = CURRENT_THETA.y;

    vec2 current_p;
    motor_controller_t contr1 = {};
    motor_controller_t contr2 = {};
    do {
        // Find the change in position we need
        get_new_delta_theta(&theta_0, &delta_theta, &theta_ret);
        CURRENT_THETA.x += theta_ret.x;
        CURRENT_THETA.y += theta_ret.y;

        // Move the motors so they reach their positions at the same time
        motor_controller_move_to(contr1, CURRENT_THETA.x, CURRENT_THETA.x / 0.1);
        motor_controller_move_to(contr2, CURRENT_THETA.y, CURRENT_THETA.y / 0.1);

        // Keep track of where we are
        get_point_from_theta(&CURRENT_THETA, &current_p);
        current_x = current_p.x;
        current_y = current_p.y;
    } while (dist < distance(current_x, current_y, target_x, target_y));


    // Lower the pen
    servo_controller_move_to(servo_cmd);

    // Keep track of where we are
    CURRENT_X = current_x;
    CURRENT_Y = current_y;
}


void G01(uint8_t arg1, uint8_t arg2) {
    // Wait for tool change
    while (TOOL_CHANGE);

    int target_x, target_y, current_x, current_y;
    int dist;
    vec2 theta_0, theta_ret, delta_theta;

    // Determine the target position
    if (!ABSOLUTE) {
        target_x = CURRENT_X + arg1;
        target_y = CURRENT_Y + arg2;
    } else {
        target_x = arg1;
        target_y = arg2;
    }

    // Convert to metric if necessary
    target_x *= MULTIPLIER;
    target_y *= MULTIPLIER;
    current_x = CURRENT_X * MULTIPLIER;
    current_y = CURRENT_Y * MULTIPLIER;

    // Find the distance that must be traveled
    dist = distance(current_x, current_y, target_x, target_y);

    // Initial value for delta theta
    delta_theta.x = 1;
    delta_theta.y = 1;

    theta_0.x = CURRENT_THETA.x;
    theta_0.y = CURRENT_THETA.y;

    vec2 current_p;
    motor_controller_t contr1 = {};
    motor_controller_t contr2 = {};
    do {
        // Find the change in position we need
        get_new_delta_theta(&theta_0, &delta_theta, &theta_ret);
        CURRENT_THETA.x += theta_ret.x;
        CURRENT_THETA.y += theta_ret.y;

        // Move the motors so they reach their positions at the same time
        motor_controller_move_to(contr1, CURRENT_THETA.x, CURRENT_THETA.x / 0.5);
        motor_controller_move_to(contr2, CURRENT_THETA.y, CURRENT_THETA.y / 0.5);

        // Keep track of where we are
        get_point_from_theta(&CURRENT_THETA, &current_p);
        current_x = current_p.x;
        current_y = current_p.y;
    } while (dist < distance(current_x, current_y, target_x, target_y));

    // Keep track of where we are
    CURRENT_X = current_x;
    CURRENT_Y = current_y;
}


void G20(void) {
    MULTIPLIER = 1.0;
}


void G21(void) {
    MULTIPLIER = 2.54;
}


void G90(void) {
    ABSOLUTE = true;
}


void G91(void) {
    ABSOLUTE = false;
}


void M02(void) {
    while (TOOL_CHANGE);
}


void M06(void) {
    write_msg("CHANGE", "TOOL  ");
    TOOL_CHANGE = true;
}


void M72(void) {
    while (TOOL_CHANGE);

    write_msg("AAAAAA", "AAAAAA");
}


// Interrupt vector, clears the tool change bool and the LCD
#pragma vector = PORT2_VECTOR
__interrupt void PORT2_ISR(void) {
    if (P2IV_P2IFG6) {
        write_msg("      ", "      ");
        TOOL_CHANGE = false;
        GPIO_clearInterrupt(GPIO_PORT_P2, GPIO_PIN6);
    }
}
