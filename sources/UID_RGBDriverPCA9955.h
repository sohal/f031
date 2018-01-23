/**
  ******************************************************************************
  * @file    UID_RGBDriverPCA9955.h
  * @author  KBee
  * @version $Id: RGB_Driver_PCA9955.h  2015-11-02 09:19:00Z sreitz $
  * @date    Oct. 2015
  * @brief   This file contains the interfacedefinition for the PCA9955A RGD Driver
	*          for including this softwaremodule to other projects
  *
  ******************************************************************************
  */
	#ifndef RGB_DRIVER_PCA9955A
	#define RGB_DRIVER_PCA9955A
	
	
	
	
	
	/*if the IDE Keil µ-Vision displays an error in include-dependencies this error may be ignored
 *since CMSIS 4.4.0 there is something emberassing with the include dependencies the compiler compiles
 *instead.
 *For more information visit: https://devzone.nordicsemi.com/question/54252/keil-fake-error-__static_inline/
 ************************************************************************************************/
	#include "stm32f0xx.h"
	#include "UIDconfig.h"
	
	
/***********************************************************************************/
/* STM32*/
#define PIN_A9_ALTFUNCT_I2C1_SCL  ((uint32_t)0x00000040)	//!< µC-specific define, gives PA9 als alternate I2C pin
#define PIN_A10_ALTFUNCT_IC21_SDA ((uint32_t)0x00000400)	//!< µC-specific define, gives PA10 als alternate I2C pin
#define PA9_MODER_I2C1_SCL     GPIO_MODER_MODER9_1				//!< µC-specific define, for initialize
#define PA10_MODER_I2C1_SDA    GPIO_MODER_MODER10_1				//!< µC-specific define, for initialize
#define RELEASE_PCA9955A_RESET GPIO_BSRR_BS_7							//!< µC-specific define, for initialize
#define NBYTES_POS 16																			//!< µC-specific used for I2C communication

/// LED_driverIndex enum hold the LED drivers I2C-addresses
#if (1U == UID_HW_VERSION)
/* hardware v1 */
enum LED_driverIndex{
	PCA9955A_1 = (0x09 << 1), ///< gives the I2C address of driver 1
	PCA9955A_2 = (0x0A << 1)  ///< gives the I2C address of driver 2
};

#elif ((2U == UID_HW_VERSION) || (3U == UID_HW_VERSION))
/* hardware v2 or v3 */
enum LED_driverIndex{
	PCA9955A_1 = (0x15 << 1), ///< gives the I2C address of driver 1
	PCA9955A_2 = (0x19 << 1)  ///< gives the I2C address of driver 2
};
#endif

