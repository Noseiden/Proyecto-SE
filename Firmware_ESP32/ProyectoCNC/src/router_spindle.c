#include "router_spindle.h"
#include "system_lib.h"
void spindle_init(void){
    gpio_config_t out_spindle = {
        .pin_bit_mask = (1ULL << SPINDLE_ONOFF),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&out_spindle);
    SPINDLE_OFF;
}
