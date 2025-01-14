/** @file HL_sci.h
*   @brief SCI Driver Definition File
*   @date 20.May.2014
*   @version 04.00.00
*   
*/

/* (c) Texas Instruments 2009-2013, All rights reserved. */


#ifndef __SCI_H__
#define __SCI_H__

#include "HL_reg_sci.h"

#ifdef __cplusplus
extern "C" {
#endif

/* USER CODE BEGIN (0) */
/* USER CODE END */

/** @enum sciIntFlags
*   @brief Interrupt Flag Definitions
*
*   Used with sciEnableNotification, sciDisableNotification
*/
enum sciIntFlags
{
    SCI_FE_INT    = 0x04000000U,  /* framing error */
    SCI_OE_INT    = 0x02000000U,  /* overrun error */
    SCI_PE_INT    = 0x01000000U,  /* parity error */
    SCI_RX_INT    = 0x00000200U,  /* receive buffer ready */
    SCI_TX_INT    = 0x00000100U,  /* transmit buffer ready */
    SCI_WAKE_INT  = 0x00000002U,  /* wakeup */
    SCI_BREAK_INT = 0x00000001U   /* break detect */
};

/** @def SCI_IDLE
*   @brief Alias name for the SCI IDLE Flag
*
*   This is an alias name for the SCI IDLE Flag.
*
*/
#define SCI_IDLE 0x00000004U

/** @struct sciBase
*   @brief SCI Register Definition
*
*   This structure is used to access the SCI module registers.
*/
/** @typedef sciBASE_t
*   @brief SCI Register Frame Type Definition
*
*   This type is used to access the SCI Registers.
*/

enum sciPinSelect
{
    PIN_SCI_TX = 4U,
    PIN_SCI_RX = 2U
};


/* Configuration registers */
typedef struct sci_config_reg
{
    uint32 CONFIG_GCR0;
    uint32 CONFIG_GCR1;
    uint32 CONFIG_SETINT;
    uint32 CONFIG_SETINTLVL;
    uint32 CONFIG_FORMAT;
    uint32 CONFIG_BRS;
    uint32 CONFIG_PIO0;
    uint32 CONFIG_PIO1;
    uint32 CONFIG_PIO6;
    uint32 CONFIG_PIO7;	
	uint32 CONFIG_PIO8;
} sci_config_reg_t;





/** 
 *  @defgroup SCI SCI
 *  @brief Serial Communication Interface Module.
 *  
 *  The SCI module is a universal asynchronous receiver-transmitter that implements the standard nonreturn
 *  to zero format. The SCI can be used to communicate, for example, through an RS-232 port or over a K-line.
 *
 *	Related Files
 *   - HL_reg_sci.h
 *   - HL_sci.h
 *   - HL_sci.c
 *  @addtogroup SCI
 *  @{
 */
 
/* SCI Interface Functions */
void sciInit(void);
void sciSetFunctional(sciBASE_t *sci, uint32 port);
void sciSetBaudrate(sciBASE_t *sci, uint32 baud);
uint32  sciIsTxReady(sciBASE_t *sci);
void sciSendByte(sciBASE_t *sci, uint8 byte);
void sciSend(sciBASE_t *sci, uint32 length, uint8 * data);
uint32  sciIsRxReady(sciBASE_t *sci);
uint32  sciIsIdleDetected(sciBASE_t *sci);
uint32  sciRxError(sciBASE_t *sci);
uint32  sciReceiveByte(sciBASE_t *sci);
void sciReceive(sciBASE_t *sci, uint32 length, uint8 * data);
void sciEnableNotification(sciBASE_t *sci, uint32 flags);
void sciDisableNotification(sciBASE_t *sci, uint32 flags);
void sciEnableLoopback(sciBASE_t *sci, loopBackType_t Loopbacktype);
void sciDisableLoopback(sciBASE_t *sci);
/** @fn void sciNotification(sciBASE_t *sci, uint32 flags)
*   @brief Interrupt callback
*   @param[in] sci   - sci module base address
*   @param[in] flags - copy of error interrupt flags
*
* This is a callback that is provided by the application and is called upon
* an interrupt.  The parameter passed to the callback is a copy of the 
* interrupt flag register.
*/
void sciNotification(sciBASE_t *sci, uint32 flags);

/* USER CODE BEGIN (1) */
/* USER CODE END */

/**@}*/
#ifdef __cplusplus
}
#endif

#endif
