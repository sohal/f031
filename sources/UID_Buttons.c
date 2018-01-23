/**
  ******************************************************************************
  * @file    UID_Buttons.c
  * @author  KBee
  * @version $Id: UID_Buttons.c  2015-11-02 09:19:00Z sreitz $
  * @date    Oct. 2015
  * @brief   This file contains the software for the UID Button Matrix
	*          all functions needed to use the Buttonmatrix are in here.
	*          For modularity some content is microprocessor specific
	*          and may be rewritten for use with other µCs
  *
  ******************************************************************************
  */
	
	
	
	
/*if the IDE Keil µ-Vision displays an error in include-dependencies this error may be ignored
 *since CMSIS 4.4.0 there is something emberassing with the include dependencies the compiler compiles
 *instead.
 *For more information visit: https://devzone.nordicsemi.com/question/54252/keil-fake-error-__static_inline/
 ************************************************************************************************/
	#include "UID_Buttons.h"
	#include "UID_Protocol.h"
	#include "additions.h"
	

	void UID_sendButtonMessage(uint32_t buttons);

	
//-----------------------------------------------------------------------------
//! \brief The UID_sendButtonMessage streams out the button states and the CRC
//! checksum to the JE via UART	
//! \param[in]     buttons     		holds the current button states
//-----------------------------------------------------------------------------
void UID_sendButtonMessage(uint32_t buttons)
{
    uint8_t buttonMessage[5];
    
    buttonMessage[0] = CmdBTNMSG;
    buttonMessage[1] = ENCODE((uint8_t)(buttons >> 6));
    buttonMessage[2] = ENCODE((uint8_t)(buttons & 0x3F));
    ComputeCRC(buttonMessage, 3, &buttonMessage[3]);

		UID_RS232send(5,&buttonMessage[0]);
}
	
	
	
	
	
//-----------------------------------------------------------------------------
//! \brief (GLOBAL)The UID_checkButtons collects the button states of the UID in account to the systick.
//! if a button is stable for a time >10ms(1 tick) the button message is send an auto repeated all 50ms(5ticks) 
//! \param[in]     TickCounter     		holds the current TickCounter from system
//-----------------------------------------------------------------------------
void UID_checkButtons(uint32_t TickCounter)
{
	  static uint32_t previousButtonState = 0;
	  static uint32_t timePreviousMessageSent = 0; /* watchdog timer for buttonMessage autorepeat */
    static struct buttonState { 
			  uint32_t oldState;
        uint32_t newState;
        uint32_t timestamp;
    }           button[NUMBER_OF_BUTTONS];
		uint32_t 		newButtonState = 0;
    uint32_t    buttonsCurrentState = 0;
    uint32_t    stableButtons = 0;
    uint32_t    i;
    uint32_t    button_i_currentState;
    uint32_t    column;
    
		if(TickCounter!=timePreviousMessageSent)
		{
		
    /* read button matrix and collect one bit per button in variable buttonsCurrentState */
    GPIOB->BSRR                 =  COLUMN1_PULLDOWN | COLUMN2_PULLUP   | COLUMN3_PULLUP;
    while ((GPIOB->IDR & 0x70) != (                   COLUMN2_PULLUP   | COLUMN3_PULLUP)) 
        ;   /* wait until output has stabilized */
    column = GPIOB->IDR;
    buttonsCurrentState  = (column & (ROW_1 | ROW_2)) | ((column & ROW_3) >> 1);
    GPIOB->BSRR                 =  COLUMN1_PULLUP   | COLUMN2_PULLDOWN | COLUMN3_PULLUP;
    while ((GPIOB->IDR & 0x70) != (COLUMN1_PULLUP   |                    COLUMN3_PULLUP))
        ;   /* wait until output has stabilized */
    column = GPIOB->IDR;
    buttonsCurrentState |= ((column & (ROW_1 | ROW_2)) << 3) | ((column & ROW_3) << 2);
    GPIOB->BSRR                 =   COLUMN1_PULLUP | COLUMN2_PULLUP   | COLUMN3_PULLDOWN;
    while ((GPIOB->IDR & 0x70) != ( COLUMN1_PULLUP | COLUMN2_PULLUP                     ))
        ;   /* wait until output has stabilized */
    column = GPIOB->IDR;
    buttonsCurrentState |= ((column & (ROW_1 | ROW_2)) << 6) | ((column & ROW_3) << 5);

    /* read external switches */
    buttonsCurrentState |= (GPIOA->IDR & (EXT_SWITCH_1_PORT | EXT_SWITCH_2_PORT)) << 5;

    /* GPIO input pins are low if button pressed, so invert and mask the result */
    buttonsCurrentState = ~buttonsCurrentState & ( BUTTON_1 | BUTTON_2 | BUTTON_3
                                                 | BUTTON_4 | BUTTON_5 | BUTTON_6
                                                 | BUTTON_7 | BUTTON_8 | BUTTON_9
                                                 | EXT_SWITCH_1 | EXT_SWITCH_2);
    
    /* debounce the buttons by implementing the following state machine
    
               \ current button status ->             (state is "stable" if new==old)
                \
    button-event \      stable      | new != old for < 10ms | new != old for > 10 ms
        |         \ ________________|_______________________|________________________
        V          |                |                       |                       |
                   |                |  new = old;           | new = old;            |
    current == old |       -        |  (status -> stable)   | (status -> stable)    |
                   |                |                       |                       |
                   |________________|_______________________|_______________________|
                   |                |                       |                       |
    current != old | new = current; |                       | old = new;            |
                   | store timestamp|           -           | (status -> stable)    |
                   |                |                       |                       |
                   |________________|_______________________|_______________________|
    */

    for (i = 0; i < NUMBER_OF_BUTTONS; i++) {
        button_i_currentState = (buttonsCurrentState & (1 << i));
        if (button_i_currentState == button[i].oldState) {
            button[i].newState     = button[i].oldState; /* i.e. stable state */
        } else {
            if (button[i].newState == button[i].oldState) { /* button was stable */
                button[i].newState  = button_i_currentState;
                button[i].timestamp = TickCounter;  /* store current time */
            } else if ((TickCounter - button[i].timestamp) > 1) {
                button[i].oldState = button[i].newState;
            }
        }
    }

    /*
     * the button[i].oldState struct members contain their bits already at the
     * correct position so that by OR-ing all of them together we get the return
     * value (we do not return unverified (un-debounced) new states).
     */
    stableButtons = 0;
    for (i = 0; i < NUMBER_OF_BUTTONS; i++) {        
        stableButtons |= button[i].oldState;
    }
		
		newButtonState=stableButtons;
		
		if ((previousButtonState != newButtonState) ||
            ((newButtonState != 0) && ((TickCounter - timePreviousMessageSent) > BUTTON_AUTOREPEAT_FREQ))) 
		{
			previousButtonState  = newButtonState;
      UID_sendButtonMessage(newButtonState);
      timePreviousMessageSent = TickCounter;
    }		
	}
}

//-----------------------------------------------------------------------------
//! \brief (GLOBAL)This function is µController specific. In this case the USART1 module from stm32f031g4 is initialized
//-----------------------------------------------------------------------------	
	void HALInitButtonGPIOs(void)
	{
		RCC->AHBENR  |= RCC_AHBENR_GPIOBEN;
		GPIOB->MODER =  (PB4_OUTPUT | PB5_OUTPUT | PB6_OUTPUT);
	}

