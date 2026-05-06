#include "current_sensor.h"
#include "system_lib.h"
adc_oneshot_unit_handle_t adc1_handle;
void I_sensor_init(void){
    timer_config_t config = {
        .divider = 80,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_DIS,
        .auto_reload = false,
    };
    timer_init(TIMER_GROUP_0, TIMER_0, &config); //Para muestreo del ADC 

    adc_oneshot_unit_init_cfg_t init_config ={ //Se usará solo ADC1
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&init_config, &adc1_handle);

    adc_oneshot_chan_cfg_t chan_config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT, //Default --> 12 bits
        .atten = ADC_ATTEN_DB_12, //voltaje de referencia: atenuación de 12dB, es decir, 0 a 3.3V
    };
    
    //Canal 0: GPIO36, sensor de corriente para ruteadora
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_0,
    &chan_config); 
    //Canal 3: GPIO39, sensor de corriente para motor X
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_3,
    &chan_config); 
    //Canal 6: GPIO34, sensor de corriente para motor Y
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6,
    &chan_config); 
    //Canal 7: GPIO35, sensor de corriente para motor Z
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_7,
    &chan_config);
    
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    timer_start(TIMER_GROUP_0, TIMER_0);
}
static float calc_I(int raw) { //static para que sea una función privada de este archivo.c
    //Trabajando en mV, pasar a A, según datasheet del sensor de corriente
    float mv = ((float)raw * 3300.0f) / 4095.0f; //f para indicar que es número punto flotante
    return (mv - 2500.0f) / 100.0f; //offset de 2.5V y 100mV/A de sensibilidad del sensor de corriente 20A
}
bool read_I_sensor(consumo_cnc_t *data){
    uint64_t timer_value; // timer para ADCs
    timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &timer_value); //para muestreo del ADC
    if(timer_value >= SAMPLE_PERIOD_US){
        int adc_raw;
        //Ingresar datos a la estructura data (de puntero *data a estructura):
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_0, &adc_raw); 
        data->s_I = calc_I(adc_raw); // -> significa ve a (*data) y accede a s_I
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_3, &adc_raw); 
        data->x_I = calc_I(adc_raw); //O también decir (*data).x_I
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &adc_raw);
        data->y_I = calc_I(adc_raw);
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_7, &adc_raw);
        data->z_I = calc_I(adc_raw);

        timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
        return true; //Hay datos leídos
    }
    return false; //Aún no ha pasado el tiempo de muestreo
}
