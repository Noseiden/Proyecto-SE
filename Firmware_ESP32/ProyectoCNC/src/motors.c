#include "motors.h"
#include "system_lib.h"

void motor_init(void){
    gpio_config_t limit_S = { //Sensores inicio y final de carrera en cada eje
        .pin_bit_mask = (1ULL << LIMIT_SWITCH_X0) | (1ULL << LIMIT_SWITCH_X1) | 
                        (1ULL << LIMIT_SWITCH_Y0) | (1ULL << LIMIT_SWITCH_Y1) | 
                        (1ULL << LIMIT_SWITCH_Z0) | (1ULL << LIMIT_SWITCH_Z1) ,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&limit_S);

    gpio_config_t out_cfg = { // Para las borneras DIR y ENA del DM556 
        .pin_bit_mask = (1ULL << DIR_MOTOR_X | 1ULL << DIR_MOTOR_Y |
                         1ULL << DIR_MOTOR_Z | 1ULL << ENA_MOTOR_X | 
                         1ULL << ENA_MOTOR_Y | 1ULL << ENA_MOTOR_Z),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&out_cfg);

    ledc_timer_config_t ledc_timer = { //por acá está la frecuencia del PWM
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_12_BIT, //Resolución --> 0 a 4095, pues es 12 bits
        .freq_hz = 2000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&ledc_timer);
    
    ledc_channel_config_t pwm_PUL_x = { //Configuración PUL X
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0, 
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = PUL_MOTOR_X,
        .duty = 0, //Duty inicial
        .hpoint = 0, //Punto de inicio del pulso: 0 (normal)
    };
    ledc_channel_config(&pwm_PUL_x);

    ledc_channel_config_t pwm_PUL_y = { //Configuración PUL Y
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0, 
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = PUL_MOTOR_Y,
        .duty = 0, //Duty inicial
        .hpoint = 0, //Punto de inicio del pulso: 0 (normal)
    };
    ledc_channel_config(&pwm_PUL_y);

    ledc_channel_config_t pwm_PUL_z = { //Configuración PUL Z
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0, 
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = PUL_MOTOR_Z,
        .duty = 0, //Duty inicial
        .hpoint = 0, //Punto de inicio del pulso: 0 (normal)
    };
    ledc_channel_config(&pwm_PUL_z);

    MOTORS_DISABLE_ALL();
    CCW_DIR_X;
    CW_DIR_Y;
    CCW_DIR_Z;

}
void motor_jog(void){
    MOTORS_DISABLE_ALL();
    
}