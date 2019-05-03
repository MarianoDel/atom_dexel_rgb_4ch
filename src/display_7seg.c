//------------------------------------------------
// #### PROYECTO DEXEL 4CH #######################
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DISPLAY_7SEG.C ###########################
//------------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "display_7seg.h"
#include "hard.h"
#include "stm32f0xx.h"
#include "spi.h"

// #include <string.h>
// #include <stdio.h>


/* Externals variables ---------------------------------------------------------*/
extern unsigned char ds3_number;
extern unsigned char ds2_number;
extern unsigned char ds1_number;
extern unsigned char display_blinking;

//para el timer
extern volatile unsigned char display_timer;


/* Global variables ------------------------------------------------------------*/
unsigned char display_state = 0;
unsigned char display_blinking_timer = 0;
unsigned char display_was_on = 0;

// unsigned char display_last_digit = 0;
// char display_vector_numbers [LAST_NUMBER];
// char * p_vector_numbers;

// display_sm_t display_state = DISPLAY_SM_INIT;

//      dp g f e d c b a
//bits   7 6 5 4 3 2 1 0
//sin negar
const unsigned char vnumbers [25] = { 0x00, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67,
                                      0x3F, 0x80, 0x40, 0x63, 0x73, 0x79, 0x58, 0x74, 0x86, 0x6D,
                                      0x5F, 0x78, 0x50, 0x6F, 0x54 };


/* Module Functions ------------------------------------------------------------*/


void UpdateDisplay (void)
{
    unsigned char a;

    if (!display_timer)
    {
        switch (display_state)	//revisa a que display le toca
        {
        case 0:
            //primero reviso si tengo blink de digito
            if (display_blinking & DISPLAY_DS3)
            {
                //rutina blinking
                if (!display_blinking_timer)
                {
                    if (display_was_on)
                    {
                        a = TranslateNumber(DISPLAY_NONE);
                        SendSegment(DISPLAY_DS3, a);
                        display_was_on = 0;
                    }
                    else
                    {
                        a = TranslateNumber(ds3_number);
                        SendSegment(DISPLAY_DS3, a);
                        display_was_on =1;
                    }
                    display_blinking_timer = DISPLAY_BLINKING_UPDATE;
                }
                else
                {
                    if (display_was_on)		//si mostraba el numero sigo asi
                    {
                        a = TranslateNumber(ds3_number);
                        SendSegment(DISPLAY_DS3, a);
                    }
                    else
                    {
                        a = TranslateNumber(DISPLAY_NONE);
                        SendSegment(DISPLAY_DS3, a);
                    }
                    display_blinking_timer--;
                }
            }
            else	//si no muestro como siempre
            {
                a = TranslateNumber(ds3_number);
                SendSegment(DISPLAY_DS3, a);
            }
            display_state++;
            break;

        case 1:
            //primero reviso si tengo blink de digito
            if (display_blinking & DISPLAY_DS2)
            {
                //rutina blinking
                if (!display_blinking_timer)
                {
                    if (display_was_on)
                    {
                        a = TranslateNumber(DISPLAY_NONE);
                        SendSegment(DISPLAY_DS2, a);
                        display_was_on = 0;
                    }
                    else
                    {
                        a = TranslateNumber(ds2_number);
                        SendSegment(DISPLAY_DS2, a);
                        display_was_on =1;
                    }
                    display_blinking_timer = DISPLAY_BLINKING_UPDATE;
                }
                else
                {
                    if (display_was_on)		//si mostraba el numero sigo asi
                    {
                        a = TranslateNumber(ds2_number);
                        SendSegment(DISPLAY_DS2, a);
                    }
                    else
                    {
                        a = TranslateNumber(DISPLAY_NONE);
                        SendSegment(DISPLAY_DS2, a);
                    }
                    display_blinking_timer--;
                }
            }
            else	//si no muestro como siempre
            {
                a = TranslateNumber(ds2_number);
                SendSegment(DISPLAY_DS2, a);
            }
            display_state++;
            break;

        case 2:
            //primero reviso si tengo blink de digito
            if (display_blinking & DISPLAY_DS1)
            {
                //rutina blinking
                if (!display_blinking_timer)
                {
                    if (display_was_on)
                    {
                        a = TranslateNumber(DISPLAY_NONE);
                        SendSegment(DISPLAY_DS1, a);
                        display_was_on = 0;
                    }
                    else
                    {
                        a = TranslateNumber(ds1_number);
                        SendSegment(DISPLAY_DS1, a);
                        display_was_on =1;
                    }
                    display_blinking_timer = DISPLAY_BLINKING_UPDATE;
                }
                else
                {
                    if (display_was_on)		//si mostraba el numero sigo asi
                    {
                        a = TranslateNumber(ds1_number);
                        SendSegment(DISPLAY_DS1, a);
                    }
                    else
                    {
                        a = TranslateNumber(DISPLAY_NONE);
                        SendSegment(DISPLAY_DS1, a);
                    }
                    display_blinking_timer--;
                }
            }
            else	//si no muestro como siempre
            {
                a = TranslateNumber(ds1_number);
                SendSegment(DISPLAY_DS1, a);
            }
            display_state = 0;
            break;

        default:
            display_state = 0;
            break;
        }
        display_timer = DISPLAY_TIMER_RELOAD;
    }
}

