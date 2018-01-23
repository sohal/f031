/**
  ******************************************************************************
  * @file    additions.h
  * @author  KBee
  * @version additions.h  2015-11-19 11:33:00Z sreitz $
  * @date    Oct. 2015
  * @brief   This file contains algorithmic addions that may be used as addons
  *
  ******************************************************************************
  */
	
	
#ifndef ADDIOTIONS_H
#define ADDIOTIONS_H
	/*if the IDE Keil µ-Vision displays an error in include-dependencies this error may be ignored
 *since CMSIS 4.4.0 there is something emberassing with the include dependencies the compiler compiles
 *instead.
 *For more information visit: https://devzone.nordicsemi.com/question/54252/keil-fake-error-__static_inline/
 ************************************************************************************************/
#include "stm32f0xx.h"

#define ENCODE(CHAR) (((CHAR)&0x3f)+33)		//!< Macro define message encoding

//The following defines implements compiler switches an check routine
#define SWITCH(X) (X 
#define CHECK(X)   X)
//This is a switch to en- or disable CRC-Checking SWITCH(0)=Disable, SWITCH(1)=Enbale. Removing the next line without changing source end in compile error
#define USE_DUMMY_CRC SWITCH(1) 


void ComputeCRC(uint8_t *data, uint32_t length, uint8_t *crcResult);




#endif