/* Register addresses inside each of the PCA9955A ICs: */
#define MODE1          					 	0x00		//!< Defines for PCA9955A register set
#define ALLCALL                  	0x01		//!< Defines for PCA9955A register set
#define SUB3                     	0x02		//!< Defines for PCA9955A register set
#define SUB2                     	0x04		//!< Defines for PCA9955A register set
#define SUB1                     	0x08		//!< Defines for PCA9955A register set
#define SLEEP                    	0x10		//!< Defines for PCA9955A register set
#define AI0                      	0x20		//!< Defines for PCA9955A register set
#define AI1                      	0x40		//!< Defines for PCA9955A register set
#define AIF                      	0x80		//!< Defines for PCA9955A register set
#define MODE2          					 	0x01		//!< Defines for PCA9955A register set
#define EXP_EN                   	0x04		//!< Defines for PCA9955A register set
#define OCH                      	0x08		//!< Defines for PCA9955A register set
#define CLRERR                   	0x10		//!< Defines for PCA9955A register set
#define DMBLNK                   	0x20		//!< Defines for PCA9955A register set
#define ERROR                    	0x40		//!< Defines for PCA9955A register set
#define OVERTEMP                 	0x80		//!< Defines for PCA9955A register set
#define LEDOUT0        					 	0x02		//!< Defines for PCA9955A register set
#define ALL_LEDS_OFF             	0x00		//!< Defines for PCA9955A register set
#define ALL_LEDS_ON              	0x55		//!< Defines for PCA9955A register set
#define ALL_LEDS_INDIV           	0xAA		//!< Defines for PCA9955A register set
#define ALL_LEDS_INDIV_AND_GROUP 	0xFF		//!< Defines for PCA9955A register set
#define LEDOUT1        					 	0x03		//!< Defines for PCA9955A register set
#define LEDOUT2        					 	0x04		//!< Defines for PCA9955A register set
#define LEDOUT3        					 	0x05		//!< Defines for PCA9955A register set
#define GRPPWM         					 	0x06		//!< Defines for PCA9955A register set
#define GRPFREQ        					 	0x07		//!< Defines for PCA9955A register set
#define PWM0           					 	0x08		//!< Defines for PCA9955A register set
#define PWM1           					 	0x09		//!< Defines for PCA9955A register set
#define PWM2           					 	0x0A		//!< Defines for PCA9955A register set
#define PWM3           					 	0x0B		//!< Defines for PCA9955A register set
#define PWM4           					 	0x0C		//!< Defines for PCA9955A register set
#define PWM5           					 	0x0D		//!< Defines for PCA9955A register set
#define PWM6           					 	0x0E		//!< Defines for PCA9955A register set
#define PWM7           					 	0x0F		//!< Defines for PCA9955A register set
#define PWM8           					 	0x10		//!< Defines for PCA9955A register set
#define PWM9           					 	0x11		//!< Defines for PCA9955A register set
#define PWM10          					 	0x12		//!< Defines for PCA9955A register set
#define PWM11          					 	0x13		//!< Defines for PCA9955A register set
#define PWM12          					 	0x14		//!< Defines for PCA9955A register set
#define PWM13          					 	0x15		//!< Defines for PCA9955A register set
#define PWM14          					 	0x16		//!< Defines for PCA9955A register set
#define PWM15          					 	0x17		//!< Defines for PCA9955A register set
#define IREF0          					 	0x18		//!< Defines for PCA9955A register set
#define IREF1          					 	0x19		//!< Defines for PCA9955A register set
#define IREF2          					 	0x1A		//!< Defines for PCA9955A register set
#define IREF3          					 	0x1B		//!< Defines for PCA9955A register set
#define IREF4          					 	0x1C		//!< Defines for PCA9955A register set
#define IREF5          					 	0x1D		//!< Defines for PCA9955A register set
#define IREF6          					 	0x1E		//!< Defines for PCA9955A register set
#define IREF7          					 	0x1F		//!< Defines for PCA9955A register set
#define IREF8          					 	0x20		//!< Defines for PCA9955A register set
#define IREF9          					 	0x21		//!< Defines for PCA9955A register set
#define IREF10         					 	0x22		//!< Defines for PCA9955A register set
#define IREF11         					 	0x23		//!< Defines for PCA9955A register set
#define IREF12         					 	0x24		//!< Defines for PCA9955A register set
#define IREF13         					 	0x25		//!< Defines for PCA9955A register set
#define IREF14         					 	0x26		//!< Defines for PCA9955A register set
#define IREF15         					 	0x27		//!< Defines for PCA9955A register set
#define RAMP_RATE_GRP0 					 	0x28		//!< Defines for PCA9955A register set
#define STEP_TIME_GRP0 					 	0x29		//!< Defines for PCA9955A register set
#define HOLD_CNTL_GRP0 					 	0x2A		//!< Defines for PCA9955A register set
#define IREF_GRP0      					 	0x2B		//!< Defines for PCA9955A register set
#define RAMP_RATE_GRP1 					 	0x2C		//!< Defines for PCA9955A register set
#define STEP_TIME_GRP1 					 	0x2D		//!< Defines for PCA9955A register set
#define HOLD_CNTL_GRP1 					 	0x2E		//!< Defines for PCA9955A register set
#define IREF_GRP1      					 	0x2F		//!< Defines for PCA9955A register set
#define RAMP_RATE_GRP2 					 	0x30		//!< Defines for PCA9955A register set
#define STEP_TIME_GRP2 					 	0x31		//!< Defines for PCA9955A register set
#define HOLD_CNTL_GRP2 					 	0x32		//!< Defines for PCA9955A register set
#define IREF_GRP2      					 	0x33		//!< Defines for PCA9955A register set
#define RAMP_RATE_GRP3 					 	0x34		//!< Defines for PCA9955A register set
#define STEP_TIME_GRP3 					 	0x35		//!< Defines for PCA9955A register set
#define HOLD_CNTL_GRP3 					 	0x36		//!< Defines for PCA9955A register set
#define IREF_GRP3      					 	0x37		//!< Defines for PCA9955A register set
#define GRAD_MODE_SEL0 					 	0x38		//!< Defines for PCA9955A register set
#define GRAD_MODE_SEL1 					 	0x39		//!< Defines for PCA9955A register set
#define GRAD_GRP_SEL0  					 	0x3A		//!< Defines for PCA9955A register set
#define GRAD_GRP_SEL1  					 	0x3B		//!< Defines for PCA9955A register set
#define GRAD_GRP_SEL2 					 	0x3C		//!< Defines for PCA9955A register set
#define GRAD_GRP_SEL3  					 	0x3D		//!< Defines for PCA9955A register set
#define GRAD_CNTL      					 	0x3E		//!< Defines for PCA9955A register set
#define OFFSET         					 	0x3F		//!< Defines for PCA9955A register set
#define SUBADR1        					 	0x40		//!< Defines for PCA9955A register set
#define SUBADR2        					 	0x41		//!< Defines for PCA9955A register set
#define SUBADR3        					 	0x42		//!< Defines for PCA9955A register set
#define ALLCALLADDR    					 	0x43		//!< Defines for PCA9955A register set
#define PWMALL         					 	0x44		//!< Defines for PCA9955A register set
#define IREFALL        					 	0x45		//!< Defines for PCA9955A register set
#define EFLAG0         					 	0x46		//!< Defines for PCA9955A register set
#define EFLAG1         					 	0x47		//!< Defines for PCA9955A register set
#define EFLAG2         					 	0x48		//!< Defines for PCA9955A register set
#define EFLAG3         					 	0x49		//!< Defines for PCA9955A register set
#define AUTO_INCREMENT						0x80		//!< Defines the auto increment bit


