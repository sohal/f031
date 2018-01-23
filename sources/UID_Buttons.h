/**
  ******************************************************************************
  * @file    UID_Buttons.h
  * @author  KBee
  * @version $Id: UID_Buttons.h  2015-11-02 09:19:00Z sreitz $
  * @date    Oct. 2015
  * @brief   This file contains the interfacedefinition for the UID Button Matrix
	*          for including this softwaremodule to other projects
  *
  ******************************************************************************
  */
	
	
#ifndef UID_BUTTONS_H
#define UID_BUTTONS_H
	/*if the IDE Keil µ-Vision displays an error in include-dependencies this error may be ignored
 *since CMSIS 4.4.0 there is something emberassing with the include dependencies the compiler compiles
 *instead.
 *For more information visit: https://devzone.nordicsemi.com/question/54252/keil-fake-error-__static_inline/
 ************************************************************************************************/
#include "stm32f0xx.h"


#define NUMBER_OF_BUTTONS      11										//!< Defines the number of Buttons used in the UID
#define COLUMN1_PULLDOWN       GPIO_BSRR_BR_4				//!< µC-specific define, pullsdown the first column
#define COLUMN2_PULLDOWN       GPIO_BSRR_BR_5				//!< µC-specific define, pullsdown the second column
#define COLUMN3_PULLDOWN       GPIO_BSRR_BR_6				//!< µC-specific define, pullsdown the third column
#define COLUMN1_PULLUP         GPIO_BSRR_BS_4				//!< µC-specific define, releases the pulldown of first column
#define COLUMN2_PULLUP         GPIO_BSRR_BS_5				//!< µC-specific define, releases the pulldown of second column
#define COLUMN3_PULLUP         GPIO_BSRR_BS_6				//!< µC-specific define, releases the pulldown of third column
#define ROW_1                  GPIO_IDR_0						//!< µC-specific define reads in first row
#define ROW_2                  GPIO_IDR_1						//!< µC-specific define reads in second row
#define ROW_3                  GPIO_IDR_3     			//!< µC-specific define reads in third row
#define EXT_SWITCH_1_PORT      GPIO_IDR_4						//!< Defines the number of Buttons used in the UID
#define EXT_SWITCH_2_PORT      GPIO_IDR_5						//!< Defines the number of Buttons used in the UID

#define PA0_OUTPUT             GPIO_MODER_MODER0_0	//!< µC-specific define for initializing PA0 as output
#define PA1_OUTPUT             GPIO_MODER_MODER1_0	//!< µC-specific define for initializing PA1 as output
#define PB4_OUTPUT             GPIO_MODER_MODER4_0	//!< µC-specific define for initializing PB4 as output
#define PB5_OUTPUT             GPIO_MODER_MODER5_0	//!< µC-specific define for initializing PB5 as output
#define PB6_OUTPUT             GPIO_MODER_MODER6_0	//!< µC-specific define for initializing PB6 as output

#define CmdBTNMSG         0xC2											//!< Defines identifier for button message

#define BUTTON_AUTOREPEAT_FREQ      5  							//!< Define for buttonmessage repeat time formula: repeattime = (BUTTON_AUTOREPEAT_FREQ * Systick)
#define BUTTON_1            0x001										//!< Defines the positin of button1 in the bit-cluster from buttonsCurrentState
#define BUTTON_2            0x002										//!< Defines the positin of button2 in the bit-cluster from buttonsCurrentState
#define BUTTON_3            0x004										//!< Defines the positin of button3 in the bit-cluster from buttonsCurrentState
#define BUTTON_4            0x008										//!< Defines the positin of button4 in the bit-cluster from buttonsCurrentState
#define BUTTON_5            0x010										//!< Defines the positin of button5 in the bit-cluster from buttonsCurrentState
#define BUTTON_6            0x020										//!< Defines the positin of button6 in the bit-cluster from buttonsCurrentState
#define BUTTON_7            0x040										//!< Defines the positin of button7 in the bit-cluster from buttonsCurrentState
#define BUTTON_8            0x080										//!< Defines the positin of button8 in the bit-cluster from buttonsCurrentState
#define BUTTON_9            0x100										//!< Defines the positin of button9 in the bit-cluster from buttonsCurrentState
#define EXT_SWITCH_1        0x200										//!< Defines the positin of external switch1 in the bit-cluster from buttonsCurrentState
#define EXT_SWITCH_2        0x400										//!< Defines the positin of external switch2 in the bit-cluster from buttonsCurrentState

	
	
	
	
	
	void HALInitButtonGPIOs(void);
	void UID_checkButtons(uint32_t TickCounter);

	
#endif
