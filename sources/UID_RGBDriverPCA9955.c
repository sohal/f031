/**
  ******************************************************************************
  * @file    UID_RGBDriverPCA9955.c
  * @author  KBee
  * @version $Id: RGB_Driver_PCA9955.c  2015-11-02 09:19:00Z sreitz $
  * @date    Oct. 2015
  * @brief   This file contains the software for the PCA9955A RGB_Driver
	*          all functions needed to use the PCA9955 are in here.
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
	#include "UID_RGBDriverPCA9955.h"
	#include "UID_Protocol.h"
	
	void I2C1_IRQHandler(void);
	void HALDisableInterrupts(void);
	void HALEnableInterrupts(void);
	void HALWriteFieldToMessageBuffer(uint8_t I2CAddress, uint8_t NumBytes, uint8_t Message[]);
	void HALIncrementRGB_TxBuffRead(void);
	void HALWriteMessageToI2C(void);
	uint8_t ReadDriverReg(enum LED_driverIndex driverIndex, uint8_t regAddress);
	uint8_t HALI2CBufferEmpty(void);	
  
  
  /* shadow registers for gradation control 
     These allow RMW operations on the contents of the bitfield registers without the need
     to actually read the registers via I2C.
  */
  
  static uint8_t u8Drv1GradModeSel0Shadow = 0, u8Drv1GradModeSel1Shadow = 0, u8Drv2GradModeSel0Shadow = 0, u8Drv2GradModeSel1Shadow = 0;
  static uint8_t u8Drv1GradGrpSel0Shadow = 0, u8Drv1GradGrpSel1Shadow = 0, u8Drv1GradGrpSel2Shadow = 0, u8Drv1GradGrpSel3Shadow = 0;
  static uint8_t u8Drv2GradGrpSel0Shadow = 0, u8Drv2GradGrpSel1Shadow = 0, u8Drv2GradGrpSel2Shadow = 0, u8Drv2GradGrpSel3Shadow = 0;
  
	
//-----------------------------------------------------------------------------
//! \brief ISR for I2C interrupt based transmission 
//-----------------------------------------------------------------------------	
	void I2C1_IRQHandler(void)
	{	
		if((I2C1->ISR & I2C_ISR_TXE) == I2C_ISR_TXE)
		{
			I2C1->TXDR = I2Cmessage[RGB_TxBuffRead].I2CdataMessage[RGB_CurrentBufferPosition];
			++RGB_CurrentBufferPosition;
		}
	}

	
	
/* used structs for RGB Drivers */
	
	
//! Structure for RGB color generation
typedef struct _ScolTab {
    uint8_t red;		//!< modulation width for red-LED 0=min, 255=max
    uint8_t green;	//!< modulation width for green-LED 0=min, 255=max
    uint8_t blue;   //!< modulation width for blue-LED 0=min, 255=max     
} ScolTab;

//-----------------------------------------------------------------------------
//! \brief colorTable holds 8 different colors \n
//! [0] : is red \n
//! [1] : is green \n
//! [2] : is blue \n
//! [3] : is yellow \n
//! [4] : is white \n
//! [5] : is black(Led out) e.g TBD \n
//! [6] : is black(Led out) e.g TBD \n
//! [7] : is black(Led out) e.g TBD
//-----------------------------------------------------------------------------
ScolTab colorTable [] = 
{
    255,   0,   0,
      0, 255,   0,
      0,   0, 255,
    255, 255,   0,
    255, 255, 255,
      0, 255, 255,
    255,   0, 255,
      0,   0,   0
};


//! Structure for blink pattern generation
typedef struct _SBlnkPat {
    uint32_t on;         //!< dutycycle time on
    uint32_t off;        //!< dutycycle time off
}SBlickPat; 

//-----------------------------------------------------------------------------
//! \brief blinkPattern holds the blinkpattern for each LED \n
//! [0] : is LED1 \n
//! [1] : is LED2 \n
//! [2] : is LED3 \n
//! [3] : is LED4 \n
//! [4] : is LED5 \n
//! [5] : is LED6 \n
//! [6] : is LED7 \n
//! [7] : is LED8 \n
//! [8] : is LED9
//-----------------------------------------------------------------------------
SBlickPat blinkPattern [] = {
        ~0, 0,           /* 13 years on, 0 centiseconds off                */
        ~0, 0,
        ~0, 0,
        ~0, 0,
        ~0, 0,
        ~0, 0,
        ~0, 0,
        ~0, 0,
        ~0, 0,
};

//! Structure for blink pattern generation
typedef struct _SLED_St {
    uint8_t colorState;			//!< holds the color index for LED
    uint8_t patternState;		//!< holds the blinkpattern for LED
}SLED_St;

