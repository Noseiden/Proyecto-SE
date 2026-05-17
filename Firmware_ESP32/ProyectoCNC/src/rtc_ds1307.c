#include "rtc_ds1307.h"
#include "system_lib.h"
#include "logging.h"
#define DS1307_ADDR 0x68
#define DS1307_REG_SEC 0X00 //registro segundos
#define I2C_PORT I2C_NUM_0
#define I2C_FREQ_HZ 100000 //Standard mode

uint8_t decimal2bcd(uint8_t decimal){
    // | es operación lógica OR entre bits 
    return ((decimal / 10) << 4) | (decimal % 10); // << significa desplazar n unidades a la izquierda
}

uint8_t bcd2decimal(uint8_t bcd){
    // & es operación lógica AND entre bits 
    return ((bcd << 4) * 10) + (bcd & 0x0F);
}

void i2c_init(void){
    i2c_config_t config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = RTC_SDA,
        .scl_io_num = RTC_SCL,
        .sda_pullup_en = GPIO_PULLDOWN_DISABLE,
        .scl_pullup_en = GPIO_PULLDOWN_DISABLE,
        .master.clk_speed = I2C_FREQ_HZ
    };
    i2c_param_config(I2C_PORT, &config);
    i2c_driver_install(I2C_PORT, config.mode, 0, 0, 0);
    GUI_INFO("BUS I2C inicializado correctamente");
}

void ds1307_write_hours(uint8_t seconds, uint8_t minutes, uint8_t hour, 
                        uint8_t day, uint8_t date, uint8_t month, uint8_t year){ //Configuración inicial, pues luego se actualizará automáticamente
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    //    Según datasheet:
    //(cmd es un formulario)
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (DS1307_ADDR << 1) | I2C_MASTER_WRITE, true); //true para que espere un ACK del esclavo
    i2c_master_write_byte(cmd, DS1307_REG_SEC, true);
    //A partir de acá, lo que me envíe empezará a escribirlo desde el 0x00
    // Y automáticamente aumentará address.
    i2c_master_write_byte(cmd, decimal2bcd(seconds), true);
    i2c_master_write_byte(cmd, decimal2bcd(minutes), true);
    i2c_master_write_byte(cmd, decimal2bcd(hour), true);
    i2c_master_write_byte(cmd, decimal2bcd(day), true);
    i2c_master_write_byte(cmd, decimal2bcd(date), true);
    i2c_master_write_byte(cmd, decimal2bcd(month), true);
    i2c_master_write_byte(cmd, decimal2bcd(year), true);
    i2c_master_stop(cmd);
    
    esp_err_t resultado = i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(1000));  //time-out, como un watchdog, si se pasa de ese tiempo, el master lo toma como error
    i2c_cmd_link_delete(cmd); //desconectar la comunicación con delete
    if (resultado == ESP_OK) { 
        GUI_INFO("Hora configurada correctamente en el DS1307"); 
    } 
    else { 
        GUI_ERROR("No se pudo escribir en el DS1307: %s", esp_err_to_name(resultado)); 
    } 
}

void ds1307_read_time(void) {  
    uint8_t datos[7]; // Buffer con los 7 bytes leídos (un byte por registro)
    i2c_cmd_handle_t cmd = i2c_cmd_link_create(); 
    // Leer desde la dirección 0x00. Inicialización:
    i2c_master_start(cmd); 
    i2c_master_write_byte(cmd, (DS1307_ADDR << 1) | I2C_MASTER_WRITE, true); // Dirección + bit de escritura (R/W = 0) 
    i2c_master_write_byte(cmd, DS1307_REG_SEC, true); // Enviar la dirección del registro desde se va a leer 
    
    // Nuevo start sin haber hecho stop --> el bus no se libera 
    i2c_master_start(cmd); // Comienza la lectura con el segundo start:
    // Dirección + bit de LECTURA (R/W = 1) 
    // El DS1307 ahora sabe que debe transmitir datos 
    i2c_master_write_byte(cmd, (DS1307_ADDR << 1) | I2C_MASTER_READ, true); // | I2C_MASTER_READ pone el bit R/W en 1 (leer) 
 
    // Leer bytes 0 al 5, enviando ACK después de cada uno:
    // ACK para confirmar la recepción correcta del byte y poder continuar
    i2c_master_read(cmd, datos, 6, I2C_MASTER_ACK); // Lee 6 bytes (seconds, ..., month) con ACK en cada uno 
 
    // El último byte (year) va con NACK 
    // NACK para detener el envío de datos de la comunicación
    i2c_master_read_byte(cmd, &datos[6], I2C_MASTER_NACK); 
 
    i2c_master_stop(cmd); //Terminar la transacción
 
    // Enviar el resultado:
    esp_err_t resultado = i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(1000)); 
    i2c_cmd_link_delete(cmd); 
 
    if (resultado != ESP_OK) { 
        GUI_ERROR("Error al leer del DS1307: %s", esp_err_to_name(resultado)); 
        return; 
    } else {
        GUI_ERROR(" "); 
        return;
    }

    // Cada byte viene en BCD, se convierte a decimal para mostrarlo:
    // Enmascarar, si no hacemos esto y CH=1, la conversión BCD daría un valor incorrecto 
    uint8_t segundos = bcd2decimal(datos[0] & 0x7F); 
    uint8_t minutos  = bcd2decimal(datos[1] & 0x7F); 
    uint8_t horas    = bcd2decimal(datos[2] & 0x3F); 
    uint8_t dia      = bcd2decimal(datos[3] & 0x07); 
    uint8_t fecha    = bcd2decimal(datos[4] & 0x3F); 
    uint8_t mes      = bcd2decimal(datos[5] & 0x1F); 
    uint8_t anio     = bcd2decimal(datos[6]); 
 
    //Enviar como Log:
    gui_send_log("RTC", "%02d/%02d/20%02d  %02d:%02d:%02d , Día semana: %d", 
             fecha, mes, anio, horas, minutos, segundos, dia); 
} 