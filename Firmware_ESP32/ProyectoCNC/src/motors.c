#include "motors.h"
#include "system_lib.h"
#include "logging.h"
#include "rtc_ds1307.h"

int segment = -1; //variable global externa declarada en el header motors.h

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
        .freq_hz = 1000,
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

    MOTOR_X_DISABLED;
    MOTOR_Y_DISABLED;
    MOTOR_Z_DISABLED;
    CCW_DIR_X;
    CCW_DIR_Y;
    CW_DIR_Z;

}

bool make_a_circle(bool init, bool pause, int last_segm){
    //dir = true --> CCW ; dir = false --> CW
    MOTOR_X_ENABLED;
    MOTOR_Y_ENABLED;
    MOTOR_Z_ENABLED;
    static int current_segment = 0;
    // 360° / 36 = 10°/segmento
    static const int total_segments = 36; //Total de segmentos en el que se divide el círculo
    static const uint32_t T_block_ms = 500;   // Duración de cada segmento en ms
    static TickType_t segment_start_time = 0;
    static uint32_t x_run_time_ms = 0;
    static uint32_t y_run_time_ms = 0;
    static bool x_running = false;
    static bool y_running = false;
    static bool circle_step_init = true;
    if(init){ //Inicializar variables una vez
        current_segment = 0;
        circle_step_init = true;
        x_running = false;
        y_running = false;
        ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, 2000);
        if(pause && last_segm != -1){
            current_segment = last_segm; //Para después de darle al botón pausa y guardar la posición actual
        }
        return false;
    }
    if(current_segment < total_segments){
        if (circle_step_init) {
            float angle;
            if(pause == false){
                angle = (2.0 * M_PI / total_segments) * current_segment; //ángulo en radianes
            } else {
                angle = (2.0 * M_PI / total_segments) * last_segm; 
            }
            // Calcular el tiempo en ms para mover cada eje
            x_run_time_ms = (uint32_t)(T_block_ms * fabs(sin(angle)));
            y_run_time_ms = (uint32_t)(T_block_ms * fabs(cos(angle)));

            if (sin(angle) >= 0) { 
                CCW_DIR_X;
            } else { 
                CW_DIR_X;
            }
            if (cos(angle) >= 0) { 
                CCW_DIR_Y;
            } else { 
                CW_DIR_Y;
            }

            if (x_run_time_ms > 0) {
                ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 2048); // Al 50% DutyCycle
                ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
                x_running = true;
            }
            if (y_run_time_ms > 0) {
                ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 2048); // Al 50% DutyCycle
                ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
                y_running = true;
            }

            segment_start_time = xTaskGetTickCount(); // Guardamos el tiempo de inicio en marca de tiempo
            circle_step_init = false;               // Aún no se inicia un nuevo segmento
        }

        uint32_t elapsed_ms = pdTICKS_TO_MS(xTaskGetTickCount() - segment_start_time);

        // Apagar motor X si ya cumplió su tiempo en este segmento:
        if (x_running && (elapsed_ms >= x_run_time_ms)) {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            x_running = false;
        }

        // Apagar motor Y si ya cumplió su tiempo en este segmento:
        if (y_running && (elapsed_ms >= y_run_time_ms)) {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
            y_running = false;
        }

        if (elapsed_ms >= T_block_ms) { //Al terminar el tiempo en este segmento
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
            
            // Actualizar para seguir en un nuevo segmento:
            current_segment++; 
            segment = current_segment; 
            circle_step_init = true;

        }

        return false; // El círculo todavía está en proceso
    }

    return true;
}

void motor_jog(int step, bool dir_x, bool dir_y, bool dir_z, bool x, bool y, bool z){
    // Pasos/mm = 3200micropasos/vuelta / 5mm/vuelta = 640 pasos/mm
    // Frecuencia = velocidad * 640pasos/mm / 60segundos --> velocidad en mm/min
    int pwm_freq = (int)((step * 640.0) / 60.0);
    ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, pwm_freq);

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
        MOTOR_X_ENABLED;
        MOTOR_Y_DISABLED;
        if((SWITCH_X1_ON && dir_x) || (SWITCH_X0_ON && dir_x == false)){ //Prevención para no superar límites de máquina
            ds1307_read_time(2);
            GUI_ERROR("Pasó el límite del área de trabajo");
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            MOTOR_X_DISABLED;
        } else {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 2048); //Al 50% DutyCyle
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            ds1307_read_time(0);
            GUI_INFO("Movimiento manual del eje X");
        }
    }
    if(y){
        MOTOR_X_DISABLED;
        MOTOR_Y_ENABLED;
        if((SWITCH_Y1_ON && dir_y) || (SWITCH_Y0_ON && dir_y == false)){ //Prevención para no superar límites de máquina
            ds1307_read_time(2);
            GUI_ERROR("Pasó el límite del área de trabajo");
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
            MOTOR_Y_DISABLED;
        } else {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 2048); //Al 50% DutyCyle
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
            ds1307_read_time(0);
            GUI_INFO("Movimiento manual del eje Y");
        }
    }
    if(z){
        MOTOR_X_DISABLED;
        MOTOR_Y_DISABLED;
        MOTOR_Z_ENABLED;
        if((SWITCH_Z0_ON && dir_z) || (SWITCH_Z1_ON && dir_z == false)){ //Prevención para no superar límites de máquina
            ds1307_read_time(2);
            GUI_ERROR("Pasó el límite del área de trabajo");
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 0);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
            MOTOR_Z_DISABLED;
        } else {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 2048); //Al 50% DutyCyle
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
            ds1307_read_time(0);
            GUI_INFO("Movimiento manual del eje Z");
        }
    }
}

bool home(bool init){
    static bool z_homed = false; //static para recordar su valor local en cada ejecución del main
    static bool x_homed = false;
    static bool y_homed = false;
    static bool x_running = false;
    static bool y_running = false;
    if (init){
        CW_DIR_X;
        CW_DIR_Y;
        CCW_DIR_Z;
        z_homed = false; 
        x_homed = false;
        y_homed = false;
        x_running = false;
        y_running = false;
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 2048); //Al 50% DutyCyle
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
        return false;
    }
    if(SWITCH_Z0_ON && !z_homed){ //Hasta que llegue a su posición inicial
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 0);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
        z_homed = true; 
    }
    if(SWITCH_Z0_ON && !init && SWITCH_X0_OFF && !x_running){
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 2048); //Al 50% DutyCyle
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        x_running = true;
    } else if (SWITCH_Z0_ON && !init && SWITCH_X0_ON && !x_homed) {
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0); 
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        x_homed = true;
    }
    if(SWITCH_Z0_ON && !init && SWITCH_Y0_OFF && !y_running){
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 2048); //Al 50% DutyCyle
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
        y_running = true;
    } else if (SWITCH_Z0_ON && !init && SWITCH_Y0_ON && !y_homed) {
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0); 
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
        y_homed = true;
    }
    if (z_homed && x_homed && y_homed) {
        return true; // Homing completado
    }
    return false;
}

void stop_motors(void){
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
}