//-----------------------------------------------------------------------------
//! \brief blinkPattern holds the blinkpattern for each LED \n
//! [0] : is state for LED1 (TBD) \n
//! [1] : is state for LED2 (TBD) \n
//! [2] : is state for LED3 (TBD) \n
//! [3] : is state for LED4 (TBD) \n
//! [4] : is state for LED5 (TBD) \n
//! [5] : is state for LED6 (TBD) \n
//! [6] : is state for LED7 (TBD) \n
//! [7] : is state for LED8 (TBD) \n
//! [8] : is state for LED9 (TBD)
//-----------------------------------------------------------------------------
SLED_St LED_State[] = {
    0,0,                 /* colorIndex 0, patternIndex 0 */
    0,0,
    1,1,
    0,0,
    0,0,
    0,0,
    0,0,
    0,0,
    4,1,
};	
	

//-----------------------------------------------------------------------------
//! \brief (GLOBAL) UID_RGBDriverSendI2CData handles I2C RGB Driver half duplex interaction
//-----------------------------------------------------------------------------
void UID_RGBDriverSendI2CData(void)
{
	HALWriteMessageToI2C();
}



//-----------------------------------------------------------------------------
//! \brief The ReadDriverReg reads 1data-byte out of the desired register from desired driver
//! \param[in]     driverIndex     		equal to I2C address from desired driver
//! \param[in]     regAddress     		equal to desired register in driver
//! \return				 the wanted databyte from the choosendrivers registers
//-----------------------------------------------------------------------------
uint8_t ReadDriverReg(enum LED_driverIndex driverIndex, uint8_t regAddress)
{
		//waits until I2C datatransfer completed so that the interrupts can be disabled safty without dataloss
		while((I2CtransferInProgress==1)&&(!HAL_STOP_BIT_SET));
		
		HALDisableInterrupts();			
    /* first put the Address of the register to be read into the control register */
    I2C1->CR2 = I2C_CR2_AUTOEND | (1 << NBYTES_POS) | driverIndex;
    while ((I2C1->ISR & I2C_ISR_TXE) != I2C_ISR_TXE)
        ;  /* Check Tx empty */
    I2C1->TXDR = regAddress & 0xFF; /* Byte to send */
    I2C1->CR2  |= I2C_CR2_START; /* Go */
    while ((I2C1->ISR & I2C_ISR_TXE) != I2C_ISR_TXE);  /* wait until write transfer finished */

    I2C1->CR2 = I2C_CR2_AUTOEND | (1 << NBYTES_POS) | driverIndex     | I2C_CR2_RD_WRN;
    I2C1->CR2 |= I2C_CR2_START; /* Go */
    while ((I2C1->ISR & I2C_ISR_RXNE) != I2C_ISR_RXNE);

		HALEnableInterrupts();	
		
    return ((uint8_t)I2C1->RXDR);
}

