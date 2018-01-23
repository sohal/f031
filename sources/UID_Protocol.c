/**
  ******************************************************************************
  * @file    UID_Protocol.c
  * @author  KBee
  * @version $Id: UID_Protocol.c  2015-11-02 09:19:00Z sreitz $
  * @date    Oct. 2015
  * @brief   This file contains the software for the UID_Protocol
	*          all functions needed to use the UID_Protocol are in here.
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
	#include "UID_Protocol.h"
	#include "UID_RGBDriverPCA9955.h"
	#include "additions.h"
	
	void USART1_IRQHandler(void);
	void UID_sendTxNACK(void);
	void UID_sendTxACK(void);
	void UID_ExecuteCommand(void);
	uint8_t UID_CommandParser(uint8_t DataByte);
	uint8_t writeUID_RxBuffer(uint8_t UARTinput);
	uint8_t readUID_RxBuffer(void);
	uint8_t checkUID_RxBufferEmpty(void);
	uint8_t writeUID_TxBuffer(uint8_t UARToutput);
	uint8_t readUID_TxBuffer(void);
	uint8_t checkUID_TxBufferEmpty(void);	
	void HALEnableRS232TxInterrupt(void);
	void HALDisableRS232TxInterrupt(void);

	void goToIdleState(void);
	void goToStoreCmdState(uint8_t Cmd);
	void storeCmd(uint8_t DataByte);
	
	uint8_t CheckIfCommandIsValid(uint8_t DataByte);
	int8_t getRemLength(uint8_t Cmd);
	
	
//-----------------------------------------------------------------------------
//! \brief stm32f031g4 specific interrupt handler 				
//-----------------------------------------------------------------------------			
	void USART1_IRQHandler(void)
	{

		if((USART1->ISR & USART_ISR_RXNE)==USART_ISR_RXNE) // messages income
		{
			uint8_t IncomeData;
			IncomeData=USART1->RDR;
			writeUID_RxBuffer(IncomeData);
		}
		
		if((USART1->ISR & USART_ISR_ORE)==USART_ISR_ORE)//If overrun occured
		{
			//reset UID_RxBuffer set parser to idle state clear overrunflag
			UID_RxBufferRead = UID_RxBufferWrite = 0;
			goToIdleState();
			USART1->ICR = USART1->ICR | USART_ICR_ORECF;
		}
		
		if(((USART1->ISR & USART_ISR_TXE)==USART_ISR_TXE)&&((USART1->CR1&USART_CR1_TXEIE)==USART_CR1_TXEIE))
		{
			if(checkUID_TxBufferEmpty())
			{
				HALDisableRS232TxInterrupt();
			}
			else
			{
				USART1->TDR=readUID_TxBuffer();
			}
		}
	}
	
	
//-----------------------------------------------------------------------------
//! \brief Checks the UID_RxBuffer
//! \return		 		  retruns true(1) if UID_RxBuffer is empty else false(0)						
//-----------------------------------------------------------------------------		
	uint8_t checkUID_RxBufferEmpty(void)
	{
		if(UID_RxBufferRead==UID_RxBufferWrite)
		{
			return(1);
		}
		else
		{
			return(0);
		}
	}
	
	
//-----------------------------------------------------------------------------
//! \brief reads data out of the UID_RxBuffer
//! \param[out]     retVal     	handles the data 
//! \return		 		  retVal								
//-----------------------------------------------------------------------------	
	uint8_t readUID_RxBuffer(void)
	{
		uint8_t retVal;
		retVal=UID_RxBuffer[UID_RxBufferRead];
		UID_RxBufferRead++;
		if(UID_RxBufferRead==UID_RxBUFFER_SIZE)
		{
			UID_RxBufferRead=0;
		}
		return(retVal);
	}
	
	
//-----------------------------------------------------------------------------
//! \brief writes data into the circullar buffer UID_RxBuffer
//! \param[in]     UARTinput     				the incoming data from UART
//! \return		 		 returns true(1) if buffer overflows else false(0)										
//-----------------------------------------------------------------------------	
	uint8_t writeUID_RxBuffer(uint8_t UARTinput)
	{
		UID_RxBuffer[UID_RxBufferWrite]=UARTinput;
		UID_RxBufferWrite++;
		if(UID_RxBufferWrite==UID_RxBUFFER_SIZE)
		{
			UID_RxBufferWrite=0;
		}
		if(UID_RxBufferWrite==UID_RxBufferRead)
		{
			return(1);	//Buffer overun
		}
		return(0);
	}
	
	
	
//-----------------------------------------------------------------------------
//! \brief Checks the UID_TxBuffer
//! \return		 		  retruns true(1) if UID_TxBuffer is empty else false(0)						
//-----------------------------------------------------------------------------		
	uint8_t checkUID_TxBufferEmpty(void)
	{
		if(UID_TxBufferRead==UID_TxBufferWrite)
		{
			return(1);
		}
		else
		{
			return(0);
		}
	}
	
	
	
//-----------------------------------------------------------------------------
//! \brief reads data out of the UID_TxBuffer
//! \param[out]     retVal     	handles the data 
//! \return		 		  retVal								
//-----------------------------------------------------------------------------	
	uint8_t readUID_TxBuffer(void)
	{
		uint8_t retVal;
		retVal=UID_TxBuffer[UID_TxBufferRead];
		UID_TxBufferRead++;
		if(UID_TxBufferRead==UID_TxBUFFER_SIZE)
		{
			UID_TxBufferRead=0;
		}
		return(retVal);
	}
	

//-----------------------------------------------------------------------------
//! \brief writes data into the circullar buffer UID_TxBuffer
//! \param[in]     UARToutput     				the desired output for UART
//! \return		 		 returns true(1) if buffer overflows else false(0)										
//-----------------------------------------------------------------------------	
	uint8_t writeUID_TxBuffer(uint8_t UARToutput)
		{
		UID_TxBuffer[UID_TxBufferWrite]=UARToutput;
		UID_TxBufferWrite++;
		if(UID_TxBufferWrite==UID_TxBUFFER_SIZE)
		{
			UID_TxBufferWrite=0;
		}
		if(UID_TxBufferWrite==UID_TxBufferRead)
		{
			return(1);	//Buffer overun
		}
		return(0);
	}
	
	
//-----------------------------------------------------------------------------
//! \brief (GLOBAL)Puts a data-array into the UART-outstream and enables the transmit-interrupt.
//! This function is in global scope all UID modules may it.	
//! TODO: should implement a safty-feature to avoid bufferoverflow
//! \param[in]     numBytes     				The number of bytes to transmit
//! \param[in]		 DataByte[]						The desired datafield (hint:overgiven with &datafield)					
//-----------------------------------------------------------------------------	
	void UID_RS232send(uint8_t numBytes, uint8_t DataByte[])
	{
		uint8_t msg;
		for(msg=0;msg<numBytes;++msg)
		{
			writeUID_TxBuffer(DataByte[msg]);
		}
		HALEnableRS232TxInterrupt();
	}
	
	
	
//-----------------------------------------------------------------------------
//! \brief UID_CheckCRC check the incoming command 
//-----------------------------------------------------------------------------
	uint8_t UID_CheckCRC(void)
	{
		uint8_t calculatedCRC[]={0,0};
		uint8_t length=0;
		length = getRemLength(CmdInputFromJE[0]);
		--length;
		ComputeCRC(&CmdInputFromJE[0],length,&calculatedCRC[0]);
		
		
		if((CmdInputFromJE[length]==calculatedCRC[0])&&(CmdInputFromJE[length+1]==calculatedCRC[1]))
		{
			return(1);
		}
		else
		{
			return(0);
		}
	}
	
	
	
//-----------------------------------------------------------------------------
//! \brief Executes a parsed command from CmdInputFromJE buffer
//-----------------------------------------------------------------------------
	void UID_ExecuteCommand(void)
	{
    uint8_t ui08Index;
    
		switch (CmdInputFromJE[0]&0xF0)
		{
			case 0xA0:
				SetSLED((CmdInputFromJE[0]&0x0F),DECODE(CmdInputFromJE[1]));
			break;

			case 0xB0:
        if (CmdSETALED == CmdInputFromJE[0])
        {
          for (ui08Index = 0; ui08Index < LED_COUNT; ui08Index++)
          {
            SetSLED(ui08Index,DECODE(CmdInputFromJE[ui08Index + 1U]));
          }
        }
				
			break;
		}
		UID_sendTxACK();
	}
	
	
	
//-----------------------------------------------------------------------------
//! \brief Checks if incoming cmd-byte is a valid UID command
//! \param[in]     Cmd     				The incoming command-byte via UART
//! \param[out]	   validCmd  			is true(1) if valid command occurs else false(0)
//! \return 			 validCmd
//-----------------------------------------------------------------------------
	uint8_t CheckIfCommandIsValid(uint8_t Cmd)
	{
		uint8_t validCmd;
		
		switch(Cmd)
		{
			case CmdSETSLED0:
				validCmd=1;
			break;
			
			case CmdSETSLED1:
				validCmd=1;
			break;

			case CmdSETSLED2:
				validCmd=1;
			break;

			case CmdSETSLED3:
				validCmd=1;
			break;


			case CmdSETSLED4:
				validCmd=1;
			break;


			case CmdSETSLED5:
				validCmd=1;
			break;


			case CmdSETSLED6:
				validCmd=1;
			break;


			case CmdSETSLED7:
				validCmd=1;
			break;


			case CmdSETSLED8:
				validCmd=1;
			break;

			
			case CmdSETALED:
				validCmd=1;
			break;

						
			case CmdSETGBR:
				validCmd=1;
			break;

									
			case CmdRESET:
				validCmd=1;
			break;


			default :
				validCmd=0;
			break;				
			
		}
		return(validCmd);
	}
	
//-----------------------------------------------------------------------------
//! \brief gets the redeem lenght depending on the incomming command. Is a helper function for UID_CommandParser
//! \param[in]     Cmd     				The incoming command-byte via UART
//! \param[out]	   MessageLength  The redeem length depending on command
//! \return				 MessageLength
//-----------------------------------------------------------------------------
	int8_t getRemLength(uint8_t Cmd)		
	{
		const int8_t MessageLengthSETSLED = 1 + CRC_CALC;
		const int8_t MessageLengthSETALED = 9 + CRC_CALC;
		const int8_t MessageLengthSETGBR = 1 + CRC_CALC;
		const int8_t MessageLengthRESET = 1 + CRC_CALC;
		int8_t MessageLength;
		switch(Cmd)
		{
			case CmdSETSLED0:
				MessageLength=MessageLengthSETSLED;
			break;

			case CmdSETSLED1:
				MessageLength=MessageLengthSETSLED;
			break;

			case CmdSETSLED2:
				MessageLength=MessageLengthSETSLED;
			break;

			case CmdSETSLED3:
				MessageLength=MessageLengthSETSLED;
			break;

			case CmdSETSLED4:
				MessageLength=MessageLengthSETSLED;
			break;

			case CmdSETSLED5:
				MessageLength=MessageLengthSETSLED;
			break;

			case CmdSETSLED6:
				MessageLength=MessageLengthSETSLED;
			break;

			case CmdSETSLED7:
				MessageLength=MessageLengthSETSLED;
			break;

			case CmdSETSLED8:
				MessageLength=MessageLengthSETSLED;
			break;;
			
			case CmdSETALED:
				MessageLength=MessageLengthSETALED;
			break;				
						
			case CmdSETGBR:
				MessageLength=MessageLengthSETGBR;
			break;					
									
			case CmdRESET:
				MessageLength=MessageLengthRESET;
			break;					

			default :
				while(1){/*invalid should never happen since just valid command are overgiven*/};						
		}
		return(MessageLength);
	}
	
