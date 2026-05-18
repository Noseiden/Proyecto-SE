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
        .channel = LEDC_CHANNEL_1, 
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = PUL_MOTOR_Y,
        .duty = 0, //Duty inicial
        .hpoint = 0, //Punto de inicio del pulso: 0 (normal)
    };
    ledc_channel_config(&pwm_PUL_y);

    ledc_channel_config_t pwm_PUL_z = { //Configuración PUL Z
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_2, 
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = PUL_MOTOR_Z,
        .duty = 0, //Duty inicial
        .hpoint = 0, //Punto de inicio del pulso: 0 (normal)
    };
    ledc_channel_config(&pwm_PUL_z);

    MOTORS_DISABLE_ALL();
    CCW_DIR_X;
    CCW_DIR_Y;
    CW_DIR_Z;

}

/*void move_motor_x(float x_pos, bool dir){
    //dir = true --> CCW ; dir = false --> CW
    MOTORS_ENABLE_ALL();

}*/

bool motor_jog(bool init, int step, bool dir_x, bool dir_y, bool dir_z, bool x, bool y, bool z){
    static TickType_t jog_start_tick = 0;
    static TickType_t jog_duration_ticks = 0;
    // Pasos/mm = 3200micropasos/vuelta / 5mm/vuelta = 640 pasos/mm
    // Frecuencia = 600mm/min * 640pasos/mm / 60 = 6.4kHz
    if(init){
        ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, 6400);
        float jog_duration_ms = (60.0 * step * 1000.0) / 600; //600 mm/min de velocidad
        jog_duration_ticks = pdMS_TO_TICKS(jog_duration_ms);
        jog_start_tick = xTaskGetTickCount(); //Registrar tick actual
        if(dir_x){
            CCW_DIR_X;
        } else {
            CW_DIR_X;
        }
        if(dir_y){
            CCW_DIR_Y;
        } else {
            CW_DIR_Y;
        }
        if(dir_z){
            CCW_DIR_Z;
        } else {
        CW_DIR_Z;
    }
        if(x){
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 2048); //Al 50% DutyCyle
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        }
        if(y){
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 2048); //Al 50% DutyCyle
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
        }
        if(z){
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 2048); //Al 50% DutyCyle
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
        }
        return false;
    }
    if((xTaskGetTickCount() - jog_start_tick) >= jog_duration_ticks){ //Cuando termine de avanzar, detiene el motor
        if(x && !init){
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            x = false;
        }
        if(y && !init){
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
            y = false;
        }
        if(z && !init){
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 0);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
            z = false;
        }
        return true; //Ya se detuvo
    }   
    return false; //No ha terminado de avanzar
}

void stop_motors(void){
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
}