//-----------------------------------------------------------------------------
//! \brief Set gradation mode and group for an LED. ONLY USEABLE WITH V2 OR V3 HARDWARE!
//! \param[in]     LED     						desired LED
//! \param[in]     newGradation  			new gradation value to be set
//-----------------------------------------------------------------------------
#if ((2U == UID_HW_VERSION) || (3U == UID_HW_VERSION))
void SetLEDGradation(uint8_t LED, uint8_t newGradation)
{
  uint8_t u8Driver = PCA9955A_1;
  uint8_t u8DriverData[7];
  uint8_t u8GradGroup = 0;
  
  if (LED_GRADGRP_NONE != newGradation)
  {
    /* a gradation becomes activated: determine its group index */
    u8GradGroup = newGradation - 1; /* ## this is unsafe as it relies on the define values ## */
  }
  
  switch(LED)
  {
    case LED1:
      u8Driver = PCA9955A_1;
      if (LED_GRADGRP_NONE == newGradation)
      {
        u8Drv1GradModeSel0Shadow &= ~(0x07U);
      } else {
        u8Drv1GradModeSel0Shadow |= (0x07U);
        
        u8Drv1GradGrpSel0Shadow &= ~(0x3F);
        u8Drv1GradGrpSel0Shadow |= ((u8GradGroup << 4U) | (u8GradGroup << 2U) | (u8GradGroup));
      }
      break;
    
    case LED2:
      u8Driver = PCA9955A_1;
      if (LED_GRADGRP_NONE == newGradation)
      {
        u8Drv1GradModeSel0Shadow &= ~(0x38U);
      } else {
        u8Drv1GradModeSel0Shadow |= (0x38U);
        
        u8Drv1GradGrpSel0Shadow &= ~(0xC0);
        u8Drv1GradGrpSel0Shadow |= ((u8GradGroup << 6U));
        u8Drv1GradGrpSel1Shadow &= ~(0x0F);
        u8Drv1GradGrpSel1Shadow |= ((u8GradGroup << 2U) | (u8GradGroup));
      }
      break;
    
    case LED3:
      u8Driver = PCA9955A_1;
      if (LED_GRADGRP_NONE == newGradation)
      {
        u8Drv1GradModeSel0Shadow &= ~(0xC0U);
        u8Drv1GradModeSel1Shadow &= ~(0x01U);
      } else {
        u8Drv1GradModeSel0Shadow |= (0xC0U);
        u8Drv1GradModeSel1Shadow |= (0x01U);
        
        u8Drv1GradGrpSel1Shadow &= ~(0xF0);
        u8Drv1GradGrpSel1Shadow |= ((u8GradGroup << 6U) | (u8GradGroup << 4U));
        u8Drv1GradGrpSel2Shadow &= ~(0x03);
        u8Drv1GradGrpSel2Shadow |= ((u8GradGroup));
      }
      break;
    
    case LED4:
      u8Driver = PCA9955A_1;
      if (LED_GRADGRP_NONE == newGradation)
      {
        u8Drv1GradModeSel1Shadow &= ~(0x0EU);
      } else {
        u8Drv1GradModeSel1Shadow |= (0x0EU);
        
        u8Drv1GradGrpSel2Shadow &= ~(0xFC);
        u8Drv1GradGrpSel2Shadow |= ((u8GradGroup << 6U) | (u8GradGroup << 4U) | (u8GradGroup << 2U));
      }
      break;
    
    case LED5:
      u8Driver = PCA9955A_1;
      if (LED_GRADGRP_NONE == newGradation)
      {
        u8Drv1GradModeSel1Shadow &= ~(0x70U);
      } else {
        u8Drv1GradModeSel1Shadow |= (0x70U);
        
        u8Drv1GradGrpSel3Shadow &= ~(0x3FU);
        u8Drv1GradGrpSel3Shadow |= ((u8GradGroup << 4U) | (u8GradGroup << 2U) | (u8GradGroup));
      }
      break;
    
    case LED6:
      u8Driver = PCA9955A_2;
      if (LED_GRADGRP_NONE == newGradation)
      {
        u8Drv2GradModeSel0Shadow &= ~(0x07U);
      } else {
        u8Drv2GradModeSel0Shadow |= (0x07U);
        
        u8Drv2GradGrpSel0Shadow &= ~(0x3F);
        u8Drv2GradGrpSel0Shadow |= ((u8GradGroup << 4U) | (u8GradGroup << 2U) | (u8GradGroup));
      }
      break;
    
#if (2U == UID_HW_VERSION)
    case LED7:
      u8Driver = PCA9955A_2;
      if (LED_GRADGRP_NONE == newGradation)
      {
        u8Drv2GradModeSel0Shadow &= ~(0x38U);
      } else {
        u8Drv2GradModeSel0Shadow |= (0x38U);
        
        u8Drv2GradGrpSel0Shadow &= ~(0xC0);
        u8Drv2GradGrpSel0Shadow |= ((u8GradGroup << 6U));
        u8Drv2GradGrpSel1Shadow &= ~(0x0F);
        u8Drv2GradGrpSel1Shadow |= ((u8GradGroup << 2U) | (u8GradGroup));
      }
      break;
      
#elif (3U == UID_HW_VERSION) /* LED was moved from outpts 3-5 to outputs 12-14, thus new gradation registers (see LED5, but different driver) */
    case LED7:
      u8Driver = PCA9955A_2;
      if (LED_GRADGRP_NONE == newGradation)
      {
        u8Drv2GradModeSel1Shadow &= ~(0x70U);
      } else {
        u8Drv2GradModeSel1Shadow |= (0x70U);
        
        u8Drv2GradGrpSel3Shadow &= ~(0x3FU);
        u8Drv2GradGrpSel3Shadow |= ((u8GradGroup << 4U) | (u8GradGroup << 2U) | (u8GradGroup));
      }
      break;
#endif
      
    case LED8:
      u8Driver = PCA9955A_2;
      if (LED_GRADGRP_NONE == newGradation)
      {
        u8Drv2GradModeSel0Shadow &= ~(0xC0U);
        u8Drv2GradModeSel1Shadow &= ~(0x01U);
      } else {
        u8Drv2GradModeSel0Shadow |= (0xC0U);
        u8Drv2GradModeSel1Shadow |= (0x01U);
        
        u8Drv2GradGrpSel1Shadow &= ~(0xF0);
        u8Drv2GradGrpSel1Shadow |= ((u8GradGroup << 6U) | (u8GradGroup << 4U));
        u8Drv2GradGrpSel2Shadow &= ~(0x03);
        u8Drv2GradGrpSel2Shadow |= ((u8GradGroup));
      }
      break;
    
    case LED9:
      u8Driver = PCA9955A_2;
      if (LED_GRADGRP_NONE == newGradation)
      {
        u8Drv2GradModeSel1Shadow &= ~(0x0EU);
      } else {
        u8Drv2GradModeSel1Shadow |= (0x0EU);
        
        u8Drv2GradGrpSel2Shadow &= ~(0xFC);
        u8Drv2GradGrpSel2Shadow |= ((u8GradGroup << 6U) | (u8GradGroup << 4U) | (u8GradGroup << 2U));
      }
      break;
    
    default:
      break;
  } /* switch(LED) */
  
  u8DriverData[0] = GRAD_MODE_SEL0 | AUTO_INCREMENT; /* always start with this register and go on */
  
  if (PCA9955A_1 == u8Driver)
  {
    u8DriverData[1] = u8Drv1GradModeSel0Shadow;
    u8DriverData[2] = u8Drv1GradModeSel1Shadow;

    u8DriverData[3] = u8Drv1GradGrpSel0Shadow;
    u8DriverData[4] = u8Drv1GradGrpSel1Shadow;
    u8DriverData[5] = u8Drv1GradGrpSel2Shadow;
    u8DriverData[6] = u8Drv1GradGrpSel3Shadow;
  } else {
    u8DriverData[1] = u8Drv2GradModeSel0Shadow;
    u8DriverData[2] = u8Drv2GradModeSel1Shadow;

    u8DriverData[3] = u8Drv2GradGrpSel0Shadow;
    u8DriverData[4] = u8Drv2GradGrpSel1Shadow;
    u8DriverData[5] = u8Drv2GradGrpSel2Shadow;
    u8DriverData[6] = u8Drv2GradGrpSel3Shadow;
  }
  
  if (LED_GRADGRP_NONE == newGradation)
  {
    /* gradation switched off: only write to mode registers */ 
    HALWriteFieldToMessageBuffer(u8Driver, 3, u8DriverData);
  } else {
    /* gradation switched on: select group and activate mode */
    HALWriteFieldToMessageBuffer(u8Driver, 7, u8DriverData);
  }
}
#endif


