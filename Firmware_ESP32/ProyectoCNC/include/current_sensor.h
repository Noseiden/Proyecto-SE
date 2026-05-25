#pragma once //para evitar que un archivo header sea incluido varias veces
#define SAMPLE_PERIOD_US 1000 // 1000us o 1ms de periodo de muestreo del ADC
#include <stdbool.h>
#include "esp_adc/adc_oneshot.h"
extern adc_oneshot_unit_handle_t adc1_handle;
typedef struct {
    float s_I; // corriente spindle
    float x_I;  
    float y_I;  
    float z_I; //corriente motor Z
} consumo_cnc_t;
void I_sensor_init(void);
float calc_I(int raw);
//bool read_I_sensor(consumo_cnc_t *data);