//-----------------------------------------------------------------------------
//! \brief Set the parser into store-state. Is a helper function for UID_CommandParser
//! this funtion takes influence to the global static variables "ParserState", "DataPosition", "remLength" 
//! \param[in]     Cmd     		The incoming command-byte via UART
//-----------------------------------------------------------------------------
	void goToStoreCmdState(uint8_t Cmd)
	{
		if(DataPosition!=0)
		{
			DataPosition=0;
		}
		remLength=getRemLength(Cmd);
		CmdInputFromJE[DataPosition]=Cmd;
		DataPosition++;
		ParserState=PARSER_IN_STORE_CMD_STATE;
	}
	
//-----------------------------------------------------------------------------
//! \brief Sets the parser into idle-state. Is a helper function for UID_CommandParser
//! this funtion takes influence to the global static variables "ParserState", "DataPosition", "remLength" 
//-----------------------------------------------------------------------------
	void goToIdleState(void)
	{
		DataPosition=0;
		remLength=-1;
		ParserState=PARSER_IN_IDLE_STATE;
	}
	
//-----------------------------------------------------------------------------
//! \brief Stores into the CmdInputFromJE buffer. Is a helper function for UID_CommandParser
//! this funtion takes influence to the global static variables "DataPosition", "remLength" 	
//! \param[in]     Cmd     		The incoming command-byte via UART
//-----------------------------------------------------------------------------
	void storeCmd(uint8_t Cmd)
	{
		CmdInputFromJE[DataPosition]=Cmd;
		DataPosition++;
		remLength--;
	}
	
	
	
