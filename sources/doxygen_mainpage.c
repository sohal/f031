/*! \mainpage Documentation of the UID
 *
 * \section intro_sec Introduction
 *
 * This document contains the description of the firmware for the UID. It is not recommended to change the software without reading this documentation.
 *
 * \section overview_sec Overview
 *The UID firmware ist seperated into 3 application modules: the UID_RGBDriverPCA9955-module which handles the drive of the PCA9955A RGB Drivers, 
 *UID_Protocol-module which handles the communication an the commandset of an extern device connected via UART and the UID_Buttons-module which reads 
 *out external buttons. For higher realiability all known microprocessor specific peripherals are accesed via the Hardware Abstraction Layer (HAL-functions). 
 *On purpose the initialize HAL-functions are passed to module-interfaces. So that they can be called from the main.
 *<IMG SRC="architecture.png" id="loader">
 * \subsection uart UART-Dataflow
 *
 *The UID is interfaced to the JE via UART. The UART from the µC is in Interrupt mode. Incoming commands from JE are written into the circullar FIFO buffer 
 *“UID_RxBuffer” and parsed into the buffer “CmdInputFromJE”. Outgoing messages from the UID are written into the cricullar FIFO buffer UID_TxBuffer and 
 *outstreamt via the interrupt service routine. The fugure below shows the dataflow from the UID-Protocol module.
 *<IMG SRC="UID_UART_Dataflow.png" id="loader">
 * 
 */
