//------------------------------------------------
// #### PROYECTO DEXEL 4CH #######################
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MAIN.C ###################################
//------------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "hard.h"
#include "stm32f0xx.h"
#include "gpio.h"
#include "uart.h"
#include "spi.h"

#include "core_cm0.h"
#include "adc.h"
#include "flash_program.h"
#include "programs_functions.h"

#include "tim.h"
#include "display_7seg.h"
#include "dsp.h"

//#include <stdio.h>
//#include <string.h>


// Externals --------------------------------------------
volatile unsigned char timer_1seg = 0;
volatile unsigned char display_timer = 0;
volatile unsigned short timer_led_comm = 0;
volatile unsigned short timer_for_channels_switch = 0;
volatile unsigned short timer_for_channels_display = 0;
volatile unsigned char buffrx_ready = 0;
volatile unsigned char *pbuffrx;
volatile unsigned short wait_ms_var = 0;

volatile unsigned char TxBuffer_SPI [TXBUFFERSIZE];
volatile unsigned char RxBuffer_SPI [RXBUFFERSIZE];
volatile unsigned char *pspi_tx;
volatile unsigned char *pspi_rx;
volatile unsigned char spi_bytes_left = 0;

// ------- Externals del DMX -------
volatile unsigned char Packet_Detected_Flag;
volatile unsigned char dmx_receive_flag = 0;
volatile unsigned char DMX_channel_received = 0;
volatile unsigned char DMX_channel_selected = 1;
volatile unsigned char DMX_channel_quantity = 4;

volatile unsigned char data1[512];
//static unsigned char data_back[10];
volatile unsigned char data[10];

volatile unsigned short prog_timer = 0;

// --- Externals del o para los Switches -------
volatile unsigned char switches_timer;

// --- Externals del display -------
unsigned char ds3_number = 0;
unsigned char ds2_number = 0;
unsigned char ds1_number = 0;
unsigned char display_blinking = 0;

// Globals ----------------------------------------
parameters_typedef param_struct;


// ------- de los timers -------
volatile unsigned short timer_standby;
volatile unsigned short timer_dmx_display_show;
volatile unsigned char filter_timer;
volatile unsigned char take_sample;


// ------- del DMX -------
volatile unsigned char signal_state = 0;
volatile unsigned char dmx_timeout_timer = 0;
//unsigned short tim_counter_65ms = 0;

// ------- de los filtros DMX -------
#define DMX_FILTER_LENGTH    32
unsigned char vd0 [DMX_FILTER_LENGTH];
unsigned char vd1 [DMX_FILTER_LENGTH];
unsigned char vd2 [DMX_FILTER_LENGTH];
unsigned char vd3 [DMX_FILTER_LENGTH];
unsigned char vd4 [DMX_FILTER_LENGTH];

#define TEMP_FILTER_LENGTH    16
unsigned short vtemp [TEMP_FILTER_LENGTH];


#define IDLE	0
#define LOOK_FOR_BREAK	1
#define LOOK_FOR_MARK	2
#define LOOK_FOR_START	3

//--- FUNCIONES DEL MODULO ---//
void TimingDelay_Decrement(void);
unsigned short Get_Temp (void);


// ------- del DMX -------
extern void EXTI4_15_IRQHandler(void);
#define DMX_TIMEOUT	20
void DMX_Ena(void);
void DMX_Disa(void);


//--- FIN DEFINICIONES DE FILTRO ---//


//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
    unsigned char main_state = 0;
    unsigned char last_function;
    unsigned char last_program, last_program_deep;
    unsigned short last_channel;
    unsigned short current_temp = 0;

#ifdef WITH_GRANDMASTER
    unsigned short acc = 0;
    unsigned char dummy = 0;
#endif
#ifdef RGB_FOR_CHANNELS
    unsigned char show_channels_state = 0;
    unsigned char fixed_data[2];		//la eleccion del usuario en los canales de 0 a 100
    unsigned char need_to_save = 0;
