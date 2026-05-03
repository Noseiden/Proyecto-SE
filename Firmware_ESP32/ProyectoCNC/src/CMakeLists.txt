# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

idf_component_register(
    SRCS 
        "main.c"
        "current_sensor.c"
        "logging.c"
        "motors.c"
        "router_spindle.c"
        "rtc_ds1307.c"
    INCLUDE_DIRS 
        "."             # Para buscar en la misma carpeta src
        "${CMAKE_CURRENT_SOURCE_DIR}/../include" # Ruta absoluta a tu carpeta include
)