/* Given index to leds fpr processing*/
#define LED1           					 	0				//!< Defines for better readability in switch condition LED-index dedicated to LED-index-1
#define LED2           					 	1				//!< Defines for better readability in switch condition LED-index dedicated to LED-index-1
#define LED3           					 	2				//!< Defines for better readability in switch condition LED-index dedicated to LED-index-1
#define LED4           					 	3				//!< Defines for better readability in switch condition LED-index dedicated to LED-index-1
#define LED5           					 	4				//!< Defines for better readability in switch condition LED-index dedicated to LED-index-1
#define LED6           					 	5				//!< Defines for better readability in switch condition LED-index dedicated to LED-index-1
#define LED7           					 	6				//!< Defines for better readability in switch condition LED-index dedicated to LED-index-1
#define LED8           					 	7				//!< Defines for better readability in switch condition LED-index dedicated to LED-index-1
#define LED9           					 	8				//!< Defines for better readability in switch condition LED-index dedicated to LED-index-1

#define LED_COUNT 9U


#if (1U == UID_HW_VERSION)
/* hardware v1 */
#define LED1_RED_PWM   					 	PWM0		//!< Define to dedicate LED to PCA9955A register set
#define LED1_RED_DRV   					 	PCA9955A_1	    //!< Define to dedicate LED to PCA9955A register set
#define LED1_GREEN_PWM 					 	PWM11		//!< Define to dedicate LED to PCA9955A register set
#define LED1_GREEN_DRV 					 	PCA9955A_1		  //!< Define to dedicate LED to PCA9955A register set
#define LED1_BLUE_PWM  					 	PWM6		//!< Define to dedicate LED to PCA9955A register set
#define LED1_BLUE_DRV  					 	PCA9955A_2  		//!< Define to dedicate LED to PCA9955A register set