#endif
    parameters_typedef * p_mem_init;

    //GPIO Configuration.
    GPIO_Config();

    //TIM Configuration.
    TIM_3_Init();
    TIM_14_Init();

    //ACTIVAR SYSTICK TIMER
    if (SysTick_Config(48000))
    {
        while (1)	/* Capture error */
        {
            if (LED)
                LED_OFF;
            else
                LED_ON;

            Wait_ms(300);
        }
    }

    /* SPI configuration ------------------------------------------------------*/
    SPI_Config();

    Packet_Detected_Flag = 0;
    DMX_channel_selected = 1;
    DMX_channel_quantity = 4;
    USART1Config();

    //arranco con todo apagado
    DMX_Disa();

    //--- COMIENZO PROGRAMA DE PRODUCCION
    RED_PWM (0);
    GREEN_PWM (0);
    BLUE_PWM (0);
    WHITE_PWM (0);

    //inicio cuestiones particulares
    //iniciar variables de usao del programa segun funcion de memoria

    //ADC configuration.
    AdcConfig();

    LED_ON;
    Wait_ms(300);
    LED_OFF;

    //muestro versiones de hardware, software y firmware
    //-- HARDWARE --
#ifdef HARD_VER_1_3
    timer_standby = 1000;
    ds1_number = DISPLAY_H;				//Hardware
    ds2_number = DISPLAY_1P;			//1.
    ds3_number = 3;						//3
    while (timer_standby)
        UpdateDisplay();
#endif

#ifdef HARD_VER_1_2
    timer_standby = 1000;
    ds1_number = DISPLAY_H;				//Hardware
    ds2_number = DISPLAY_1P;			//1.
    ds3_number = 2;						//2
    while (timer_standby)
        UpdateDisplay();
#endif

#ifdef HARD_VER_1_1
    timer_standby = 1000;
    ds1_number = DISPLAY_H;				//Hardware
    ds2_number = DISPLAY_1P;			//1.
    ds3_number = 1;						//1
    while (timer_standby)
        UpdateDisplay();
#endif

#ifdef HARD_VER_1_0
    timer_standby = 1000;
    ds1_number = DISPLAY_H;				//Hardware
    ds2_number = DISPLAY_1P;			//1.
    ds3_number = DISPLAY_ZERO;			//0
    while (timer_standby)
        UpdateDisplay();
#endif

//-- SOFTWARE --
#ifdef SOFT_VER_1_8
    timer_standby = 1000;
    ds1_number = DISPLAY_S;    //S
    ds2_number = DISPLAY_1P;   //1.
    ds3_number = 8;            //8
    while (timer_standby)
        UpdateDisplay();
#endif

#ifdef SOFT_VER_1_7
    timer_standby = 1000;
    ds1_number = DISPLAY_S;	   //S
    ds2_number = DISPLAY_1P;   //1.
    ds3_number = 7;		   //7
    while (timer_standby)
        UpdateDisplay();
#endif
        
//-- FIRMWARE --
#ifdef RGB_FOR_CHANNELS
    timer_standby = 1000;
    ds1_number = DISPLAY_C;				//Channels
    ds2_number = DISPLAY_H;				//
    ds3_number = DISPLAY_N;				//
    while (timer_standby)
        UpdateDisplay();
#endif

#ifdef RGB_FOR_PROGRAMS
    timer_standby = 1000;
    ds1_number = DISPLAY_P;				//PRG
    ds2_number = DISPLAY_R;				//
    ds3_number = DISPLAY_G;				//
    while (timer_standby)
        UpdateDisplay();
#endif

//-- OUTPUTS --
#ifdef RGB_OUTPUT_LM317
    timer_standby = 1000;
    ds1_number = 3;						//LM317
    ds2_number = 1;						//
    ds3_number = 7;						//
    while (timer_standby)
        UpdateDisplay();
#endif

#if ((defined RGB_OUTPUT_MOSFET_KIRNO) || (defined RGB_OUTPUT_CAT))
    timer_standby = 1000;
    ds1_number = DISPLAY_C;				//CAT o MOSFET KIRNO
    ds2_number = DISPLAY_A;				//
    ds3_number = DISPLAY_T;				//
    while (timer_standby)
        UpdateDisplay();