//Traduce numeros y letras a segmentos
//del 1 al 9; 10 es cero; 11 es punto; 0 apagar; resto letras
unsigned char TranslateNumber (unsigned char number)
{
    if (number < sizeof (vnumbers))
        number = vnumbers[number];
    else
        number = 0;

#ifdef HARD_VER_1_0
        number = ~number;
#endif
    
    return number;
}


//Convierte los numeros mostrados por DS a numero de canal
unsigned short FromDsToChannel (void)	
{
    unsigned short ch;

    if (ds1_number == DISPLAY_ZERO)
        ch = 0;
    else
        ch = ds1_number * 100;

    if (ds2_number != DISPLAY_ZERO)
        ch += ds2_number * 10;

    if (ds3_number != DISPLAY_ZERO)
        ch += ds3_number;

    return ch;
}


// Convierte numeros a los 3 valores DS para poder mostrarlos
//DS1 centena
//DS2 decena
//DS3 unidades
void FromChannelToDs (unsigned short number)
{
    unsigned char a, b;

    a = number / 100;
    ds1_number = a;
    if (ds1_number == 0)
        ds1_number = 10;

    b = (number - a * 100) / 10;
    ds2_number = b;
    if (ds2_number == 0)
        ds2_number = 10;

    ds3_number = number - a * 100 - b * 10;
    if (ds3_number == 0)
        ds3_number = 10;
}


//Envia numeros o letras a uno de los 3 displays 7seg con hc595
//ver1.3 ver1.2 ver 1.1 ==> 1 hc595 y 3 tr
//ver1.0 ==> 2 hc595
void SendSegment (unsigned char display, unsigned char segment)
{
    unsigned char dbkp = 0;

    OE_OFF;

#if ((defined HARD_VER_1_3) || (defined HARD_VER_1_2) || (defined HARD_VER_1_1))

    PWR_DS1_OFF;
    PWR_DS2_OFF;
    PWR_DS3_OFF;

    dbkp = display;
    //Desplazando 1 a la izq para compensar error en hc595
    if (segment & 0x80)
    	display |= 1;
    else
    	display &= 0xFE;

    Send_SPI_Single (display);
    segment <<= 1;
    Send_SPI_Single (segment);

    if (dbkp == DISPLAY_DS1)
        PWR_DS1_ON;
    else if (dbkp == DISPLAY_DS2)
        PWR_DS2_ON;
    else if (dbkp == DISPLAY_DS3)
        PWR_DS3_ON;

#endif

#ifdef HARD_VER_1_0
    //Desplazando 1 a la izq para compensar error en hc595
    display <<= 1;
    if (segment & 0x80)
    	display |= 1;
    else
    	display &= 0xFE;

    Send_SPI_Single (display);
    segment <<= 1;
    Send_SPI_Single (segment);
#endif

    OE_ON;
}




// void ShowNumbersAgain (void)
// {
//     ShowNumbers (display_last_digit);
// }

// //	dp g f e d c b a
// //bits   7 6 5 4 3 2 1 0
// //negados
// const unsigned char v_display_numbers [] = { 0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
//                                              0x80, 0x98, 0x7F, 0xBF, 0x9C, 0x06, 0xB0, 0xA3,
//                                              0x8B, 0x92, 0x8C };

// //beware read the switches disables de displayed number
// //system calls ShowNumbersAgain to solve this
// void ShowNumbers (unsigned char number)	//from 0 to 9 are numbers; 10 to 15 are other symbols
// {
//     display_last_digit = number;
//     if ((number != DISPLAY_NONE) && (number < SIZEOF_VDISPLAY))
//         number = v_display_numbers[number];
//     else
//         number = 0xFF;

    
//     //unsigned char a;
//     //me fijo si lo saco con punto
//     //if (number > 128)

//     // number = number & 0x0F;
//     // switch (number)
//     // display_last_digit = number;
//     // switch (number)
//     // {
//     // case 10:
//     //     //number = 0x3F;
//     //     number = 0xC0;
//     //     break;

//     // case DISPLAY_ERROR:
//     //     //E
//     //     number = 0x06;
//     //     break;

