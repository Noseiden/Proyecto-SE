#pragma once //para evitar que un archivo header sea incluido varias veces

#define UART_PORT UART_NUM_0 //Comunicación UART por USB ESP32 con computador

#define LIMIT_SWITCH_X0 (GPIO_NUM_32)
#define LIMIT_SWITCH_X1 (GPIO_NUM_33)
#define LIMIT_SWITCH_Y0 (GPIO_NUM_27)
#define LIMIT_SWITCH_Y1 (GPIO_NUM_14)
#define LIMIT_SWITCH_Z0 (GPIO_NUM_12)
#define LIMIT_SWITCH_Z1 (GPIO_NUM_13)

#define SPINDLE_ONOFF (GPIO_NUM_23)
#define RTC_SCL (GPIO_NUM_22)
#define RTC_SDA (GPIO_NUM_21)
#define DIR_MOTOR_Z (GPIO_NUM_19)
#define PUL_MOTOR_Z (GPIO_NUM_18)
#define ENA_MOTOR_Z (GPIO_NUM_5)
#define ENA_MOTOR_Y (GPIO_NUM_17)
#define DIR_MOTOR_Y (GPIO_NUM_16)
#define PUL_MOTOR_Y (GPIO_NUM_4)
#define ENA_MOTOR_X (GPIO_NUM_0)
#define DIR_MOTOR_X (GPIO_NUM_2)
#define PUL_MOTOR_X (GPIO_NUM_15)