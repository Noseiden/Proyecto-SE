#include "logging.h"
#include "system_lib.h"
void gui_send_log(const char *level, const char *format, ...) { //Función variádica (argumentos inf.)
    char buffer[128]; //Hasta 128 caracteres
    int len = sprintf(buffer, "[%s] ", level); //Etiqueta inicial, ej: "[ERROR] "
    
    //Se extraen las variables y se ponen en el texto con stdarg:
    va_list args; //crea una lista de argumentos vacía
    va_start(args, format); // empieza a buscar argumentos justo después de variable format)
    len += vsnprintf(buffer + len, sizeof(buffer) - len, format, args); //Comom printf, pero crea un buffer con va_list como entrada
    va_end(args); //limpia la memoria de la pila de datos, es decir, cierra la lista

    //Añadir un salto de línea vital para que Python sepa dónde termina el mensaje:
    if (len < sizeof(buffer) - 1) {
        buffer[len++] = '\n';
        buffer[len] = '\0';
    }
    uart_write_bytes(UART_PORT, buffer, len); //Enviamos los bytes exactos a Python
}

