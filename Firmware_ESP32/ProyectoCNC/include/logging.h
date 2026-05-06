#pragma once //para evitar que un archivo header sea incluido varias veces
void gui_send_log(const char *level, const char *format, ...);
//Máscaras para la función:
#define GUI_INFO(...)  gui_send_log("INFO", __VA_ARGS__) //__VA_ARGS__ para recibir número inf. de argumentos
#define GUI_WARN(...)  gui_send_log("WARN", __VA_ARGS__)
#define GUI_ERROR(...) gui_send_log("ERROR", __VA_ARGS__)