//-----------------------------------------------------------------------------
//! \brief Set IREF of the three components of an LED. ONLY USEABLE WITH V2 OR V3 HARDWARE!
//! \param[in]     LED     						desired LED
//! \param[in]     newIREF       			new IREF value to be set
//-----------------------------------------------------------------------------
#if ((2U == UID_HW_VERSION) || (3U == UID_HW_VERSION))
void SetLEDIREF(uint8_t LED, uint8_t newIREF)
{
  uint8_t u8Driver;
  uint8_t u8DriverData[4]; /* takes register address and three IREF values for I2C transmission*/
  
  switch(LED)
  {
    case LED1:
      u8Driver = LED1_RED_DRV; /* red component always is first in driver output sequence for each LED */
      u8DriverData[0] = LED1_RED_PWM; /* will be shifted to IREF registers at the end of the function */
      break;
    
    case LED2:
      u8Driver = LED2_RED_DRV;
      u8DriverData[0] = LED2_RED_PWM;
      break;
    
    case LED3:
      u8Driver = LED3_RED_DRV;
      u8DriverData[0] = LED3_RED_PWM;
      break;
    
    case LED4:
      u8Driver = LED4_RED_DRV;
      u8DriverData[0] = LED4_RED_PWM;
      break;
    
    case LED5:
      u8Driver = LED5_RED_DRV;
      u8DriverData[0] = LED5_RED_PWM;
      break;
    
    case LED6:
      u8Driver = LED6_RED_DRV;
      u8DriverData[0] = LED6_RED_PWM;
      break;
    
    case LED7:
      u8Driver = LED7_RED_DRV;
      u8DriverData[0] = LED7_RED_PWM;
      break;
    
    case LED8:
      u8Driver = LED8_RED_DRV;
      u8DriverData[0] = LED8_RED_PWM;
      break;
    
    case LED9:
      u8Driver = LED9_RED_DRV;
      u8DriverData[0] = LED9_RED_PWM;
      break;
    
    default:
      break;
  } /* switch(LED) */
  
  u8DriverData[0] |= AUTO_INCREMENT;
  u8DriverData[0] += 0x10U; /* shift from PWM to IREF register */
  u8DriverData[1] = newIREF;
  u8DriverData[2] = newIREF;
  u8DriverData[3] = newIREF;
  
  HALWriteFieldToMessageBuffer(u8Driver, 4, u8DriverData);
}
#endif
//-----------------------------------------------------------------------------
//! \brief The SetRGBLED switches the desired LED into another state via I2C interaction
//! \param[in]     LED     						desired LED
//! \param[in]     redValue     			red intendensy
//! \param[in]     greenValue     		green intendensy
//! \param[in]     blueValue     			blue intendensy
//-----------------------------------------------------------------------------
void SetRGBLED(uint32_t LED, uint32_t redValue, uint32_t greenValue, uint32_t blueValue)
{
	uint8_t redLED[2];
	uint8_t greenLED[2];
	uint8_t blueLED[2];
	uint8_t redDriver, greenDriver, blueDriver;
    switch (LED) {
			
        case LED1:
						redDriver=LED1_RED_DRV; redLED[0]=LED1_RED_PWM; redLED[1]=redValue;
						greenDriver=LED1_GREEN_DRV; greenLED[0]=LED1_GREEN_PWM; greenLED[1]=greenValue;
						blueDriver=LED1_BLUE_DRV; blueLED[0]=LED1_BLUE_PWM; blueLED[1]=blueValue;                      
            break;
        case LED2:
						redDriver=LED2_RED_DRV; redLED[0]=LED2_RED_PWM; redLED[1]=redValue;
						greenDriver=LED2_GREEN_DRV; greenLED[0]=LED2_GREEN_PWM; greenLED[1]=greenValue;
						blueDriver=LED2_BLUE_DRV; blueLED[0]=LED2_BLUE_PWM; blueLED[1]=blueValue;                       
            break;
        case LED3:
						redDriver=LED3_RED_DRV; redLED[0]=LED3_RED_PWM; redLED[1]=redValue;
						greenDriver=LED3_GREEN_DRV; greenLED[0]=LED3_GREEN_PWM; greenLED[1]=greenValue;
						blueDriver=LED3_BLUE_DRV; blueLED[0]=LED3_BLUE_PWM; blueLED[1]=blueValue;  					                       
            break;
        case LED4:
						redDriver=LED4_RED_DRV; redLED[0]=LED4_RED_PWM; redLED[1]=redValue;
						greenDriver=LED4_GREEN_DRV; greenLED[0]=LED4_GREEN_PWM; greenLED[1]=greenValue;
						blueDriver=LED4_BLUE_DRV; blueLED[0]=LED4_BLUE_PWM; blueLED[1]=blueValue;  					                      
            break;
        case LED5:
						redDriver=LED5_RED_DRV; redLED[0]=LED5_RED_PWM; redLED[1]=redValue;
						greenDriver=LED5_GREEN_DRV; greenLED[0]=LED5_GREEN_PWM; greenLED[1]=greenValue;
						blueDriver=LED5_BLUE_DRV; blueLED[0]=LED5_BLUE_PWM; blueLED[1]=blueValue;  					                      
            break;
        case LED6:
						redDriver=LED6_RED_DRV; redLED[0]=LED6_RED_PWM; redLED[1]=redValue;
						greenDriver=LED6_GREEN_DRV; greenLED[0]=LED6_GREEN_PWM; greenLED[1]=greenValue;
						blueDriver=LED6_BLUE_DRV; blueLED[0]=LED6_BLUE_PWM; blueLED[1]=blueValue;  					                       
            break;
        case LED7:
						redDriver=LED7_RED_DRV; redLED[0]=LED7_RED_PWM; redLED[1]=redValue;
						greenDriver=LED7_GREEN_DRV; greenLED[0]=LED7_GREEN_PWM; greenLED[1]=greenValue;
						blueDriver=LED7_BLUE_DRV; blueLED[0]=LED7_BLUE_PWM; blueLED[1]=blueValue;					                       
            break;
        case LED8:
						redDriver=LED8_RED_DRV; redLED[0]=LED8_RED_PWM; redLED[1]=redValue;
						greenDriver=LED8_GREEN_DRV; greenLED[0]=LED8_GREEN_PWM; greenLED[1]=greenValue;
						blueDriver=LED8_BLUE_DRV; blueLED[0]=LED8_BLUE_PWM; blueLED[1]=blueValue;					                       
            break;
        case LED9:
						redDriver=LED9_RED_DRV; redLED[0]=LED9_RED_PWM; redLED[1]=redValue;
						greenDriver=LED9_GREEN_DRV; greenLED[0]=LED9_GREEN_PWM; greenLED[1]=greenValue;
						blueDriver=LED9_BLUE_DRV; blueLED[0]=LED9_BLUE_PWM; blueLED[1]=blueValue;					                       
            break;
        default:
            break;  /* ignore */			
    }
		HALWriteFieldToMessageBuffer(redDriver, 2, &redLED[0]);
		HALWriteFieldToMessageBuffer(greenDriver, 2, &greenLED[0]);
		HALWriteFieldToMessageBuffer(blueDriver, 2, &blueLED[0]);
}


