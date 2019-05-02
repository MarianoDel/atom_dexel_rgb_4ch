# atom_dexel_rgb_4ch
BKP Dexel placa RGB Board v1.2 v1.1 v1.0


Makefile cambiado a Linux, arm-none-eabi-gcc ver 5.2.1

More line feeds
# Control Dexel RGB 4CH rs485
micro STM32F030K6T6
Placa RGB Board v1.2 v1.1 v1.0

Antes de empezar revisar seleccion del micro y configuracion
------------------------------------------------------------

* work area size y flash image
>stm32f0_flash.cfg

* work area size y flash image
>stm32f0_flash_lock.cfg

* work area size
>stm32f0_gdb.cfg

* work area size
>stm32f0_reset.cfg

* end of ram; stack size; memory lenght
>./cmsis_boot/startup/stm32_flash.ld

* elegir procesador definiciones en linea 68 -> pero modificar en Makefile
>./cmsis_boot/stm32f0xx.h
>.Makefile