#endif

    //--- Main loop ---//
    while(1)
    {
        switch (main_state)
        {
        case MAIN_INIT:
            //cargo las variables desde la memoria
            p_mem_init = (parameters_typedef *) (unsigned int *) PAGE31;
#ifdef RGB_FOR_PROGRAMS
            param_struct.last_function_in_flash = p_mem_init->last_function_in_flash;
            param_struct.last_program_in_flash = p_mem_init->last_program_in_flash;
            param_struct.last_program_deep_in_flash = p_mem_init->last_program_deep_in_flash;
            param_struct.last_channel_in_flash = p_mem_init->last_channel_in_flash;
#endif

#ifdef RGB_FOR_CHANNELS
            param_struct.last_function_in_flash = p_mem_init->last_function_in_flash;
            param_struct.last_program_in_flash = p_mem_init->last_program_in_flash;
            param_struct.last_program_deep_in_flash = p_mem_init->last_program_deep_in_flash;
            param_struct.last_channel_in_flash = p_mem_init->last_channel_in_flash;

            param_struct.pwm_channel_1 = p_mem_init->pwm_channel_1;
            if (param_struct.pwm_channel_1 > 100)
                param_struct.pwm_channel_1 = 100;

            param_struct.pwm_channel_2 = p_mem_init->pwm_channel_2;
            if (param_struct.pwm_channel_2 > 100)
                param_struct.pwm_channel_2 = 100;

            param_struct.pwm_channel_3 = p_mem_init->pwm_channel_3;
            if (param_struct.pwm_channel_3 > 100)
                param_struct.pwm_channel_3 = 100;

            param_struct.pwm_channel_4 = p_mem_init->pwm_channel_4;
            if (param_struct.pwm_channel_4 > 100)
                param_struct.pwm_channel_4 = 100;

#endif

            //reviso donde estaba la ultima vez
            if (param_struct.last_function_in_flash == 0xFF)	//memoria borrada
            {
                last_function = FUNCTION_DMX;
                last_program = 1;
                last_program_deep = 1;
                last_channel = 1;

#ifdef RGB_FOR_CHANNELS
                fixed_data[0] = 0;
                fixed_data[1] = 0;
#endif

            }
            else
            {
#ifdef RGB_FOR_PROGRAMS
                last_function = param_struct.last_function_in_flash;
                last_program = param_struct.last_program_in_flash;
                last_channel = param_struct.last_channel_in_flash;
                last_program_deep = param_struct.last_program_deep_in_flash;
#endif

#ifdef RGB_FOR_CHANNELS
                last_function = param_struct.last_function_in_flash;
                last_program = param_struct.last_program_in_flash;
                last_channel = param_struct.last_channel_in_flash;
                last_program_deep = param_struct.last_program_deep_in_flash;

                fixed_data[0] = param_struct.pwm_channel_1;
                fixed_data[1] = param_struct.pwm_channel_2;
#endif


            }

            if (last_function == FUNCTION_DMX)
            {
                FromChannelToDs(last_channel);		//muestro el ultimo canal DMX seleccionado
                DMX_channel_selected = last_channel;
                DMX_Ena();
                main_state = MAIN_DMX_NORMAL;
                timer_dmx_display_show = DMX_DISPLAY_SHOW_TIMEOUT;
            }
            else
            {
                DMX_Disa();
#ifdef RGB_FOR_PROGRAMS
                ds1_number = DISPLAY_PROG;
                ds2_number = last_program;
                ds3_number = last_program_deep;
                main_state = MAIN_MAN_PX_NORMAL;
#endif
#ifdef RGB_FOR_CHANNELS
                ds1_number = DISPLAY_C;
                ds2_number = DISPLAY_H;
                ds3_number = last_program;
                main_state = MAIN_MAN_PX_NORMAL;
#endif
            }
            break;

        case MAIN_DMX_CHECK_CHANNEL:
            //apago todos los canales antes de empezar
            RED_PWM (0);
            GREEN_PWM (0);
            BLUE_PWM (0);
            WHITE_PWM (0);

            FromChannelToDs(last_channel);		//muestro el ultimo canal DMX seleccionado
            timer_standby = TIMER_STANDBY_TIMEOUT;
            main_state++;
            break;

        case MAIN_DMX_CHECK_CHANNEL_B:
            //espero que se libere el switch teniendo en cuenta que puedo venir de MANUAL
            if (CheckS1() == S_NO)
            {
                timer_standby = TIMER_STANDBY_TIMEOUT;
                main_state = MAIN_DMX_CHECK_CHANNEL_SELECTED;
            }
            break;

        case MAIN_DMX_CHECK_CHANNEL_SELECTED:
            if (CheckS1() > S_NO)
                main_state = MAIN_DMX_CHECK_CHANNEL_S1;

            if (CheckS2() > S_NO)
                main_state = MAIN_DMX_CHECK_CHANNEL_S2;

            if (!timer_standby)
                main_state = MAIN_DMX_SAVE_CONF;

            break;

        case MAIN_DMX_CHECK_CHANNEL_S1:
            //espero que se libere el switch o poner S_HALF y luego forzar 0
            if (CheckS1() == S_NO)
            {
                //corro una posicion o empiezo blinking
                //segun donde este puede ser que tenga que ir a programas si
                //se daja aretado mas tiempo
                if (display_blinking & 0x07)
                {
                    //muevo el display en blinking
                    if (display_blinking & DISPLAY_DS3)
                        display_blinking = DISPLAY_DS1;
                    else
                        display_blinking <<= 1;
                }
                else	//no estaba en blinking, lo actico para DS1
                {
                    display_blinking = DISPLAY_DS1;
                }
                main_state = MAIN_DMX_CHECK_CHANNEL_SELECTED;
            }

            //me fijo si tengo que cambiar a MANUAL
            if (CheckS1() >= S_HALF)
            {
                main_state = MAIN_MAN_PX_CHECK;
            }

            timer_standby = TIMER_STANDBY_TIMEOUT;
            break;

        case MAIN_DMX_CHECK_CHANNEL_S2:
            //espero que se libere el switch o poner S_HALF y luego forzar 0
            if (CheckS2() == S_NO)
            {
                //sino hay blinking no doy bola
                //segun donde esta blinking sumo 0-5 o 0-9
                if (display_blinking & 0x07)
                {
                    switch (display_blinking)	//el canal dmx lo formo despues de guardar
                    {
                    case DISPLAY_DS1:	//desde 0 a 5
                        if (ds1_number == DISPLAY_ZERO)
                            ds1_number = 1;
                        else if (ds1_number < 5)
                            ds1_number++;
                        else
                            ds1_number = DISPLAY_ZERO;
                        break;

                    case DISPLAY_DS2:
                        if (ds2_number == DISPLAY_ZERO)
                            ds2_number = 1;
                        else if (ds2_number < 9)
                            ds2_number++;
                        else
                            ds2_number = DISPLAY_ZERO;
                        break;

                    case DISPLAY_DS3:
                        if (ds3_number == DISPLAY_ZERO)
                            ds3_number = 1;
                        else if (ds3_number < 9)
                            ds3_number++;
                        else
                            ds3_number = DISPLAY_ZERO;
                        break;
                    }
                    last_channel = FromDsToChannel();
                }

                main_state = MAIN_DMX_CHECK_CHANNEL_SELECTED;
            }
            timer_standby = TIMER_STANDBY_TIMEOUT;
            break;

        case MAIN_DMX_SAVE_CONF:
            display_blinking = 0;
            //se cambio el canal, hago el update
            if ((last_channel == 0) || (last_channel > 511))		//no puede ser canal 0 ni mas de 511
                last_channel = 1;

            DMX_channel_selected = last_channel;
            DMX_Disa();
#ifdef RGB_FOR_PROGRAMS
            //hago update de memoria y grabo
            param_struct.last_channel_in_flash = last_channel;
            param_struct.last_function_in_flash = FUNCTION_DMX;
            param_struct.last_program_in_flash = last_program;
            param_struct.last_program_deep_in_flash = last_program_deep;
            WriteConfigurations ();
#endif

#ifdef RGB_FOR_CHANNELS
            //hago update de memoria y grabo
            param_struct.last_channel_in_flash = last_channel;
            param_struct.last_function_in_flash = FUNCTION_DMX;
            param_struct.last_program_in_flash = last_program;
            param_struct.last_program_deep_in_flash = last_program_deep;

            param_struct.pwm_channel_1 = fixed_data[0];
            param_struct.pwm_channel_2 = fixed_data[1];
            WriteConfigurations ();
#endif
            main_state++;
            timer_dmx_display_show = DMX_DISPLAY_SHOW_TIMEOUT;
            DMX_Ena();
            break;

        case MAIN_DMX_NORMAL:
            if (Packet_Detected_Flag)
            {
                //llego un paquete DMX
                Packet_Detected_Flag = 0;
                //en data tengo la info
                //FromChannelToDs(data[0]);	//no muestro el valor actual, solo el canal 1


                /*
                //filtro en tramas DMX
                RED_PWM (MAFilter32_Byte(data[0], vd0));	//RED
                GREEN_PWM (MAFilter32_Byte(data[1], vd1));	//GREEN
                BLUE_PWM (MAFilter32_Byte(data[2], vd2));	//BLUE
                WHITE_PWM (MAFilter32_Byte(data[3], vd3));	//WHITE
                */

                /*
                  RED_PWM (data[0]);	//RED	la salida ahora la hace el filtro
                  GREEN_PWM (data[1]);	//GREEN
                  BLUE_PWM (data[2]);	//BLUE
                  WHITE_PWM (data[3]);	//WHITE
                */
            }

#ifdef WITH_GRANDMASTER
            if (!filter_timer)
            {
                filter_timer = 5;

                //data[9] = MAFilter32_Byte(data[4], vd4);
                //data[9] = data[4];

                acc = data[0] * data[4];
                data[5] = acc >> 8;
                acc = data[1] * data[4];
                data[6] = acc >> 8;
                acc = data[2] * data[4];
                data[7] = acc >> 8;
                acc = data[3] * data[4];
                data[8] = acc >> 8;

                RED_PWM (MAFilter32_Byte(data[5], vd0));	//RED
                GREEN_PWM (MAFilter32_Byte(data[6], vd1));	//GREEN
                BLUE_PWM (MAFilter32_Byte(data[7], vd2));	//BLUE
                WHITE_PWM (MAFilter32_Byte(data[8], vd3));	//WHITE

                //RED_PWM (MAFilter32_Byte(data[0], vd0));	//RED
                //GREEN_PWM (MAFilter32_Byte(data[1], vd1));	//GREEN
                //BLUE_PWM (MAFilter32_Byte(data[2], vd2));	//BLUE
                //WHITE_PWM (MAFilter32_Byte(data[3], vd3));	//WHITE
            }
#else
            if (!filter_timer)
            {
                //filter_timer = 100;		//para prueba con placa mosfet comparad con controldor chino
                filter_timer = 5;
                //TODO: integrar a canales
                RED_PWM (MAFilter32_Byte(data[0], vd0));	//RED
                GREEN_PWM (MAFilter32_Byte(data[1], vd1));	//GREEN
                BLUE_PWM (MAFilter32_Byte(data[2], vd2));	//BLUE
                WHITE_PWM (MAFilter32_Byte(data[3], vd3));	//WHITE
            }
#endif

            if ((CheckS1() > S_NO) || (CheckS2() > S_NO))
            {
                //si se toco un boton una vez solo prendo display
                if (!timer_dmx_display_show)
                {
                    timer_dmx_display_show = DMX_DISPLAY_SHOW_TIMEOUT;
                }
                else if (timer_dmx_display_show < (DMX_DISPLAY_SHOW_TIMEOUT - 1000))	//1 seg despues
                {
                    main_state = MAIN_DMX_CHECK_CHANNEL_SELECTED;
                    timer_standby = TIMER_STANDBY_TIMEOUT;
                }
                FromChannelToDs(last_channel);
            }

            if (!timer_dmx_display_show)
            {
                //debo apagar display
                ds1_number = DISPLAY_NONE;
                ds2_number = DISPLAY_NONE;
                ds3_number = DISPLAY_NONE;
            }
            break;

        case MAIN_MAN_PX_CHECK:
#ifdef RGB_FOR_PROGRAMS
            //vengo de la otra funcion, reviso cual fue el ultimo programa
            DMX_Disa();
            ds1_number = DISPLAY_PROG;
            ds2_number = last_program;		//program no puede ser menor a 1
            ds3_number = last_program_deep;	//program_deep no puede ser menor a 1

            display_blinking = DISPLAY_DS2;

            main_state++;
#endif
#ifdef RGB_FOR_CHANNELS
            //vengo de la otra funcion, reviso cual fue el ultimo canal usado
            DMX_Disa();
            LED_OFF;
            ds1_number = DISPLAY_C;
            ds2_number = DISPLAY_H;		//program no puede ser menor a 1
            ds3_number = last_program;	//program_deep no puede ser menor a 1

            display_blinking = 0;
            need_to_save = 1;			//tengo que grabar, vengo de DMX
            main_state++;
            //limpio variables last y PWM
            Func_For_Cat(0, 0);
#endif
            break;

        case MAIN_MAN_PX_CHECK_B:
            //verifico que se libere el boton, porque puede ser que venga de la otra funcion
            if (CheckS1() == S_NO)
            {
                main_state = MAIN_MAN_PX_CHECK_DEEP;
                timer_standby = TIMER_STANDBY_TIMEOUT;
            }
            break;

        case MAIN_MAN_PX_CHECK_DEEP:
#ifdef RGB_FOR_PROGRAMS
            if (CheckS1() > S_NO)
                main_state = MAIN_MAN_PX_CHECK_S1;

            if (CheckS2() > S_NO)
                main_state = MAIN_MAN_PX_CHECK_S2;

            if (!timer_standby)
                main_state = MAIN_MAN_PX_SAVE_CONF;

            //si estan cambiando el programa igual muestro como va quedando
            Func_PX_Ds(ds1_number, ds2_number, ds3_number);
#endif

#ifdef RGB_FOR_CHANNELS
            if (CheckS1() > S_NO)
                main_state = MAIN_MAN_PX_CHECK_S1;

            if (CheckS2() > S_NO)
            {
                main_state = MAIN_MAN_PX_CHECK_S2;
                need_to_save = 1;
            }

            //salgo de dos maneras
            //grabando o no grabando
            if (!timer_standby)
            {
                if (need_to_save)
                {
                    main_state = MAIN_MAN_PX_SAVE_CONF;
                    need_to_save = 0;
                }
                else
                    main_state = MAIN_MAN_PX_NORMAL;
            }


            //si estan cambiando el programa igual muestro como va quedando
            Func_For_Cat(fixed_data[0], fixed_data[1]);
#endif
            break;

        case MAIN_MAN_PX_CHECK_S1:
#ifdef RGB_FOR_PROGRAMS
            //espero que se libere el switch  o poner S_HALF y luego forzar 0
            if (CheckS1() == S_NO)
            {
                //corro una posicion o empiezo blinking
                //segun donde este puede ser que tenga que ir a programas
                if (display_blinking & 0x07)
                {
                    //muevo el display en blinking
                    if (display_blinking & DISPLAY_DS3)
                    {
                        display_blinking = DISPLAY_DS2;
                    }
                    else
                        display_blinking <<= 1;
                }
                else	//no estaba en blinking, lo activo para DS1
                {
                    display_blinking = DISPLAY_DS2;
                }
                main_state = MAIN_MAN_PX_CHECK_DEEP;
            }

            //me fijo si tengo que cambiar a DMX
            if (CheckS1() >= S_HALF)
            {
                //salto a dmx
                DMX_Ena();
                main_state = MAIN_DMX_CHECK_CHANNEL;
                display_blinking = DISPLAY_DS1;
            }

            timer_standby = TIMER_STANDBY_TIMEOUT;
#endif
#ifdef RGB_FOR_CHANNELS
            //espero que se libere el switch  o poner S_HALF y luego forzar 0
            if (CheckS1() == S_NO)
            {
                //TODO: integrar a canales
                if (last_program < RGB_FOR_CHANNELS_NUM)
                    last_program++;
                else
                    last_program = 1;

                ds1_number = DISPLAY_C;
                ds2_number = DISPLAY_H;
                ds3_number = last_program;
                main_state = MAIN_MAN_PX_CHECK_DEEP;
            }

            //me fijo si tengo que cambiar a DMX
            if (CheckS1() >= S_HALF)
            {
                //salto a dmx
                DMX_Ena();
                main_state = MAIN_DMX_CHECK_CHANNEL;
                display_blinking = DISPLAY_DS1;
            }

            timer_standby = TIMER_STANDBY_TIMEOUT_REDUCED;
            //timer_standby = TIMER_STANDBY_TIMEOUT;
#endif
            break;

        case MAIN_MAN_PX_CHECK_S2:
#ifdef RGB_FOR_PROGRAMS
            //espero que se libere el switch  o poner S_HALF y luego forzar 0
            if (CheckS2() == S_NO)
            {
                //sino hay blinking no doy bola
                //muevo DS3 desde 1 a 9 cambiando profundidad
                if (display_blinking & DISPLAY_DS3)
                {
                    if (ds3_number < 9)
                        ds3_number++;
                    else
                        ds3_number = 1;		//no puede ser 0

                    last_program_deep = ds3_number;
                }

                //muevo DS2 desde 1 a 9 cambiando programas
                if (display_blinking & DISPLAY_DS2)
                {
                    if (ds2_number < 9)
                        ds2_number++;
                    else
                        ds2_number = 1;

                    last_program = ds2_number;
                }
                main_state = MAIN_MAN_PX_CHECK_DEEP;
            }
            timer_standby = TIMER_STANDBY_TIMEOUT;
#endif
#ifdef RGB_FOR_CHANNELS
            switch (last_program)
            {
            case 1:
                if (!timer_for_channels_switch)
                {
                    if (fixed_data[0] < 100)
                        fixed_data[0]++;
                    else
                        fixed_data[0] = 0;

                    FromChannelToDs(fixed_data[0]);
                    timer_for_channels_switch = TT_CHANNELS_SW;
                }
                break;

            case 2:
                if (!timer_for_channels_switch)
                {
                    if (fixed_data[1] < 100)
                        fixed_data[1]++;
                    else
                        fixed_data[1] = 0;

                    FromChannelToDs(fixed_data[1]);
                    timer_for_channels_switch = TT_CHANNELS_SW;
                }
                break;

            default:
                last_program = 1;
                break;
            }
            main_state = MAIN_MAN_PX_CHECK_DEEP;

            timer_standby = TIMER_STANDBY_TIMEOUT;
#endif
            break;

        case MAIN_MAN_PX_SAVE_CONF:
            DMX_Disa();
            display_blinking = 0;
#ifdef RGB_FOR_PROGRAMS
            //hago update de memoria y grabo
            param_struct.last_channel_in_flash = last_channel;
            param_struct.last_function_in_flash = FUNCTION_MAN;
            param_struct.last_program_in_flash = last_program;
            param_struct.last_program_deep_in_flash = last_program_deep;
            WriteConfigurations ();
#endif

#ifdef RGB_FOR_CHANNELS
            param_struct.last_channel_in_flash = last_channel;
            param_struct.last_function_in_flash = FUNCTION_CAT;
            param_struct.last_program_in_flash = last_program;
            param_struct.last_program_deep_in_flash = last_program_deep;

            param_struct.pwm_channel_1 = fixed_data[0];
            param_struct.pwm_channel_2 = fixed_data[1];
            WriteConfigurations ();
#endif

            main_state++;
            break;

        case MAIN_MAN_PX_NORMAL:
#ifdef RGB_FOR_PROGRAMS
            Func_PX(last_program, last_program_deep);

            if ((CheckS1() > S_NO) || (CheckS2() > S_NO))
            {
                main_state = MAIN_MAN_PX_CHECK_DEEP;
                timer_standby = TIMER_STANDBY_TIMEOUT;
            }
#endif
#ifdef RGB_FOR_CHANNELS
            Func_For_Cat(fixed_data[0], fixed_data[1]);

            if ((CheckS1() > S_NO) || (CheckS2() > S_NO))
            {
                main_state = MAIN_MAN_PX_CHECK_DEEP;
                timer_standby = TIMER_STANDBY_TIMEOUT;
            }

            if (!timer_for_channels_display)
            {
                switch (show_channels_state)
                {
                case SHOW_CHANNELS:
                    ds1_number = DISPLAY_C;
                    ds2_number = DISPLAY_H;		//program no puede ser menor a 1
                    ds3_number = last_program;	//program_deep no puede ser menor a 1
                    show_channels_state = SHOW_NUMBERS;
                    break;

                case SHOW_NUMBERS:
                    switch (last_program)
                    {
                    case 1:
                        FromChannelToDs(fixed_data[0]);
                        break;

                    case 2:
                        FromChannelToDs(fixed_data[1]);
                        break;

                    default:
                        last_program = 1;
                        break;
                    }
                    show_channels_state = SHOW_CHANNELS;
                    break;

                default:
                    show_channels_state = SHOW_NUMBERS;
                    break;
                }
                timer_for_channels_display = TT_CHANNELS_DISPLAY;
            }
#endif

            break;

        case MAIN_TEMP_OVERLOAD:
            //corto completo y dejo prendiendo y apagando el led cada 100ms
            //excepto FAN que lo pongo a maximo
            CTRL_FAN_ON;
            //DMX_Disa();	//por ahora no corto el DMX
            RED_PWM (0);
            GREEN_PWM (0);
            BLUE_PWM (0);
            WHITE_PWM (0);

            //con ds1_number, ds2_number y ds3_number tengo la info de display
            ds1_number = DISPLAY_NONE;
            ds2_number = DISPLAY_NONE;
            ds3_number = DISPLAY_E;

            main_state++;
            break;

        case MAIN_TEMP_OVERLOAD_B:
            if (!timer_standby)
            {
                timer_standby = 100;
                if (LED)
                    LED_OFF;
                else
                    LED_ON;
            }

            if (current_temp > TEMP_DISCONECT)
            {
                ds2_number = DISPLAY_E;
            }

            if (current_temp < TEMP_IN_50)
            {
                if (last_function == FUNCTION_DMX)
                {
                    FromChannelToDs(last_channel);		//muestro el ultimo canal DMX seleccionado
                    DMX_channel_selected = last_channel;
                    main_state = MAIN_DMX_NORMAL;
                    timer_dmx_display_show = DMX_DISPLAY_SHOW_TIMEOUT;
                }
                else
                {
#ifdef RGB_FOR_CHANNELS
                    ds1_number = DISPLAY_C;
                    ds2_number = DISPLAY_H;
                    ds3_number = last_program;
                    ResetLastValues();
#else
                    ds1_number = DISPLAY_PROG;
                    ds2_number = last_program;
                    ds3_number = last_program_deep;
#endif
                    main_state = MAIN_MAN_PX_NORMAL;
                }
            }
            break;

        default:
            main_state = MAIN_INIT;
            break;

        }

        UpdateDisplay ();
        UpdateSwitches ();

        //sensado de temperatura
        if (!take_sample)
        {
            take_sample = 10;	//tomo muestra cada 10ms
            current_temp = Get_Temp();

            if ((main_state != MAIN_TEMP_OVERLOAD) && (main_state != MAIN_TEMP_OVERLOAD_B))
            {
                if (current_temp > TEMP_IN_65)
                {
                    //corto los leds	ver si habia DMX cortar y poner nuevamente
                    main_state = MAIN_TEMP_OVERLOAD;
                }
                else if (current_temp > TEMP_IN_35)
                    CTRL_FAN_ON;
                else if (current_temp < TEMP_IN_30)
                    CTRL_FAN_OFF;
            }
        }
    }	//termina while(1)

    return 0;
}


