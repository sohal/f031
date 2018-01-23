/**
  ******************************************************************************
  * @file    main.c
  * @author  KBee
  * @version $Id: main.c 87 2015-10-30 15:45:12Z jwaibl $
  * @date    Oct. 2015
  * @brief   This file contains the software for the User Interface Device
  *
  ******************************************************************************
  */
	





/*if the IDE Keil µ-Vision displays an error in include-dependencies this error may be ignored
 *since CMSIS 4.4.0 there is something emberassing with the include dependencies the compiler compiles
 *instead.
 *For more information visit: https://devzone.nordicsemi.com/question/54252/keil-fake-error-__static_inline/
 ************************************************************************************************/
#include "stm32f0xx.h"


#include "UID_RGBDriverPCA9955.h"
#include "UID_Buttons.h"
#include "UID_Protocol.h"

#define SYSTEM_TICK_FREQ_100_Hz   100  					//!< Defining the systick frequency

#define DEBUG_LED_GREEN_ON     GPIO_BSRR_BR_1		//!< µC specific define to hanlde green debug LED
#define DEBUG_LED_GREEN_OFF    GPIO_BSRR_BS_1		//!< µC specific define to hanlde green debug LED
#define DEBUG_LED_AMBER_ON     GPIO_BSRR_BR_0 	//!< µC specific define to hanlde amber debug LED
#define DEBUG_LED_AMBER_OFF    GPIO_BSRR_BS_0		//!< µC specific define to hanlde amber debug LED

void HALToggle_DebugLED_AMBER(uint32_t Tickcounter);

//! Ticks is used as tick counter
volatile uint32_t Ticks = 0;


//----------------------------------------------------------------------------
//! \brief SysTick_Handler handles the internal tick count
//----------------------------------------------------------------------------*/
void SysTick_Handler(void) 
{
    Ticks++;
#if 0
    if (0 == (Ticks % 50))
    {
        GPIOA->ODR = GPIOA->ODR^(0x02U); /*TODO change function to single debug LED on HW v3; quick fix for now*/
    }
#endif

}


//----------------------------------------------------------------------------
//! \brief µC-specific implentation for debug LEDs 
//----------------------------------------------------------------------------*/
void HALInit_DebugLEDsGPIOs(void)
{

    /* clock setup */
    /* enable GPIOA+B, I2C*/
    RCC->AHBENR  |= RCC_AHBENR_GPIOAEN;
	  

    GPIOA->ODR    = 0;  /* turn both debug-LEDs on */
    GPIOA->MODER  |= ( PA0_OUTPUT| PA1_OUTPUT);

    GPIOA->BSRR   = DEBUG_LED_AMBER_OFF;
}

void HALToggle_DebugLED_AMBER(uint32_t Tickcounter)
{
	static uint32_t oldTicks=0;
	if((Tickcounter!=oldTicks)&&(Tickcounter%50==0))
	{
		GPIOA->ODR = GPIOA->ODR^(0x02U); /*TODO change function to single debug LED on HW v3; quick fix for now*/
		oldTicks=Tickcounter;
	}
}


//----------------------------------------------------------------------------
//! \brief the main function handles the programm flow after initialize the UID-functionality
//! is performed.
//----------------------------------------------------------------------------*/
int main(void)

{	
		HALInitRS232GPIOs();
		HALInitButtonGPIOs();
		HALInitRGBDriverGPIOs();	
		HALInit_DebugLEDsGPIOs();
	
		Init_PCA9955A ();
		SysTick_Config (SystemCoreClock/SYSTEM_TICK_FREQ_100_Hz); //count centi seconds
  
#if 1
  /* HMI2017 standalone setting: illuminate OK, Cancel, Set; fade center light fast in cyan*/
    SetSLED(1U, 9U);
    SetSLED(3U, 13U);
    SetSLED(7U, 8U);
    SetSLED(6U, 37U);
#endif

  /* default setting: center blinking yellow */
    SetSLED(6U, 27U);
  
    while(1)
    {	
#if 1			
      HALToggle_DebugLED_AMBER(Ticks);
#endif
			UID_RunCmdParser();
			UID_checkButtons(Ticks);
			UID_RGBDriverSendI2CData();
    }
}


