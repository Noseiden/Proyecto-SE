#pragma once //para evitar que un archivo header sea incluido varias veces
#include <stdbool.h>
// \ para continuar en la siguiente línea de código
// LÓGICA INVERSA por uso de BJTs:
#define MOTORS_DISABLE_ALL() do { \
    gpio_set_level(ENA_MOTOR_X, 0); \
    gpio_set_level(ENA_MOTOR_Y, 0); \
    gpio_set_level(ENA_MOTOR_Z, 0); \
} while(0) //Do-while(0) para agregar correctamente el ";" una vez en la etapa de preprocesado
//Esto podría haber sido una función, pero es más óptimo implementarlo en una macro o máscara que solo se expande antes de compilar
#define MOTORS_ENABLE_ALL() do { \
    gpio_set_level(ENA_MOTOR_X, 1); \
    gpio_set_level(ENA_MOTOR_Y, 1); \
    gpio_set_level(ENA_MOTOR_Z, 1); \
} while(0)
#define CW_DIR_X gpio_set_level(DIR_MOTOR_X, 1) 
#define CCW_DIR_X gpio_set_level(DIR_MOTOR_X, 0)
#define CW_DIR_Y gpio_set_level(DIR_MOTOR_Y, 1)
#define CCW_DIR_Y gpio_set_level(DIR_MOTOR_Y, 0)
#define CW_DIR_Z gpio_set_level(DIR_MOTOR_Z, 1)
#define CCW_DIR_Z gpio_set_level(DIR_MOTOR_Z, 0)

void motor_init(void);
void move_motor_x(float x_pos, bool dir);
void move_motor_y(float y_pos, bool dir);
void move_motor_z(float z_pos, bool dir);
bool motor_jog(bool init, int step, bool dir_x, bool dir_y, bool dir_z, bool x, bool y, bool z);
void stop_motors(void);

