//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### HARD.H ################################
//---------------------------------------------
#ifndef _HARD_H_
#define _HARD_H_

#include "stm32f0xx.h"


//-- Defines For Configuration -------------------
//---- Configuration for Hardware Versions -------
#define HARD_VER_1_3
// #define HARD_VER_1_2
// #define HARD_VER_1_1
// #define HARD_VER_1_0

//---- Configuration for Firmware ----------------
#define SOFT_VER_1_8          //agrego funcionalidad 1ch paralelo
// #define SOFT_VER_1_7

// #define WITH_GRANDMASTER	//tiene un quinto canal que funciona como grandmaster de los 4 primeros

// #define RGB_FOR_CHANNELS    //solo 2ch con dimmer fijo
#define RGB_FOR_PROGRAMS    //elije programas y mueve los 4ch en secuencias predefinidas  

//------ Configuration for Firmware-Channels -----
#ifdef RGB_FOR_CHANNELS
#define RGB_FOR_CHANNELS_NUM	2
//#define RGB_FOR_CHANNELS_NUM	4
#if (RGB_FOR_CHANNELS_NUM == 2)		//SYNCHRO
//#define RGB_FOR_CHANNELS_CH1_CH3_SYNC
//#define RGB_FOR_CHANNELS_CH2_CH4_SYNC
#endif
#endif

//---- Configuration for Firmware-Programs --------
#ifdef RGB_FOR_PROGRAMS
#define WHITE_AS_IN_RGB		//el blanco lo forma con los 3 colores
//#define WHITE_AS_WHITE	//el blanco tiene leds blancos individuales
#endif
//---- Configuration for Outputs -----------------
#define RGB_OUTPUT_LM317
//#define RGB_OUTPUT_MOSFET_KIRNO	//muestra CAT en display
//#define RGB_OUTPUT_CAT			//muestra CAT en display

//-- End Of Defines For Configuration ---------------

//GPIOA pin1
#define LED ((GPIOA->ODR & 0x0002) != 0)
#define LED_ON	GPIOA->BSRR = 0x00000002
#define LED_OFF GPIOA->BSRR = 0x00020000

//GPIOA pin2
#define S1_PIN ((GPIOA->IDR & 0x0004) == 0)

//GPIOA pin3
#define S2_PIN ((GPIOA->IDR & 0x0008) == 0)

//GPIOA pin4
#define CTRL_FAN ((GPIOA->ODR & 0x0010) != 0)
#define CTRL_FAN_ON GPIOA->BSRR = 0x00000010
#define CTRL_FAN_OFF GPIOA->BSRR = 0x00100000

//GPIOA pin5
#define PWR_DS3 ((GPIOA->ODR & 0x0020) != 0)
#define PWR_DS3_ON GPIOA->BSRR = 0x00000020
#define PWR_DS3_OFF GPIOA->BSRR = 0x00200000

//GPIOA pin6
//GPIOA pin7

//GPIOA pin8
#define DMX_INPUT ((GPIOA->IDR & 0x0100) != 0)

//GPIOA pin9
//GPIOA pin10
//GPIOA pin11
#define PWR_DS2 ((GPIOA->ODR & 0x0800) != 0)
#define PWR_DS2_ON GPIOA->BSRR = 0x00000800
#define PWR_DS2_OFF GPIOA->BSRR = 0x08000000

//GPIOA pin12
#define PWR_DS1 ((GPIOA->ODR & 0x1000) != 0)
#define PWR_DS1_ON GPIOA->BSRR = 0x00001000
#define PWR_DS1_OFF GPIOA->BSRR = 0x10000000

//GPIOA pin13
//GPIOA pin14
//GPIOA pin15

//GPIOB pin0
//GPIOB pin1
//GPIOB pin3
//GPIOB pin4
//GPIOB pin5

//GPIOB pin6
#define OE ((GPIOB->ODR & 0x0040) != 0)
#define OE_OFF GPIOB->BSRR = 0x00000040
#define OE_ON GPIOB->BSRR = 0x00400000


#define CH_IN_TEMP ADC_Channel_0
#define LAST_NUMBER		5
#define LAST_NUMBER_SPEAK		5

#define DOOR_ROOF	200
#define DOOR_THRESH	180


//ESTADOS DEL PROGRAMA PRINCIPAL
#define MAIN_INIT					0

#define MAIN_DMX_CHECK_CHANNEL				10
#define MAIN_DMX_CHECK_CHANNEL_B			11
#define MAIN_DMX_CHECK_CHANNEL_SELECTED		12
#define MAIN_DMX_CHECK_CHANNEL_S1			13
#define MAIN_DMX_CHECK_CHANNEL_S2			14
#define MAIN_DMX_SAVE_CONF					15
#define MAIN_DMX_NORMAL						16

#define MAIN_MAN_PX_CHECK			20
#define MAIN_MAN_PX_CHECK_B			21
#define MAIN_MAN_PX_CHECK_DEEP		22
#define MAIN_MAN_PX_CHECK_S1		23
#define MAIN_MAN_PX_CHECK_S2		24
#define MAIN_MAN_PX_SAVE_CONF		25
#define MAIN_MAN_PX_NORMAL			26

#define MAIN_TEMP_OVERLOAD			30
#define MAIN_TEMP_OVERLOAD_B		31

//---- Temperaturas en el LM335
//37	2,572
//40	2,600
//45	2,650
//50	2,681
//55	2,725
//60	2,765
#define TEMP_IN_30		3226
#define TEMP_IN_35		3279
#define TEMP_IN_50		3434
#define TEMP_IN_65		3591
#define TEMP_DISCONECT		4000

#define SWITCHES_TIMER_RELOAD	10
#define SWITCHES_THRESHOLD_FULL	300		//3 segundos
#define SWITCHES_THRESHOLD_HALF	100		//1 segundo
#define SWITCHES_THRESHOLD_MIN	5		//50 ms

#define TT_CHANNELS_DISPLAY     2000    //tiempo entre que dice canal y el numero
#define TT_CHANNELS_SW          200     //timeout para incrementar dimmer en channels
#define TIMER_STANDBY_TIMEOUT_REDUCED	2000	//reduced 2 segs
#define TIMER_STANDBY_TIMEOUT			6000	//6 segundos
#define DMX_DISPLAY_SHOW_TIMEOUT		30000	//30 segundos

typedef enum {
    S_NO = 0,
    S_MIN,
    S_HALF,
    S_FULL

} switch_state_t;

#define FUNCTION_DMX	1
#define FUNCTION_MAN	2
#define FUNCTION_CAT	FUNCTION_MAN

//--- Exported Module Functions ------------
void UpdateSwitches (void);
switch_state_t CheckS1 (void);
switch_state_t CheckS2 (void);

#endif /* _HARD_H_ */