#define LED2_RED_PWM   					 	PWM1		//!< Define to dedicate LED to PCA9955A register set
#define LED2_RED_DRV   					 	PCA9955A_1  		//!< Define to dedicate LED to PCA9955A register set
#define LED2_GREEN_PWM 					 	PWM12		//!< Define to dedicate LED to PCA9955A register set
#define LED2_GREEN_DRV 					 	PCA9955A_1		  //!< Define to dedicate LED to PCA9955A register set
#define LED2_BLUE_PWM  					 	PWM7		//!< Define to dedicate LED to PCA9955A register set
#define LED2_BLUE_DRV  					 	PCA9955A_2  		//!< Define to dedicate LED to PCA9955A register set

#define LED3_RED_PWM   					 	PWM2		//!< Define to dedicate LED to PCA9955A register set
#define LED3_RED_DRV   					 	PCA9955A_1  		//!< Define to dedicate LED to PCA9955A register set
#define LED3_GREEN_PWM 					 	PWM13		//!< Define to dedicate LED to PCA9955A register set
#define LED3_GREEN_DRV 					 	PCA9955A_1	  	//!< Define to dedicate LED to PCA9955A register set
#define LED3_BLUE_PWM  					 	PWM8		//!< Define to dedicate LED to PCA9955A register set
#define LED3_BLUE_DRV  					 	PCA9955A_2  		//!< Define to dedicate LED to PCA9955A register set

#define LED4_RED_PWM   					 	PWM3		//!< Define to dedicate LED to PCA9955A register set
#define LED4_RED_DRV   					 	PCA9955A_1  		//!< Define to dedicate LED to PCA9955A register set
#define LED4_GREEN_PWM 					 	PWM14		//!< Define to dedicate LED to PCA9955A register set
#define LED4_GREEN_DRV 					 	PCA9955A_1  		//!< Define to dedicate LED to PCA9955A register set
#define LED4_BLUE_PWM  					 	PWM9		//!< Define to dedicate LED to PCA9955A register set
#define LED4_BLUE_DRV  					 	PCA9955A_2  		//!< Define to dedicate LED to PCA9955A register set

#define LED5_RED_PWM   					 	PWM4		//!< Define to dedicate LED to PCA9955A register set
#define LED5_RED_DRV   					 	PCA9955A_1  		//!< Define to dedicate LED to PCA9955A register set
#define LED5_GREEN_PWM 					 	PWM15		//!< Define to dedicate LED to PCA9955A register set
#define LED5_GREEN_DRV 					 	PCA9955A_1		  //!< Define to dedicate LED to PCA9955A register set
#define LED5_BLUE_PWM  					 	PWM10		//!< Define to dedicate LED to PCA9955A register set
#define LED5_BLUE_DRV  					 	PCA9955A_2  		//!< Define to dedicate LED to PCA9955A register set

#define LED6_RED_PWM   					 	PWM5		//!< Define to dedicate LED to PCA9955A register set
#define LED6_RED_DRV   					 	PCA9955A_1  		//!< Define to dedicate LED to PCA9955A register set
#define LED6_GREEN_PWM 					 	PWM0		//!< Define to dedicate LED to PCA9955A register set
#define LED6_GREEN_DRV 					 	PCA9955A_2		  //!< Define to dedicate LED to PCA9955A register set
#define LED6_BLUE_PWM  					 	PWM11		//!< Define to dedicate LED to PCA9955A register set
#define LED6_BLUE_DRV  					 	PCA9955A_2   		//!< Define to dedicate LED to PCA9955A register set

