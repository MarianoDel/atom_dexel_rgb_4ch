//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### FLASH_PROGRAM.H #################################
//---------------------------------------------

#ifndef _FLASH_PROGRAM_H_
#define _FLASH_PROGRAM_H_

#include "stm32f0xx.h"
#include <stdint.h>

// Define the STM32F10x FLASH Page Size depending on the used STM32 device
// si es mayor a 128K la pagina es de 2KB, sino 1KB

#define FLASH_PAGE_SIZE		1024
#define FLASH_PAGE_SIZE_DIV2	512
#define FLASH_PAGE_SIZE_DIV4	256

#define PAGE28_START	0
#define PAGE29_START	256
#define PAGE30_START	512
#define PAGE31_START	768
#define PAGE31_END		1023

#define PAGE27			((uint32_t)0x08006C00)
#define PAGE28			((uint32_t)0x08007000)
#define PAGE29			((uint32_t)0x08007400)
#define PAGE30			((uint32_t)0x08007800)
#define PAGE31			((uint32_t)0x08007C00)

typedef union mem_bkp {
		unsigned int v_bkp [FLASH_PAGE_SIZE_DIV4];
		unsigned char v_bkp_8u [FLASH_PAGE_SIZE];
		unsigned short v_bkp_16u [FLASH_PAGE_SIZE_DIV2];
} mem_bkp_typedef;

typedef struct parameters {

	unsigned char last_function_in_flash;

	unsigned char last_program_in_flash;
	unsigned char last_program_deep_in_flash;
	unsigned char dummy;	//corrige desplazamiento de memoria

	unsigned short last_channel_in_flash;
	unsigned char dummy1;	//corrige desplazamiento de memoria
	unsigned char dummy2;	//corrige desplazamiento de memoria

	//agregados para RGB_FOR_CAT
	unsigned char pwm_channel_1;
	unsigned char pwm_channel_2;
	unsigned char pwm_channel_3;
	unsigned char pwm_channel_4;

} parameters_typedef;


typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

typedef enum
{
    FLASH_BUSY = 1,
    FLASH_ERROR_WRP,
    FLASH_ERROR_PROGRAM,
    FLASH_COMPLETE,
    FLASH_TIMEOUT
    
} FLASH_Status;

#define FLASH_ER_PRG_TIMEOUT         ((uint32_t)0x000B0000)
#define FLASH_FLAG_BSY                 FLASH_SR_BSY     /*!< FLASH Busy flag */
#define FLASH_FLAG_WRPERR              FLASH_SR_WRPERR  /*!< FLASH Write protected error flag */

//-------- Exported Functions -------------
unsigned char WriteConfigurations (void);
unsigned char WriteFlash(unsigned int * p, uint32_t p_addr, unsigned char with_lock, unsigned char len_in_4);

#endif
