# INTRODUCCIÓN

## Las máquinas CNC (Control Numérico Computarizado) son ampliamente utilizadas en procesos de manufactura debido a su capacidad para producir piezas con alta precisión y repetibilidad. Estas máquinas permiten controlar el movimiento de herramientas de corte mediante trayectorias programadas, posibilitando la fabricación de geometrías complejas a partir de materiales sólidos. En muchos sistemas CNC de arquitectura abierta o de bajo costo, el desplazamiento de los ejes se realiza mediante motores paso a paso operando en lazo abierto. Bajo ciertas condiciones, como sobrecargas mecánicas, aceleraciones excesivas o configuraciones incorrectas de operación, puede presentarse el fenómeno conocido como stepper motor skipping o pérdida de pasos. Este evento ocurre cuando el motor no logra ejecutar un paso ordenado por el controlador, provocando una discrepancia entre la posición teórica y la posición real del sistema, lo que puede generar errores en el mecanizado o incluso comprometer la integridad del proceso. Con el fin de mejorar la confiabilidad del sistema, en este proyecto se propone el desarrollo de una máquina CNC compacta de tres ejes basada en un microcontrolador ESP32. El sistema integrará un conjunto de mecanismos de supervisión capaces de detectar condiciones anómalas durante la operación, tales como pérdida de pasos en los motores y consumos de corriente elevados. Adicionalmente, se desarrollará un firmware embebido encargado de gestionar la lógica de control, registrar eventos relevantes del sistema y comunicar el estado de la máquina al usuario mediante una interfaz gráfica.

# ALCANCE DEL PROYECTO

## El proyecto consiste en el diseño e implementación de una máquina CNC de tres ejes controlada mediante un microcontrolador ESP32, incorporando un sistema embebido orientado al monitoreo y supervisión del sistema de movimiento. El sistema integrará electrónica de potencia para el control de motores, sensores para el monitoreo de variables relevantes y un firmware encargado de gestionar la lógica de operación, el registro de eventos (logging) y la comunicación con el usuario. Asimismo, se implementará una interfaz gráfica que permitirá visualizar el estado del sistema y reportar advertencias o fallas detectadas durante la operación. El alcance del proyecto incluye el diseño de la arquitectura del sistema, la implementación del firmware, la integración del hardware y la construcción de un prototipo funcional para su demostración. No se contempla el desarrollo de software CAM ni la optimización industrial del proceso de mecanizado.

# OBJETIVO GENERAL

## Desarrollar una máquina CNC compacta de tres ejes basada en un sistema embebido con microcontrolador ESP32 que permita supervisar el funcionamiento del sistema de movimiento.

# OBJETIVOS ESPECÍFICOS

## a) Diseñar la arquitectura de hardware y firmware del sistema embebido encargado del control y supervisión de la máquina.

## b) Implementar un mecanismo de detección de pérdida de pasos.

## c) Desarrollar un sistema de monitoreo de corriente para identificar condiciones anormales de operación.

## d) Implementar estrategias de reacción ante fallas, como la detección segura del proceso o la generación de alertas 	.

## e) Diseñar una interfaz gráfica que permita visualizar el estado del sistema y los eventos registrados.

## f) Integrar físicamente el sistema mediante una PCB o tarjeta universal soldada para la construcción del prototipo funcional.
    