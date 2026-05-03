#include "current_sensor.h"
#include "system_lib.h"
adc_oneshot_unit_handle_t adc1_handle;
uint64_t timer_value = 0; // timer para ADCs
int adc_raw_IS, adc_raw_IX, adc_raw_IY, adc_raw_IZ;
int IS, IX, IY, IZ;
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
void read_I_sensor(void){
    timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &timer_value); //para muestreo del ADC
    if(timer_value >= SAMPLE_PERIOD_US){
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_0, &adc_raw_IS);
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_3, &adc_raw_IX);
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &adc_raw_IY);
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_7, &adc_raw_IZ);
        //Valor digital recibido: número entero de 0 a 4095, pues es de 12 bits, entonces niveles = 2^12
        IS = (adc_raw_IS * 100) / 4095; //en porcentaje
        IX = (adc_raw_IX * 100) / 4095; //en porcentaje
        IY = (adc_raw_IY * 100) / 4095; //en porcentaje
        IZ = (adc_raw_IZ * 100) / 4095; //en porcentaje
        timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    }
}
