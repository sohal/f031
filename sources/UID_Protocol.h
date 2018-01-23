/**
  ******************************************************************************
  * @file    UID_Protocol.h
  * @author  KBee
  * @version $Id: UID_Protocol.h  2015-11-02 09:19:00Z sreitz $
  * @date    Oct. 2015
  * @brief   This file contains the interfacedefinition for the UID_Protocol
	*          for including this softwaremodule to other projects
  *
  ******************************************************************************
  */
#ifndef UID_PROTOCOL
#define UID_PROTOCOL




	#include "stm32f0xx.h"
	
	#define CRC_CALC 2												//!< Adds 2 bytes to the regular command lenght is used to add 
	
	#define CmdSETSLED0 0xA0									//!< Defines the value for SETSLED Command for LED 1
	#define CmdSETSLED1 0xA1									//!< Defines the value for SETSLED Command for LED 2
	#define CmdSETSLED2 0xA2									//!< Defines the value for SETSLED Command for LED 3
	#define CmdSETSLED3 0xA3									//!< Defines the value for SETSLED Command for LED 4
	#define CmdSETSLED4 0xA4									//!< Defines the value for SETSLED Command for LED 5
	#define CmdSETSLED5 0xA5									//!< Defines the value for SETSLED Command for LED 6
	#define CmdSETSLED6 0xA6									//!< Defines the value for SETSLED Command for LED 7
	#define CmdSETSLED7 0xA7									//!< Defines the value for SETSLED Command for LED 8
	#define CmdSETSLED8 0xA8									//!< Defines the value for SETSLED Command for LED 9
	
	#define CmdSETALED 0xB4										//!< Defines the value for SETALED Command for all LED
	#define CmdSETGBR  0xB6										//!< Defines the value for SETGBR Command for all LED
	#define CmdRESET   0xBD										//!< Defines the value for RESET Command for all LED
	
	#define PARSER_IN_IDLE_STATE 0x00					//!< Defines the value for the parsers idle state
	#define PARSER_IN_STORE_CMD_STATE 0x01		//!< Defines the value for the parsers store command state
	#define MAX_CMD_LENGTH 12									//!< Defines the maximal command length			
	#define UID_RxBUFFER_SIZE 40							//!< Defines the size for FIFO circullar UART-Rx buffer
	#define UID_TxBUFFER_SIZE 25							//!< Defines the size for FIFO circullar UART-Tx buffer
	#define START_UP_REM_LENGTH -1						//!< Defines the startup value from redem length
	#define RxSC 0x80													//!< Defines the highest bit for Rx_Set_Command
	#define TxACK 0xDA												//!< Defines the value for acknolege from UID
	#define TxNACK 0xDD												//!< Defines the value for no acknolege from UID
	#define BAUDRATE_57600 57600							//!< Defines the baudrate value
	
	#define DECODE(CHAR) ((CHAR)-33)					//!< Defined macro subtracs value 33 from encoded data
	
	#define PA2_MODER_USART1_TX    GPIO_MODER_MODER2_1				//!< µC specific define depending UART Tx Pin
	#define PA3_MODER_USART1_RX    GPIO_MODER_MODER3_1				//!< µC specific define depending UART Rx Pin
	#define PIN_A2_ALTFUNCT_USART_TX  ((uint32_t)0x00000100)	//!< µC specific define depending UART Tx Pin
	#define PIN_A3_ALTFUNCT_USART_RX  ((uint32_t)0x00001000)	//!< µC specific define depending UART Rx Pin	



	//these global variables are read only for other modules. accessible just from UID_Protocol.c 

	// are belonging to parser
	//!Commands are parsed into the CmdInputFromJE buffer. CmdInputFromJE is accesible from protocolmodule. It is read only for other UID Modules
	static uint8_t CmdInputFromJE[MAX_CMD_LENGTH];
	//!Makes the redeem length global accesbile for all protocolmodule functions. It is read only for other UID Modules
	static int8_t  remLength = START_UP_REM_LENGTH;
	//!DataPosition is used to access CmdInputFromJE. Can be munipulated from protocolmodule read only for other modules
	static uint8_t DataPosition = 0;
	//!Holds the actual parser state. Can be munipulated from protocolmodule read only for other modules
	static uint8_t ParserState = PARSER_IN_IDLE_STATE;							//at StatUp Parser is in IdleState
	
	
	//!FIFO circullar buffer for incoming UART data. Can be munipulated from protocolmodule read only for other modules
	static uint8_t UID_RxBuffer[UID_RxBUFFER_SIZE];
	//!Read access for RxBuffer. Can be munipulated from protocolmodule read only for other modules
	static uint8_t UID_RxBufferRead = 0;
	//!Write access for RxBuffer. Can be munipulated from protocolmodule read only for other modules
	static uint8_t UID_RxBufferWrite = 0;
	
	//!FIFO circullar buffer for incoming UART outsream data. Can be munipulated from protocolmodule read only for other modules
	static uint8_t UID_TxBuffer[UID_TxBUFFER_SIZE];
	//!Read access for TxBuffer. Can be munipulated from protocolmodule read only for other modules
	static uint8_t UID_TxBufferRead = 0;
	//!Write access for TxBuffer. Can be munipulated from protocolmodule read only for other modules
	static uint8_t UID_TxBufferWrite = 0;
	
	
	//interface for main.c
	void HALInitRS232GPIOs(void);
	void UID_RunCmdParser(void);
	
	//interface for UID_Buttons.c
	void UID_RS232send(uint8_t numBytes,uint8_t DataByte[]);

	
#endif
