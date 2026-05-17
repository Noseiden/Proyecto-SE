#include "system_lib.h" //Tiene librerías necesarias de C y PlatformIO, así como también los pines GPIO
#include "current_sensor.h"
#include "logging.h"
#include "motors.h"
#include "router_spindle.h"
#include "rtc_ds1307.h"

int step_mm = 0;
int step_received = 0;

typedef enum { //botones de GUI
    CMD_NONE, 
    CMD_START, 
    CMD_PAUSE, 
    CMD_STOP, 
    CMD_RESET, 
    CMD_ORIGIN, 
    CMD_JOG_XP,
    CMD_JOG_XM,
    CMD_JOG_YP,
    CMD_JOG_YM,
    CMD_JOG_ZP,
    CMD_JOG_ZM,
} gui_command_t;
volatile gui_command_t last_command = CMD_NONE; //volátil porque puede cambiar en interrupciones

typedef enum { //Estados
    STATE_INIT,
    STATE_IDLE,
    STATE_JOG,
    STATE_RUNNING,
    STATE_PAUSE,
    STATE_ALARM
} cnc_state_t;
volatile cnc_state_t current_state = STATE_INIT;

float pos_x = 0, pos_y = 0, pos_z = 0;

void task_receive_gui(void *pvParameters){ //Recepción de botones de GUI para cambio de estado
    uint8_t data[128];
    while(1){
        int len = uart_read_bytes(UART_PORT, data, sizeof(data) - 1, 20 / portTICK_PERIOD_MS);
        if (len > 0) {
            data[len] = '\0'; // Convertimos a string de C
            char *p_step = strstr((char*)data, "STEP:");
            // Comparamos lo recibido con las palabras clave de botones
            if (p_step != NULL){
                if(sscanf(p_step, "STEP:%d", &step_mm) == 1){  // Con sscanf, busca en STEP: un número entero(%d) para llevarlo a step_mm
                    GUI_INFO("Paso de JOG actualizado a: %d mm/min", step_mm);
                }
            } else if (strstr((char*)data, "START"))  last_command = CMD_START;
            else if (strstr((char*)data, "PAUSE"))  last_command = CMD_PAUSE;
            else if (strstr((char*)data, "STOP"))   last_command = CMD_STOP;
            else if (strstr((char*)data, "RESET"))  last_command = CMD_RESET;
            else if (strstr((char*)data, "Zp"))     last_command = CMD_JOG_ZP;
            else if (strstr((char*)data, "Zm"))     last_command = CMD_JOG_ZM;
            else if (strstr((char*)data, "Xp"))     last_command = CMD_JOG_XP;
            else if (strstr((char*)data, "Xm"))     last_command = CMD_JOG_XM;
            else if (strstr((char*)data, "Yp"))     last_command = CMD_JOG_YP;
            else if (strstr((char*)data, "Ym"))     last_command = CMD_JOG_YM;
            else if (strstr((char*)data, "ORIGIN")) last_command = CMD_ORIGIN;
        }
        vTaskDelay(pdMS_TO_TICKS(10)); //Para evitar WatchDog
    }
}