#define LED7_RED_PWM   					 	PWM6		//!< Define to dedicate LED to PCA9955A register set
#define LED7_RED_DRV   					 	PCA9955A_1  		//!< Define to dedicate LED to PCA9955A register set
#define LED7_GREEN_PWM 					 	PWM1		//!< Define to dedicate LED to PCA9955A register set
#define LED7_GREEN_DRV 					 	PCA9955A_2		  //!< Define to dedicate LED to PCA9955A register set
#define LED7_BLUE_PWM  					 	PWM12		//!< Define to dedicate LED to PCA9955A register set
#define LED7_BLUE_DRV  					 	PCA9955A_2   		//!< Define to dedicate LED to PCA9955A register set

#define LED8_RED_PWM   					 	PWM7		//!< Define to dedicate LED to PCA9955A register set
#define LED8_RED_DRV   					 	PCA9955A_1  		//!< Define to dedicate LED to PCA9955A register set
#define LED8_GREEN_PWM 					 	PWM2		//!< Define to dedicate LED to PCA9955A register set
#define LED8_GREEN_DRV 					 	PCA9955A_2		  //!< Define to dedicate LED to PCA9955A register set
#define LED8_BLUE_PWM  					 	PWM13		//!< Define to dedicate LED to PCA9955A register set
#define LED8_BLUE_DRV  					 	PCA9955A_2   		//!< Define to dedicate LED to PCA9955A register set

#define LED9_RED_PWM   					 	PWM8		//!< Define to dedicate LED to PCA9955A register set
#define LED9_RED_DRV   					 	PCA9955A_1  		//!< Define to dedicate LED to PCA9955A register set
#define LED9_GREEN_PWM 					 	PWM3		//!< Define to dedicate LED to PCA9955A register set
#define LED9_GREEN_DRV 					 	PCA9955A_2		  //!< Define to dedicate LED to PCA9955A register set
#define LED9_BLUE_PWM  					 	PWM14		//!< Define to dedicate LED to PCA9955A register set
#define LED9_BLUE_DRV  					 	PCA9955A_2   		//!< Define to dedicate LED to PCA9955A register set

#elif ((2U == UID_HW_VERSION) || (3U == UID_HW_VERSION))
/* hardware v2 or v3 */
#define LED1_RED_PWM   					 	PWM0		//!< Define to dedicate LED to PCA9955A register set
#define LED1_RED_DRV   					 	PCA9955A_1	    //!< Define to dedicate LED to PCA9955A register set
#define LED1_GREEN_PWM 					 	PWM1 		//!< Define to dedicate LED to PCA9955A register set
#define LED1_GREEN_DRV 					 	PCA9955A_1		  //!< Define to dedicate LED to PCA9955A register set
#define LED1_BLUE_PWM  					 	PWM2		//!< Define to dedicate LED to PCA9955A register set
#define LED1_BLUE_DRV  					 	PCA9955A_1  		//!< Define to dedicate LED to PCA9955A register set

#define LED2_RED_PWM   					 	PWM3		//!< Define to dedicate LED to PCA9955A register set
#define LED2_RED_DRV   					 	PCA9955A_1  		//!< Define to dedicate LED to PCA9955A register set
#define LED2_GREEN_PWM 					 	PWM4 		//!< Define to dedicate LED to PCA9955A register set
#define LED2_GREEN_DRV 					 	PCA9955A_1		  //!< Define to dedicate LED to PCA9955A register set
#define LED2_BLUE_PWM  					 	PWM5		//!< Define to dedicate LED to PCA9955A register set
#define LED2_BLUE_DRV  					 	PCA9955A_1  		//!< Define to dedicate LED to PCA9955A register set