//--- End of Main ---//
unsigned short Get_Temp (void)
{
    return MAFilter16 (ReadADC1_SameSampleTime (CH_IN_TEMP), vtemp);
}


void DMX_Ena(void)
{
	//habilito la interrupcion
	EXTIOn ();
	USART1_RX_ENA;
}


void DMX_Disa(void)
{
	//deshabilito la interrupcion
	EXTIOff ();
	USART1_RX_DISA;
}


void EXTI4_15_IRQHandler(void)		//nueva detecta el primer 0 en usart Consola PHILIPS
{
	unsigned short aux;

	if(EXTI->PR & 0x0100)	//Line8
	{
		//si no esta con el USART detecta el flanco	PONER TIMEOUT ACA?????
		if ((dmx_receive_flag == 0) || (dmx_timeout_timer == 0))
		//if (dmx_receive_flag == 0)
		{
			switch (signal_state)
			{
				case IDLE:
					if (!(DMX_INPUT))
					{
						//Activo timer en Falling.
						TIM14->CNT = 0;
						TIM14->CR1 |= 0x0001;
						signal_state++;
					}
					break;

				case LOOK_FOR_BREAK:
					if (DMX_INPUT)
					{
						//Desactivo timer en Rising.
						aux = TIM14->CNT;

						//reviso BREAK
						//if (((tim_counter_65ms) || (aux > 88)) && (tim_counter_65ms <= 20))
						//if ((aux > 87) && (aux < 210))	//Consola STARLET 6
						//if ((aux > 87) && (aux < 2000))		//Consola marca CODE tiene break 1.88ms
						if ((aux > 87) && (aux < 4600))		//Consola marca CODE modelo A24 tiene break 4.48ms fecha 11-04-17
						{
							//LED_ON;		//TODO: apaga para pruebas
							signal_state++;
							DMX_channel_received = 0;
							dmx_timeout_timer = DMX_TIMEOUT;		//activo el timeout para esperar un MARK valido
						}
						else	//falso disparo
							signal_state = IDLE;
					}
					else	//falso disparo
						signal_state = IDLE;

					TIM14->CR1 &= 0xFFFE;
					break;

				case LOOK_FOR_MARK:
					if ((!(DMX_INPUT)) && (dmx_timeout_timer))	//termino Mark after break
					{
						//ya tenia el serie habilitado
						dmx_receive_flag = 1;
						LED_ON;
					}
					else	//falso disparo
					{
						//termine por timeout
						dmx_receive_flag = 0;
					}
					signal_state = IDLE;
					//LED_OFF;						//TODO: apaga para pruebas
					break;

				default:
					signal_state = IDLE;
					break;
			}
		}
		EXTI->PR |= 0x0100;
	}
}

void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (display_timer)
        display_timer--;

    if (timer_standby)
        timer_standby--;

    if (switches_timer)
        switches_timer--;

    if (dmx_timeout_timer)
        dmx_timeout_timer--;

    if (timer_dmx_display_show)
        timer_dmx_display_show--;

    if (prog_timer)
        prog_timer--;

    if (take_sample)
        take_sample--;

    if (filter_timer)
        filter_timer--;

    if (timer_for_channels_switch)
        timer_for_channels_switch--;

    if (timer_for_channels_display)
        timer_for_channels_display--;

}

//--- end of file ---//