//-----------------------------------------------------------------------------
//! \brief (GLOBAL) The SetSLED sets a single LED into the desired state in condition to the colorTable and stores the state into the LED_State field
//! \param[in]     LED     						desired LED
//! \param[in]     index     					index from colorTable
//-----------------------------------------------------------------------------
void SetSLED (uint32_t LED, uint32_t newState)
{
    LED_State[LED].colorState = (newState & 0x07);        /* remember current LED color */ 
    LED_State[LED].patternState = (newState >> 3);        /* remember current LED pattern */ 
    SetRGBLED(LED, colorTable[LED_State[LED].colorState].red, colorTable[LED_State[LED].colorState].green, colorTable[LED_State[LED].colorState].blue);
    switch (LED_State[LED].patternState)
    {
      case 0:
        SetLEDIREF(LED, LED_IREF_OFF);
        SetLEDGradation(LED, LED_GRADGRP_NONE);
      break;
      
      case 1:
        SetLEDIREF(LED, LED_IREF_FULL);
        SetLEDGradation(LED, LED_GRADGRP_NONE);
      break;
      
      case 2:
        SetLEDIREF(LED, LED_IREF_LOW);
        SetLEDGradation(LED, LED_GRADGRP_NONE);
      break;
      
      case 3:
        /*  SetLEDIREF() should not be necessary according to datasheet, but apparently is:
            LED stays dark otherwise, although the channel-specific IREF should 
            be irrelevant when the respective channel operates in gradation mode.
            Any non-zero value seems to suffice.
        */
        SetLEDIREF(LED, LED_IREF_FULL);
        SetLEDGradation(LED, LED_GRADGRP_0);
      break;
      
      case 4:
        SetLEDIREF(LED, LED_IREF_FULL);
        SetLEDGradation(LED, LED_GRADGRP_1);
      break;

      case 5:
        SetLEDIREF(LED, LED_IREF_FULL);
        SetLEDGradation(LED, LED_GRADGRP_2);
      break;

      case 6:
        SetLEDIREF(LED, LED_IREF_FULL);
        SetLEDGradation(LED, LED_GRADGRP_3);
      break;
      
      default:
        SetLEDIREF(LED, LED_IREF_OFF);
        SetLEDGradation(LED, LED_GRADGRP_NONE);
        
    }
}

