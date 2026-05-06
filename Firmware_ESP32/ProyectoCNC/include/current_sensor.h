#pragma once //para evitar que un archivo header sea incluido varias veces
#define SAMPLE_PERIOD_US 1000 // 1000us o 1ms de periodo de muestreo del ADC, para la LDR y LM35
#include <stdbool.h>
typedef struct {
    float s_I; // corriente spindle
    float x_I;  
    float y_I;  
    float z_I; //corriente motor Z
} consumo_cnc_t;
void I_sensor_init(void);
bool read_I_sensor(consumo_cnc_t *data);