#define LED3_RED_PWM   					 	PWM6		//!< Define to dedicate LED to PCA9955A register set
#define LED3_RED_DRV   					 	PCA9955A_1  		//!< Define to dedicate LED to PCA9955A register set
#define LED3_GREEN_PWM 					 	PWM7 		//!< Define to dedicate LED to PCA9955A register set
#define LED3_GREEN_DRV 					 	PCA9955A_1	  	//!< Define to dedicate LED to PCA9955A register set
#define LED3_BLUE_PWM  					 	PWM8		//!< Define to dedicate LED to PCA9955A register set
#define LED3_BLUE_DRV  					 	PCA9955A_1  		//!< Define to dedicate LED to PCA9955A register set

#define LED4_RED_PWM   					 	PWM9		//!< Define to dedicate LED to PCA9955A register set
#define LED4_RED_DRV   					 	PCA9955A_1  		//!< Define to dedicate LED to PCA9955A register set
#define LED4_GREEN_PWM 					 	PWM10		//!< Define to dedicate LED to PCA9955A register set
#define LED4_GREEN_DRV 					 	PCA9955A_1  		//!< Define to dedicate LED to PCA9955A register set
#define LED4_BLUE_PWM  					 	PWM11   //!< Define to dedicate LED to PCA9955A register set
#define LED4_BLUE_DRV  					 	PCA9955A_1  		//!< Define to dedicate LED to PCA9955A register set

#define LED5_RED_PWM   					 	PWM12 	//!< Define to dedicate LED to PCA9955A register set
#define LED5_RED_DRV   					 	PCA9955A_1  		//!< Define to dedicate LED to PCA9955A register set
#define LED5_GREEN_PWM 					 	PWM13		//!< Define to dedicate LED to PCA9955A register set
#define LED5_GREEN_DRV 					 	PCA9955A_1		  //!< Define to dedicate LED to PCA9955A register set
#define LED5_BLUE_PWM  					 	PWM14		//!< Define to dedicate LED to PCA9955A register set
#define LED5_BLUE_DRV  					 	PCA9955A_1  		//!< Define to dedicate LED to PCA9955A register set

#define LED6_RED_PWM   					 	PWM0		//!< Define to dedicate LED to PCA9955A register set
#define LED6_RED_DRV   					 	PCA9955A_2  		//!< Define to dedicate LED to PCA9955A register set
#define LED6_GREEN_PWM 					 	PWM1		//!< Define to dedicate LED to PCA9955A register set
#define LED6_GREEN_DRV 					 	PCA9955A_2		  //!< Define to dedicate LED to PCA9955A register set
#define LED6_BLUE_PWM  					 	PWM2 		//!< Define to dedicate LED to PCA9955A register set
#define LED6_BLUE_DRV  					 	PCA9955A_2   		//!< Define to dedicate LED to PCA9955A register set

#if (2U == UID_HW_VERSION)
#define LED7_RED_PWM   					 	PWM3		//!< Define to dedicate LED to PCA9955A register set
#define LED7_RED_DRV   					 	PCA9955A_2  		//!< Define to dedicate LED to PCA9955A register set
#define LED7_GREEN_PWM 					 	PWM4		//!< Define to dedicate LED to PCA9955A register set
#define LED7_GREEN_DRV 					 	PCA9955A_2		  //!< Define to dedicate LED to PCA9955A register set
#define LED7_BLUE_PWM  					 	PWM5 		//!< Define to dedicate LED to PCA9955A register set
#define LED7_BLUE_DRV  					 	PCA9955A_2   		//!< Define to dedicate LED to PCA9955A register set

#else /* hardware v3 */
#define LED7_RED_PWM   					 	PWM12		//!< Define to dedicate LED to PCA9955A register set
#define LED7_RED_DRV   					 	PCA9955A_2  		//!< Define to dedicate LED to PCA9955A register set
#define LED7_GREEN_PWM 					 	PWM13		//!< Define to dedicate LED to PCA9955A register set
#define LED7_GREEN_DRV 					 	PCA9955A_2		  //!< Define to dedicate LED to PCA9955A register set
#define LED7_BLUE_PWM  					 	PWM14		//!< Define to dedicate LED to PCA9955A register set
#define LED7_BLUE_DRV  					 	PCA9955A_2   		//!< Define to dedicate LED to PCA9955A register set
#endif

