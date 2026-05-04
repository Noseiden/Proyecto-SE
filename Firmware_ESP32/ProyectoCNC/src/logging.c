#include "logging.h"
#include "system_lib.h"
void gui_send_log(const char *level, const char *format, ...) {
    char buffer[128]; //Hasta 128 caracteres
    int len = sprintf(buffer, "[%s] ", level); //Etiqueta inicial, ej: "[ERROR] "
    
    //Se extraen las variables y se ponen en el texto con stdarg:
    va_list args;
    va_start(args, format);
    len += vsnprintf(buffer + len, sizeof(buffer) - len, format, args);
    va_end(args);

    //Añadimos un salto de línea vital para que Python sepa dónde termina el mensaje:
    if (len < sizeof(buffer) - 1) {
        buffer[len++] = '\n';
        buffer[len] = '\0';
    }

    uart_write_bytes(UART_PORT, buffer, len); //Enviamos los bytes exactos a Python
}