//-----------------------------------------------------------------------------
//! \brief LEDs_allOff switches all LEDs off
//-----------------------------------------------------------------------------
void LEDs_allOff (void)
{
	uint8_t allLEDsOut[2];
		allLEDsOut[0]=PWMALL;
		allLEDsOut[1]=0;
		HALWriteFieldToMessageBuffer(PCA9955A_1,2,&allLEDsOut[0]);
		HALWriteFieldToMessageBuffer(PCA9955A_2,2,&allLEDsOut[0]); 
}

//-----------------------------------------------------------------------------
//! \brief (GLOBAL)Init_PCA9955A initializes both PCA9955A drivers
//-----------------------------------------------------------------------------
void Init_PCA9955A (void)
{
		uint8_t initLEDOUT_Regs[] = {(LEDOUT0|AUTO_INCREMENT), 
																	ALL_LEDS_INDIV_AND_GROUP, 
																	ALL_LEDS_INDIV_AND_GROUP, 
																	ALL_LEDS_INDIV_AND_GROUP, 
																	ALL_LEDS_INDIV_AND_GROUP};
		uint8_t initIREFORALL_Reg[] = {IREFALL,0xFF};

		uint8_t registerData[] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U};
    
	
    GPIOB->BSRR = RELEASE_PCA9955A_RESET;

		HALWriteFieldToMessageBuffer(PCA9955A_1,5,&initLEDOUT_Regs[0]);
		HALWriteFieldToMessageBuffer(PCA9955A_1,2,&initIREFORALL_Reg[0]);
		HALWriteFieldToMessageBuffer(PCA9955A_2,5,&initLEDOUT_Regs[0]);
		HALWriteFieldToMessageBuffer(PCA9955A_2,2,&initIREFORALL_Reg[0]);		

		registerData[0] = MODE2 | AUTO_INCREMENT;
		registerData[1] = 0x04; /* activate exponential adjustment for gradation control */
    HALWriteFieldToMessageBuffer(PCA9955A_1,2,&registerData[0]);
    HALWriteFieldToMessageBuffer(PCA9955A_2,2,&registerData[0]);

