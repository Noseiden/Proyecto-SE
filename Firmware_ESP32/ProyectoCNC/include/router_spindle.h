#pragma once //para evitar que un archivo header sea incluido varias veces
#define SPINDLE_OFF gpio_set_level(SPINDLE_ONOFF, 0)
#define SPINDLE_ON gpio_set_level(SPINDLE_ONOFF, 1)
spindle_init(void);

