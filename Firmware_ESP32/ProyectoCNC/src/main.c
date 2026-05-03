#include "current_sensor.h"
#include "logging.h"
#include "motors.h"
#include "router_spindle.h"
#include "rtc_ds1307.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

typedef enum { // Definición de estados
    STATE_INIT,
    STATE_IDLE,
    STATE_JOG,
    STATE_RUNNING,
    STATE_FEED_HOLD,
    STATE_ALARM
} cnc_state_t;

// Variable global de estado (volátil porque puede cambiar en interrupciones)
volatile cnc_state_t current_state = STATE_INIT;

float pos_x = 0, pos_y = 0, pos_z = 0;
float step = 1.0;

void procesar_comando_gui(char* comando) {  //Recepción de los botones de la interfaz (GUI) 
    // Ejemplo de lógica para los botones:
    if (strcmp(comando, "START") == 0 && (current_state == STATE_IDLE || current_state == STATE_FEED_HOLD)) {
        current_state = STATE_RUNNING;
    } 
    
    else if (strcmp(comando, "FEED_HOLD") == 0 && current_state == STATE_RUNNING) {
        current_state = STATE_FEED_HOLD;
    }

    else if (strcmp(comando, "STOP") == 0) {
        current_state = STATE_ALARM;
    }

    else if (strcmp(comando, "RESET") == 0) {
        current_state = STATE_IDLE;
        ESP_LOGI("CNC", "Alarma liberada.");
    }

    else if (strcmp(comando, "SET_ORIGIN") == 0 && current_state == STATE_IDLE) {
        pos_x = 0; pos_y = 0; pos_z = 0;
        ESP_LOGI("CNC", "Origen establecido en (0,0,0)");
        // Aquí llamarías a tu función de logging del DS1307
    }

    else if (strstr(comando, "JOG") != NULL && current_state == STATE_IDLE) {
        current_state = STATE_JOG;
    }
}

void app_main(void) {
    while (1) {
        switch (current_state) {
            case STATE_INIT:
                ESP_LOGI("CNC", "Iniciando sistema...");
                // Aquí llamarías a motores_init(), rtc_init(), etc.
                current_state = STATE_IDLE;
                break;

            case STATE_IDLE:
                // La máquina no hace nada, espera comandos de la GUI
                // El spindle está apagado
                break;

            case STATE_JOG:
                ESP_LOGI("CNC", "Moviendo eje a paso: %.2f", step);
                // motores_mover(eje, jog_step); 
                current_state = STATE_IDLE; 
                break;

            case STATE_RUNNING:
                // Si hay un error de sensor o botón Stop -> current_state = STATE_ALARM;
                break;

            case STATE_FEED_HOLD:
                // Detener motores pero mantener ruteadora encendida
                // motores_stop_gradual();
                break;

            case STATE_ALARM:
                // BLOQUEO TOTAL: Apagar ruteadora y motores
                // ruteadora_off();
                // motores_disable();
                ESP_LOGE("CNC", "SISTEMA BLOQUEADO - REQUIERE RESET");
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Pequeña espera para no saturar CPU
    }
}