//-----------------------------------------------------------------------------
//! \brief The UID_CommandParser parses icoming commands into the CmdInputFromJE buffer.
//!
//! \param[in]     Cmd     		The incoming command-byte via UART
//! \param[out]    checkCRC   Is true if a whole command was parsed into CmdInputFromJE. else false
//! \return                		checkCRC
//-----------------------------------------------------------------------------
	uint8_t UID_CommandParser(uint8_t Cmd)
	{
		uint8_t checkCRC;
		
		switch (ParserState)
		{
			case PARSER_IN_IDLE_STATE:
				if((Cmd&RxSC)==RxSC)
				{
					if(CheckIfCommandIsValid(Cmd))
					{
						goToStoreCmdState(Cmd);
						checkCRC=0;						
					}
					else
					{
						UID_sendTxNACK();
						ParserState=PARSER_IN_IDLE_STATE;
						checkCRC=0;					
					}
				}
				else
				{
						checkCRC=0;
				}					
			break;

				
			case PARSER_IN_STORE_CMD_STATE:
				if((Cmd&RxSC)==RxSC)
				{
					if(CheckIfCommandIsValid(Cmd))
					{
						UID_sendTxNACK();
						goToStoreCmdState(Cmd);	
						checkCRC=0;						
					}
					else
					{
						UID_sendTxNACK();
						goToIdleState();	
						checkCRC=0;					
					}
				}
				else
				{
					if(remLength>0)
					{
						storeCmd(Cmd);
						checkCRC=0;
						if(remLength==0)	//If Cmd is complete a CRC calculation 
						{
							checkCRC=1;
						}						
					}
					else
					{
						while(1){/*should reach this condition*/};
					}
				}	
			break;				
			
			default:
				while(1){/*should never run in this state this whileloop is debug only*/};
		}	
		return(checkCRC);
	}
	
	
	