#if 0
		registerData[0] = GRAD_MODE_SEL1 | AUTO_INCREMENT;
		registerData[1] = 0xFF;
    HALWriteFieldToMessageBuffer(PCA9955A_1,2,&registerData[0]);
    HALWriteFieldToMessageBuffer(PCA9955A_2,2,&registerData[0]);

		registerData[0] = GRAD_GRP_SEL0 | AUTO_INCREMENT;
		registerData[1] = 0xAA;
		registerData[2] = 0xAA;
		registerData[3] = 0xAA;
		registerData[4] = 0xAA;
    HALWriteFieldToMessageBuffer(PCA9955A_1,5,&registerData[0]);
    HALWriteFieldToMessageBuffer(PCA9955A_2,5,&registerData[0]);
#endif

/* blink  */
    registerData[0] = RAMP_RATE_GRP0 | AUTO_INCREMENT;
		registerData[1] = 0x00; /*ramp rate*/
		registerData[2] = 0x00; /*step time*/
		registerData[3] = 0xDB; /*hold cntl*/
		registerData[4] = LED_IREF_FULL; /*IREF*/
    HALWriteFieldToMessageBuffer(PCA9955A_1,5,&registerData[0]);
    HALWriteFieldToMessageBuffer(PCA9955A_2,5,&registerData[0]);

/* fast pulsing */
    registerData[0] = RAMP_RATE_GRP1 | AUTO_INCREMENT;
		registerData[1] = 0xC1; /*ramp rate*/
		registerData[2] = 0x0A; /*step time*/
		registerData[3] = 0xC0; /*hold cntl*/
		registerData[4] = LED_IREF_FULL; /*IREF*/
    HALWriteFieldToMessageBuffer(PCA9955A_1,5,&registerData[0]);
    HALWriteFieldToMessageBuffer(PCA9955A_2,5,&registerData[0]);

/* slow pulsing */
    registerData[0] = RAMP_RATE_GRP2 | AUTO_INCREMENT;
		registerData[1] = 0xC1; /*ramp rate*/
		registerData[2] = 0x1E; /*step time*/
		registerData[3] = 0xC0; /*hold cntl*/
		registerData[4] = LED_IREF_FULL; /*IREF*/
    HALWriteFieldToMessageBuffer(PCA9955A_1,5,&registerData[0]);
    HALWriteFieldToMessageBuffer(PCA9955A_2,5,&registerData[0]);

/* decay blinking */
    registerData[0] = RAMP_RATE_GRP3 | AUTO_INCREMENT;
		registerData[1] = 0x41; /*ramp rate*/
		registerData[2] = 0x0A; /*step time*/
		registerData[3] = 0xC1; /*hold cntl*/
		registerData[4] = LED_IREF_FULL; /*IREF*/
    HALWriteFieldToMessageBuffer(PCA9955A_1,5,&registerData[0]);
    HALWriteFieldToMessageBuffer(PCA9955A_2,5,&registerData[0]);

		registerData[0] = GRAD_CNTL | AUTO_INCREMENT;
		registerData[1] = 0xFF;
    HALWriteFieldToMessageBuffer(PCA9955A_1,2,&registerData[0]);
    HALWriteFieldToMessageBuffer(PCA9955A_2,2,&registerData[0]);

}


//-----------------------------------------------------------------------------
//! \brief (GLOBAL)This function is µController specific. In this case the I2C module from stm32f031g4 is initialized
//-----------------------------------------------------------------------------
void HALInitRGBDriverGPIOs(void)
{
	RCC->AHBENR |= (RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN);
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	
	NVIC->ISER[0] |= (1 << (I2C1_IRQn & 0x1F));  															// enable the NVIC for I2C-Module
	HALEnableInterrupts();
	GPIOA->AFR[1] = PIN_A9_ALTFUNCT_I2C1_SCL | PIN_A10_ALTFUNCT_IC21_SDA;
	
	GPIOA->MODER |= (PA9_MODER_I2C1_SCL  | PA10_MODER_I2C1_SDA);
	
	GPIOB->ODR = 0x0L; /* reset PCA9955A LED drivers */
	
	GPIOB->MODER |= PB7_OUTPUT;
	
	I2C1->TIMINGR = (uint32_t)0x50330309;
	I2C1->CR1 = I2C1->CR1|I2C_CR1_PE;	
  
    			
}

