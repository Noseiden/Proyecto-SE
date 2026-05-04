#pragma once
#include <math.h>
#include <stdint.h>
#include <string.h> //para los caracteres
#include <stdio.h>
#include <stdarg.h> //Para el logging
#include <stdlib.h> //Para atoi() --> ASCII to integer
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/timer.h"
#include "esp_adc/adc_oneshot.h" //ADC
#include "driver/ledc.h" //PWM
#include "driver/uart.h" //Comunicación serial UART

#include "pin_map.h"