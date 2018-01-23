/**
  ******************************************************************************
  * @file    additions.c
  * @author  KBee
  * @version $Id: additions.c  2015-11-19 11:23:00Z sreitz $
  * @date    Oct. 2015
  * @brief   This file contains addon functions
  *
  ******************************************************************************
  */
	
	
	
	
/*if the IDE Keil µ-Vision displays an error in include-dependencies this error may be ignored
 *since CMSIS 4.4.0 there is something emberassing with the include dependencies the compiler compiles
 *instead.
 *For more information visit: https://devzone.nordicsemi.com/question/54252/keil-fake-error-__static_inline/
 ************************************************************************************************/




#include "additions.h" 

//-----------------------------------------------------------------------------
//! \brief ComputeCRC calculates a 12CRC over the desired datalength
//! \param[in]     *data     			pointer to the to CRC-Calc
//! \param[in]     length     		desired datalength
//! \param[out]    *crcResult     pointer to outputfield field[0] holds the higher 6 bit field[1] the lower 6
//-----------------------------------------------------------------------------
	void ComputeCRC(uint8_t *data, uint32_t length, uint8_t *crcResult)
	{
    /* Variable declaration */
    uint8_t current_byte;
    uint8_t input_bit;
    uint16_t reg;
    uint16_t poly;
    int32_t counter;

    /* Variable initialisation */
    current_byte = 0x00;
    input_bit = 0x00;
    reg = 0x0000;
    poly = 0x0F13;
    counter = 0;

    while (counter < (length << 3))
    {
        if ((counter % 8) == 0)
        {
            current_byte = data[counter >> 3];
        }
        input_bit = current_byte & 0x80;
        current_byte = current_byte << 1;

        if (((reg & 0x0800)>>4)^(input_bit))
        {
            reg = (reg << 1) ^ poly;
        }
        else
        {
            reg = (reg << 1);
        }
        reg = reg & 0x0FFF;
        counter++;
    }
		
	#if !CHECK(USE_DUMMY_CRC)		
  crcResult[0] = ENCODE((uint8_t)((reg&0x0FC0)>>6)); /* return 12bit CRC*/
  crcResult[1] = ENCODE((uint8_t)reg&0x003F);
	#else
  crcResult[0] = 'Y'; /* return 12bit CRC*/
  crcResult[1] = 'Y';		
	#endif
	}

