#pragma once //para evitar que un archivo header sea incluido varias veces
#include <stdbool.h>
// \ para continuar en la siguiente línea de código
// LÓGICA INVERSA por uso de BJTs:
#define MOTOR_X_DISABLED gpio_set_level (ENA_MOTOR_X, 1) 
#define MOTOR_Y_DISABLED gpio_set_level(ENA_MOTOR_Y, 1)
#define MOTOR_Z_DISABLED gpio_set_level(ENA_MOTOR_Z, 1)
#define MOTOR_X_ENABLED gpio_set_level (ENA_MOTOR_X, 0) 
#define MOTOR_Y_ENABLED gpio_set_level(ENA_MOTOR_Y, 0)
#define MOTOR_Z_ENABLED gpio_set_level(ENA_MOTOR_Z, 0)

#define CW_DIR_X gpio_set_level(DIR_MOTOR_X, 1) 
#define CCW_DIR_X gpio_set_level(DIR_MOTOR_X, 0)
#define CW_DIR_Y gpio_set_level(DIR_MOTOR_Y, 1)
#define CCW_DIR_Y gpio_set_level(DIR_MOTOR_Y, 0)
#define CW_DIR_Z gpio_set_level(DIR_MOTOR_Z, 1)
#define CCW_DIR_Z gpio_set_level(DIR_MOTOR_Z, 0)

#define SWITCH_X0_OFF (gpio_get_level(LIMIT_SWITCH_X0) == 1)
#define SWITCH_X0_ON (gpio_get_level(LIMIT_SWITCH_X0) == 0)
#define SWITCH_X1_OFF (gpio_get_level(LIMIT_SWITCH_X1) == 1)
#define SWITCH_X1_ON (gpio_get_level(LIMIT_SWITCH_X1) == 0)
#define SWITCH_Y0_OFF (gpio_get_level(LIMIT_SWITCH_Y0) == 1)
#define SWITCH_Y0_ON (gpio_get_level(LIMIT_SWITCH_Y0) == 0)
#define SWITCH_Y1_OFF (gpio_get_level(LIMIT_SWITCH_Y1) == 1)
#define SWITCH_Y1_ON (gpio_get_level(LIMIT_SWITCH_Y1) == 0)
#define SWITCH_Z0_OFF (gpio_get_level(LIMIT_SWITCH_Z0) == 1)
#define SWITCH_Z0_ON (gpio_get_level(LIMIT_SWITCH_Z0) == 0)
#define SWITCH_Z1_OFF (gpio_get_level(LIMIT_SWITCH_Z1) == 1)
#define SWITCH_Z1_ON (gpio_get_level(LIMIT_SWITCH_Z1) == 0)

extern int segment; //extern para indicar al compilador que espere a encontrar esta declaración de variable en este archivo
void motor_init(void);
bool make_a_circle(bool init, bool pause, int last_segm);
void motor_jog(int step, bool dir_x, bool dir_y, bool dir_z, bool x, bool y, bool z);
bool home(bool init);
void stop_motors(void);