//     // case DISPLAY_ERROR2:
//     //     //E inv
//     //     number = 0xB0;
//     //     break;

//     // case DISPLAY_LINE:
//     //     //raya media
//     //     number = 0xBF;
//     //     break;

//     // case DISPLAY_REMOTE:
//     //     //cuadro superior
//     //     number = 0x9C;
//     //     break;

//     // case DISPLAY_SQR_DOWN:
//     //     //cuadro inferior
//     //     number = 0xA3;
//     //     break;

//     // case 1:
//     //     //number = 0x06;
//     //     number = 0xF9;
//     //     break;
//     // case 2:
//     //     //number = 0x5B;
//     //     number = 0xA4;
//     //     break;
//     // case 3:
//     //     //number = 0x4F;
//     //     number = 0xB0;
//     //     break;
//     // case 4:
//     //     //number = 0x66;
//     //     number = 0x99;
//     //     break;
//     // case 5:
//     //     //number = 0x6D;
//     //     number = 0x92;
//     //     break;
//     // case 6:
//     //     //number = 0x7D;
//     //     number = 0x82;
//     //     break;
//     // case 7:
//     //     //number = 0x07;
//     //     number = 0xF8;
//     //     break;
//     // case 8:
//     //     //number = 0x7F;
//     //     number = 0x80;
//     //     break;
//     // case 9:
//     //     //number = 0x67;
//     //     number = 0x98;
//     //     break;
//     // case 11:
//     //     //number = 0x80;	//solo punto
//     //     number = 0x7F;	//solo punto
//     //     break;

//     // default:
//     //     //apagar el display
//     //     number = 0xFF;
//     //     break;
//     // }
//     SendSegment (number);
// }

// unsigned char DisplayIsFree (void)
// {
//     if (display_state == DISPLAY_SM_INIT)
//         return 1;
//     else
//         return 0;
// }

// void UpdateDisplayResetSM (void)
// {
//     p_vector_numbers = display_vector_numbers;
//     display_state = DISPLAY_SM_INIT;
// }

// void UpdateDisplayStartSM (void)
// {
//     display_state = DISPLAY_SM_SENDING;
// }

// //show secuences of numbers by calling to ShowNumbers
// //the numbers must be setted by calling to VectorToDisplay()
// //ONLY Numbers and the POINT
// void UpdateDisplaySM (void)
// {

//     switch (display_state)
//     {
//     case DISPLAY_SM_INIT:
//         break;

//     case DISPLAY_SM_SENDING:
//         if (*p_vector_numbers == '.')
//             ShowNumbers(DISPLAY_POINT);
//         else if (*p_vector_numbers == 'h')
//             ShowNumbers(DISPLAY_H);
//         else if (*p_vector_numbers == 's')
//             ShowNumbers(DISPLAY_S);
//         else
//             ShowNumbers(*p_vector_numbers - '0');
        
//         p_vector_numbers++;
//         display_state++;
//         display_timeout = DISPLAY_TIMER_IN_ON;
//         break;

//     case DISPLAY_SM_SHOWING:
//         if (!display_timeout)
//         {
//             ShowNumbers(DISPLAY_NONE);
//             display_timeout = DISPLAY_TIMER_IN_OFF;
//             display_state++;
//         }
//         break;

//     case DISPLAY_SM_WAITING:
//         if (!display_timeout)
//         {
//             if ((*p_vector_numbers != '\0') &&
//                 (p_vector_numbers < &display_vector_numbers[SIZEOF_VDISPLAY]))
//                 display_state = DISPLAY_SM_SENDING;
//             else
//             {
//                 //llegue al final
//                 p_vector_numbers = display_vector_numbers;
//                 // memset (numbers, '\0', sizeof(numbers));
//                 display_state = DISPLAY_SM_INIT;
//             }
//         }
//         break;

//     default:
//         display_state = DISPLAY_SM_INIT;
//         break;
//     }
// }

// //carga los numeros a mostrar en secuencia en un vector
// //acepta numeros en formato char y el punto
// //en general 3 digitos hasta LAST_NUMBER
// void VectorToDisplayStr (char * s_number)
// {
//     unsigned char len;

//     len = strlen(s_number);    //sin el \0
//     if (len < LAST_NUMBER)
//     {
//         UpdateDisplayResetSM();
//         strcpy(display_vector_numbers, s_number);
//         UpdateDisplayStartSM();
//     }
// }

// void ConvertPositionToDisplay (unsigned short pos)
// {
//     char buff [6] = { '\0' };
    
//     if (pos > 999)
//         return;

//     sprintf(buff, "%03d.", pos);
//     VectorToDisplayStr(buff);
    
// }


// //--- end of file ---//
