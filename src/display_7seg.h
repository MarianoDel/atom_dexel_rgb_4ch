//------------------------------------------------------
// #### PROYECTO PANEL ALARMA VAPORE - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DISPLAY_7SEG.H #################################
//------------------------------------------------------
#ifndef _DISPLAY_7SEG_H_
#define _DISPLAY_7SEG_H_

// Additional Includes for Configuration ------------------------------


// Defines for Configuration ----------------------------------------
// #define DISPLAY_TIMER_IN_ON		800
// #define DISPLAY_TIMER_IN_OFF		200
//ESTADOS DEL DISPLAY EN RGB_FOR_CAT
#define SHOW_CHANNELS	0
#define SHOW_NUMBERS	1

//ESTADOS DEL DISPLAY
#define DISPLAY_INIT		0
#define DISPLAY_SENDING		1
#define DISPLAY_SHOWING		2
#define DISPLAY_WAITING		3


#define DISPLAY_P	DISPLAY_PROG

#define DISPLAY_DS3		0x04
#define DISPLAY_DS2		0x02
#define DISPLAY_DS1		0x01

#define DISPLAY_TIMER_RELOAD	6		//166Hz / 3
#define DISPLAY_BLINKING_UPDATE 	12			//son 18 ms de update * num elegido


// Exported Macros and Defines ----------------------------------------
#define DISPLAY_ZERO	10
#define DISPLAY_POINT	11
#define DISPLAY_LINE	12
#define DISPLAY_REMOTE	13
#define DISPLAY_PROG	14
#define DISPLAY_E		15
#define DISPLAY_C		16
#define DISPLAY_H		17
#define DISPLAY_1P		18
#define DISPLAY_S		19
#define DISPLAY_A		20
#define DISPLAY_T		21
#define DISPLAY_R		22
#define DISPLAY_G		23
#define DISPLAY_N		24
#define DISPLAY_NONE	0

// #define SIZEOF_VDISPLAY    DISPLAY_PROG + 1
//cargar nuevos simbolos en ShowNumbers
//y en UpdateDisplaySM si se necesita enviarlos vectorizados

//estados de la SM del DISPLAY
typedef enum {
    DISPLAY_SM_INIT = 0,
    DISPLAY_SM_SENDING,
    DISPLAY_SM_SHOWING,
    DISPLAY_SM_WAITING

} display_sm_t;



// Exported Module Functions ------------------------------------------
// void ConvertPositionToDisplay (unsigned short);
// void UpdateDisplaySM (void);
// void UpdateDisplayResetSM (void);
// void UpdateDisplayStartSM (void);
// void VectorToDisplayStr (char *);
// void ShowNumbers (unsigned char);
unsigned short FromDsToChannel (void);
void FromChannelToDs (unsigned short);
unsigned char TranslateNumber (unsigned char);
void SendSegment (unsigned char, unsigned char);

// void ShowNumbersAgain (void);
// unsigned char DisplayIsFree (void);


#endif    /* _DISPLAY_7SEG_H_ */

//--- end of file ---//