void app_main(void) {
    uart_config_t uart_config = {
        .baud_rate = 115200,  
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT
    };
    uart_param_config(UART_PORT, &uart_config); // & para mandar de una vez como pointer
    uart_driver_install(UART_PORT, 1024, 1024, 0, NULL, 0);
    
    I_sensor_init(); //Configuración ADC de los sensores de corriente
    consumo_cnc_t corrientes_actuales;
    cnc_state_t last_reported_state = -1;
    xTaskCreate(task_receive_gui, "GUI_Task", 4096, NULL, 5, NULL); //para que FreeRTOS identifique la tarea 
    i2c_init();
    ds1307_write_hours(0, 0, 12, 5, 22, 5, 26); // 12:00:00, día 5 de la semana, 22/05/26 
    TickType_t ultima_lectura_rtc = xTaskGetTickCount(); //control del tiempo RTC
    while (1) {
        if (last_command != CMD_NONE) { //Las transiciones a cada estado
            switch(last_command) {
                case CMD_START: current_state = STATE_RUNNING; break;
                case CMD_PAUSE: current_state = STATE_PAUSE; break;
                case CMD_RESET: current_state = STATE_IDLE; break;
                case CMD_STOP:  current_state = STATE_ALARM; break;
                case CMD_ORIGIN: 
                    if(current_state == STATE_IDLE) {
                        pos_x = 0; pos_y = 0; pos_z = 0; 
                    }
                    break;
                case CMD_JOG_XP: 
                    if(current_state == STATE_IDLE) {
                        current_state = STATE_JOG; 
                        pos_x += step_mm; 
                    }
                    break;
                case CMD_JOG_XM: 
                    if(current_state == STATE_IDLE) {
                        current_state = STATE_JOG; 
                        pos_x -= step_mm; 
                    }
                    break;
                case CMD_JOG_YP: 
                    if(current_state == STATE_IDLE) {
                        current_state = STATE_JOG; 
                        pos_y += step_mm; 
                    }
                    break;
                case CMD_JOG_YM: 
                    if(current_state == STATE_IDLE) {
                        current_state = STATE_JOG; 
                        pos_y -= step_mm; 
                    }
                    break;
                case CMD_JOG_ZP: 
                    if(current_state == STATE_IDLE) {
                        current_state = STATE_JOG; 
                        pos_z += step_mm; 
                    }
                    break;
                case CMD_JOG_ZM: 
                    if(current_state == STATE_IDLE) {
                        current_state = STATE_JOG; 
                        pos_z -= step_mm; 
                    }
                    break;
                default: // Por seguridad, siempre es bueno tener un default
                    break;
            }
            last_command = CMD_NONE; //Reset del comando después de procesarlo
        }
        if (current_state != last_reported_state){ //Para enviar los mensajes de logging una vez se ha cambiado de estado
            switch (current_state) { //Lo que hace cada estado
                case STATE_IDLE:
                    GUI_INFO("Esperando orden de maquinado o Jog manual");
                    // La máquina no hace nada, espera comandos de la GUI
                    // El spindle está apagado
                    break;

                case STATE_JOG:
                    GUI_INFO("Movimiento manual de los ejes");
                    // motores_mover(eje, jog_step); 
                    current_state = STATE_IDLE; 
                    break;

                case STATE_RUNNING:
                    GUI_INFO("En proceso de maquinado...");
                    // Lógica de seguridad: Si la fresa se atasca en el material, la corriente subirá mucho.
                    // En los cuatro motores, jalan hasta 4A 
                    if (read_I_sensor(&corrientes_actuales)) {
                        if (corrientes_actuales.s_I > 3) {
                            GUI_ERROR("Sobrecarga en la Ruteadora: %.2f A", corrientes_actuales.s_I);
                            current_state = STATE_ALARM;
                        } else if (corrientes_actuales.x_I > 3) {
                            GUI_ERROR("Sobrecarga en el motor X: %.2f A", corrientes_actuales.x_I);
                            current_state = STATE_ALARM;
                        } else if (corrientes_actuales.y_I > 3) {
                            GUI_ERROR("Sobrecarga en el motor Y: %.2f A", corrientes_actuales.y_I);
                            current_state = STATE_ALARM;
                        } else if (corrientes_actuales.z_I > 3) {
                            GUI_ERROR("Sobrecarga en el motor Z: %.2f A", corrientes_actuales.z_I);
                            current_state = STATE_ALARM;
                        }
                    }
                    
                    break;

                case STATE_PAUSE:
                    GUI_INFO("Continuar el proceso presionando Start");
                    GUI_WARN("Ruteadora encendida al pausar el proceso");
                    // Apagar motores inmediatamente pero mantener ruteadora encendida
                    // motores_disable();
                    break;

                case STATE_ALARM:
                    GUI_INFO("Proceso detenido completamente");
                    GUI_WARN("Pérdida de pasos");
                    // Apagar motores inmediatamente y detener después de un segundo el spindle
                    // ruteadora_off_delay();
                    // motores_disable();
                    break;
                default: //Por seguridad
                    break;
            }
            last_reported_state = current_state; //Guardamos el estado actual
        }
        if ((xTaskGetTickCount() - ultima_lectura_rtc) >= pdMS_TO_TICKS(1000)) { //Cada segundo
            ultima_lectura_rtc = xTaskGetTickCount(); // Actualizaz el contador
            ds1307_read_time(); // enviará y mostrará la hora actual por UART
        }
        vTaskDelay(pdMS_TO_TICKS(20)); // Pequeña espera para no saturar CPU
    }
}