//-----------------------------------------------------------------------------
//! \brief HALIncrementRGB_TxBuffRead increments RGB_TxBuffRead
//-----------------------------------------------------------------------------		
	void HALIncrementRGB_TxBuffRead(void)
	{
		++RGB_TxBuffRead;
		if(RGB_TxBuffRead==RGB_I2CMESSAGE_BUFFER_SIZE)
		{
			RGB_TxBuffRead=0;
		}
	}

//-----------------------------------------------------------------------------
//! \brief HALEnableInterrupts is µC specific and enables the TXE and TC interrupts
//-----------------------------------------------------------------------------
void HALEnableInterrupts(void)
{
	I2C1->CR1 = I2C1->CR1 | I2C_CR1_TXIE;
}

//-----------------------------------------------------------------------------
//! \brief HALDisableInterrupts is disabling alle used I2C interrupts
//-----------------------------------------------------------------------------
void HALDisableInterrupts(void)
{
	I2C1->CR1 = I2C1->CR1&0xFFFFFFFD;
}

//-----------------------------------------------------------------------------
//! \brief (GLOBAL) HALWriteMessageToI2C sends the data from RGB_TxBuffer via I2C to slaves
//-----------------------------------------------------------------------------		
	void HALWriteMessageToI2C(void)
	{
		if(HAL_STOP_BIT_SET)	
		{
			I2CtransferInProgress=0;																						// in case the stop bit occures there is no I2C interaction active
			if(RGB_CurrentBufferPosition==I2Cmessage[RGB_TxBuffRead].numBytes)	// check up if datatransfer complete
			{
				HALIncrementRGB_TxBuffRead();																			//if complete send next message, else resend
			}
			I2C1->ICR |= I2C_ICR_STOPCF;																				//clear stop-bit
		}
		if((!HALI2CBufferEmpty())&&(I2CtransferInProgress==0))								//check up transmitstatus and buffer, if I2C ready and buffer not empty send message
		{
			I2CtransferInProgress=1;
			I2C1->CR2 =  I2C_CR2_AUTOEND|(I2Cmessage[RGB_TxBuffRead].numBytes << NBYTES_POS) | I2Cmessage[RGB_TxBuffRead].slaveAddress;
			I2C1->TXDR = I2Cmessage[RGB_TxBuffRead].I2CdataMessage[0]; // I2C slave internal register address
			RGB_CurrentBufferPosition = 1;
			I2C1->CR2  |= I2C_CR2_START; // Go
		}
	}


	
//-----------------------------------------------------------------------------
//! \brief HALWriteFieldToMessageBuffer writes a desired message into the RGB_TxBuffer \n
//! I2Cmessage[RGB_TxBuffWrite].slaveAddress holds the I2C-address from slave \n
//!	I2Cmessage[RGB_TxBuffWrite].numBytes holds the number of bytes to transfer via I2C \n
//!	I2Cmessage[RGB_TxBuffWrite].I2CdataMessage[msgByte] holds the data to send via I2C (slave register included at MSB!) \n	
//! \param[in]     I2CAddress     		the I2C hardware address
//! \param[in]     NumBytes     			the number bytes to send
//! \param[in]     Message[]     			the field to write in the regaddress is included
//-----------------------------------------------------------------------------		
	void HALWriteFieldToMessageBuffer(uint8_t I2CAddress, uint8_t NumBytes, uint8_t Message[])
	{
		uint8_t msgByte=0;
		if(NumBytes>RGB_TxBUFFER_SIZE)
		{
			while(1){/* invalid!this is a debug loop max datasize is RGB_TxBUFFER_SIZE*/};
		}
		I2Cmessage[RGB_TxBuffWrite].slaveAddress = I2CAddress;
		I2Cmessage[RGB_TxBuffWrite].numBytes = NumBytes;	
		for(msgByte=0; msgByte<NumBytes;++msgByte)
		{
			I2Cmessage[RGB_TxBuffWrite].I2CdataMessage[msgByte] = Message[msgByte];
		}
		++RGB_TxBuffWrite;
		if(RGB_TxBuffWrite==RGB_I2CMESSAGE_BUFFER_SIZE)
		{
			RGB_TxBuffWrite=0;
		}	
	}
	
//-----------------------------------------------------------------------------
//! \brief HALI2CBufferEmpty checks is there are messages to transfer via I2C
//-----------------------------------------------------------------------------		
	uint8_t HALI2CBufferEmpty(void)
	{
		if(RGB_TxBuffRead==RGB_TxBuffWrite)
		{
			return(1);
		}
		else
		{
			return(0);
		}
	}