//-----------------------------------------------------------------------------
//!\brief (GLOBAL)The UID_RunCmdParser is callable from other modules. In the UID just the
//!main calls. First the checkUID_RxBufferEmpty-function is called to look
//!for new data. If new data is availiable the UID_CommandParser is called. After a whole
//!command was stored in CmdInputFromJE a CRC check is done.
//!if the CRC passes the command is executed. 
//-----------------------------------------------------------------------------	
	void UID_RunCmdParser(void)
	{
		uint8_t DataFromRxBuffer;
		if(checkUID_RxBufferEmpty())
		{
			return;
		}
		else																					//if databyte is availiable from UART
		{
			DataFromRxBuffer=readUID_RxBuffer();
			if(UID_CommandParser(DataFromRxBuffer))
			{
				if(UID_CheckCRC())
				{
					UID_ExecuteCommand();
					goToIdleState();
				}
				else
				{
					UID_sendTxNACK();
					goToIdleState();				
				}
			}
		}
	}
	
	
//-----------------------------------------------------------------------------
//! \brief (GLOBAL)This function is µController specific. In this case the USART1 module from stm32f031g4 is initialized
//-----------------------------------------------------------------------------
	void HALInitRS232GPIOs(void)
	{
		
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;																				// USART clock module is powered with clock
		
		NVIC->ISER[0] |= (1 << (USART1_IRQn & 0x1F));  															// enable the NVIC for UASRT1-Module
		
		USART1->BRR = SystemCoreClock / BAUDRATE_57600; 														// calculate Divisor from Baudrate
		/* Bits 29-31 of CR1 register must be kept at reset value */
		USART1->CR1 = USART1->CR1 | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;// enable transceiver mode
		USART1->CR1 = USART1->CR1 | USART_CR1_UE;																		// enable USART1 Module working from now
		
		RCC->AHBENR  |= RCC_AHBENR_GPIOAEN;																					// GPIOA clock mudole is powered with clock
		GPIOA->AFR[0] = PIN_A2_ALTFUNCT_USART_TX | PIN_A3_ALTFUNCT_USART_RX;				// enable alternative for PA2 and PA3/used for UART
		GPIOA->MODER = GPIOA->MODER | PA2_MODER_USART1_TX | PA3_MODER_USART1_RX;		// set the GPIO for alternate purpose		
	}
	
	
//-----------------------------------------------------------------------------
//! \brief stm32f031g4 specific enables UART interrupt									
//-----------------------------------------------------------------------------	
	void HALEnableRS232TxInterrupt(void)
	{
		USART1->CR1 = USART1->CR1|USART_CR1_TXEIE;
	}
	
//-----------------------------------------------------------------------------
//! \brief stm32f031g4 specific disables UART interrupt									
//-----------------------------------------------------------------------------	
	void HALDisableRS232TxInterrupt(void)
	{
		USART1->CR1 = USART1->CR1&0xFFFFFF7F;
	}
	
	
//-----------------------------------------------------------------------------
//! \brief sends NACK to JE									
//-----------------------------------------------------------------------------	
	void UID_sendTxNACK(void)
	{
		uint8_t out = TxNACK;
		UID_RS232send(1,&out);
	}
	
	
//-----------------------------------------------------------------------------
//! \brief sends ACK to JE									
//-----------------------------------------------------------------------------		
	void UID_sendTxACK(void)
	{
		uint8_t out = TxACK;
		UID_RS232send(1,&out);
	}