#define LED8_RED_PWM   					 	PWM6		//!< Define to dedicate LED to PCA9955A register set
#define LED8_RED_DRV   					 	PCA9955A_2  		//!< Define to dedicate LED to PCA9955A register set
#define LED8_GREEN_PWM 					 	PWM7		//!< Define to dedicate LED to PCA9955A register set
#define LED8_GREEN_DRV 					 	PCA9955A_2		  //!< Define to dedicate LED to PCA9955A register set
#define LED8_BLUE_PWM  					 	PWM8 		//!< Define to dedicate LED to PCA9955A register set
#define LED8_BLUE_DRV  					 	PCA9955A_2   		//!< Define to dedicate LED to PCA9955A register set

#define LED9_RED_PWM   					 	PWM9		//!< Define to dedicate LED to PCA9955A register set
#define LED9_RED_DRV   					 	PCA9955A_2  		//!< Define to dedicate LED to PCA9955A register set
#define LED9_GREEN_PWM 					 	PWM10 	//!< Define to dedicate LED to PCA9955A register set
#define LED9_GREEN_DRV 					 	PCA9955A_2		  //!< Define to dedicate LED to PCA9955A register set
#define LED9_BLUE_PWM  					 	PWM11		//!< Define to dedicate LED to PCA9955A register set
#define LED9_BLUE_DRV  					 	PCA9955A_2   		//!< Define to dedicate LED to PCA9955A register set

#endif
/******************************************************************/

  #define LED_IREF_OFF (0U)
  #define LED_IREF_LOW (10U)
  #define LED_IREF_FULL (255U)
  
  #define LED_GRADGRP_NONE 0U
  #define LED_GRADGRP_0    1U
  #define LED_GRADGRP_1    2U
  #define LED_GRADGRP_2    3U
  #define LED_GRADGRP_3    4U


	#define RGB_TxBUFFER_SIZE 10
	#define RGB_I2CMESSAGE_BUFFER_SIZE 60
	#define HAL_STOP_BIT_SET ((I2C1->ISR&I2C_ISR_STOPF)==I2C_ISR_STOPF)
	
	#define PB7_OUTPUT             GPIO_MODER_MODER7_0
	
//! Structure for I2C HAL
typedef struct _SI2CMessage {
    uint8_t slaveAddress;												//!< holds the color index for LED
    uint8_t numBytes;														//!< holds the blinkpattern for LED
		uint8_t I2CdataMessage[RGB_TxBUFFER_SIZE];	//!< holds the message to send via I2C
}SI2CMessage_St;
	
	
	//!RGB_TxBuffer is a 2-dimesional buffer for I2C data transfer
	static SI2CMessage_St I2Cmessage[RGB_I2CMESSAGE_BUFFER_SIZE];
	//static uint8_t RGB_TxBuffer[RGB_I2CMESSAGE_BUFFER_SIZE][RGB_TxBUFFER_SIZE];
	//!RGB_TxBuffRead is a control variable to control the RGB_TxBuffer
	static uint8_t RGB_TxBuffRead=0;
	//!RGB_TxBuffWrite is a control variable to control the RGB_TxBuffer
	static uint8_t RGB_TxBuffWrite=0;
	//!RGB_CurrentBufferPosition holds the actuall position from TxBuffer
	static uint8_t RGB_CurrentBufferPosition = 0;
	//!I2CtransferInProgress holds the transfer state
	static uint8_t I2CtransferInProgress = 0;


	void UID_RGBDriverSendI2CData(void);
	void SetSLED (uint32_t LED, uint32_t newState);
	void Init_PCA9955A (void);
	void HALInitRGBDriverGPIOs(void);
	
	#endif

