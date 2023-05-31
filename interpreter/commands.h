#include <stdint.h>

#ifndef GCODE_COMMANDS_H_
#define GCODE_COMMANDS_H_


typedef struct vec2 {
    int x;
    int y;
} vec2;



// Converts a command to the number of arguments that command takes
int cmd_to_nargs(uint8_t cmd);

void get_new_delta_theta(vec2 *theta_0, vec2 *delta_theta, vec2 *theta_ret);
void get_point_from_theta(vec2 *theta, vec2 *ret_point);

#define G00_NUM (1 << 7)
#define G01_NUM (1 << 7) + 1
#define G20_NUM (1 << 7) + 20
#define G21_NUM (1 << 7) + 21
#define G90_NUM (1 << 7) + 90
#define G91_NUM (1 << 7) + 91
#define M02_NUM 2
#define M06_NUM 6
#define M72_NUM 72

void G00(uint8_t arg1, uint8_t arg2);
void G01(uint8_t arg1, uint8_t arg2);
void G20(void);
void G21(void);
void G90(void);
void G91(void);
void M02(void);
void M06(void);
void M72(void);

#endif
