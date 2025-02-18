/** @file HL_mibspi.h
*   @brief MIBSPI Driver Definition File
*   @date 02-Mar-2016
*   @version 04.05.02
*   
*/

/* 
* Copyright (C) 2009-2016 Texas Instruments Incorporated - www.ti.com  
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/



#ifndef __MIBSPI_H__
#define __MIBSPI_H__

#include "HL_reg_mibspi.h"

#ifdef __cplusplus
extern "C" {
#endif

/* USER CODE BEGIN (0) */
/* USER CODE END */

/** @enum triggerEvent
*   @brief Transfer Group Trigger Event
*/
enum triggerEvent
{
    TRG_NEVER   = 0U,
    TRG_RISING  = 1U,
    TRG_FALLING = 2U,
    TRG_BOTH    = 3U,
    TRG_HIGH    = 5U,
    TRG_LOW     = 6U,
    TRG_ALWAYS  = 7U
};

/** @enum triggerSource
*   @brief Transfer Group Trigger Source
*/
enum triggerSource
{
    TRG_DISABLED,
    TRG_GIOA0,
    TRG_GIOA1,
    TRG_GIOA2,
    TRG_GIOA3,
    TRG_GIOA4,
    TRG_GIOA5,
    TRG_GIOA6,
    TRG_GIOA7,
    TRG_HET1_8,
    TRG_HET1_10,
    TRG_HET1_12,
    TRG_HET1_14,
    TRG_HET1_16,
    TRG_HET1_18,
    TRG_TICK
};


/** @enum mibspiPinSelect
*   @brief mibspi Pin Select
*/
enum mibspiPinSelect
{
    PIN_CS0 = 0U,
    PIN_CS1 = 1U,
    PIN_CS2 = 2U,
    PIN_CS3 = 3U,
    PIN_CS4 = 4U,
    PIN_CS5 = 5U,
    PIN_CS6 = 6U,
    PIN_CS7 = 7U,
    PIN_ENA = 8U,
    PIN_CLK = 9U,
    PIN_SIMO = 10U,
    PIN_SOMI = 11U,
   PIN_SIMO_1 = 17U,
   PIN_SIMO_2 = 18U,
   PIN_SIMO_3 = 19U,
   PIN_SIMO_4 = 20U,
   PIN_SIMO_5 = 21U,
   PIN_SIMO_6 = 22U,
   PIN_SIMO_7 = 23U,
   PIN_SOMI_1 = 25U,
   PIN_SOMI_2 = 26U,
   PIN_SOMI_3 = 27U,
   PIN_SOMI_4 = 28U,
   PIN_SOMI_5 = 29U,
   PIN_SOMI_6 = 30U,
   PIN_SOMI_7 = 31U
};


/** @enum chipSelect
*   @brief Transfer Group Chip Select
*/
enum chipSelect
{
    CS_NONE = 0xFFU,
    CS_0    = 0xFEU,
    CS_1    = 0xFDU,
    CS_2    = 0xFBU,
    CS_3    = 0xF7U,
    CS_4    = 0xEFU,
    CS_5    = 0xDFU,
    CS_6    = 0xBFU,
    CS_7    = 0x7FU
};

/** @typedef mibspiPmode_t
*   @brief Mibspi Parellel mode Type Definition
*
*   This type is used to represent Mibspi Parellel mode.
*/
typedef enum mibspiPmode
{
    PMODE_NORMAL        = 0x0U,
    PMODE_2_DATALINE    = 0x1U,
    PMODE_4_DATALINE    = 0x2U,
    PMODE_8_DATALINE    = 0x3U
}mibspiPmode_t;

/** @typedef mibspiDFMT_t
*   @brief Mibspi Data format selection Type Definition
*
*   This type is used to represent Mibspi Data format selection.
*/
typedef enum mibspiDFMT
{
    DATA_FORMAT0    = 0x0U,
    DATA_FORMAT1    = 0x1U,
    DATA_FORMAT2    = 0x2U,
    DATA_FORMAT3    = 0x3U
}mibspiDFMT_t;

typedef struct mibspi_config_reg
{
    uint32 CONFIG_GCR1;
    uint32 CONFIG_INT0;
    uint32 CONFIG_LVL;
    uint32 CONFIG_PCFUN;
    uint32 CONFIG_PCDIR;
    uint32 CONFIG_PCPDR;
    uint32 CONFIG_PCDIS;
    uint32 CONFIG_PCPSL;
    uint32 CONFIG_DELAY;
    uint32 CONFIG_FMT0;
    uint32 CONFIG_FMT1;
    uint32 CONFIG_FMT2;
    uint32 CONFIG_FMT3;
    uint32 CONFIG_MIBSPIE;
    uint32 CONFIG_LTGPEND;
    uint32 CONFIG_TGCTRL[8U];
    uint32 CONFIG_PAR_ECC_CTRL;
}mibspi_config_reg_t;






/** 
 *  @defgroup MIBSPI MIBSPI
 *  @brief Multi-Buffered Serial Peripheral Interface Module.
 *  
 *  The MibSPI/MibSPIP is a high-speed synchronous serial input/output port that allows a serial bit stream of
 *  programmed length (2 to 16 bits) to be shifted in and out of the device at a programmed bit-transfer rate.
 *  The MibSPI has a programmable buffer memory that enables programmed transmission to be completed
 *  without CPU intervention
 *
 *	Related Files
 *   - HL_reg_mibspi.h
 *   - HL_mibspi.h
 *   - HL_mibspi.c
 *  @addtogroup MIBSPI
 *  @{
 */
 
/* MIBSPI Interface Functions */
void mibspiInit(void);
boolean mibspiIsBuffInitialized(mibspiBASE_t *mibspi);
void mibspiOutofReset(mibspiBASE_t *mibspi);
void mibspiReset(mibspiBASE_t *mibspi);
void mibspiSetFunctional(mibspiBASE_t *mibspi, uint32 port);
void mibspiSetData(mibspiBASE_t *mibspi, uint32 group, uint16 * data);
uint32 mibspiGetData(mibspiBASE_t *mibspi, uint32 group, uint16 * data);
void mibspiTransfer(mibspiBASE_t *mibspi, uint32 group);
boolean mibspiIsTransferComplete(mibspiBASE_t *mibspi, uint32 group);
void mibspiEnableGroupNotification(mibspiBASE_t *mibspi, uint32 group, uint32 level);
void mibspiDisableGroupNotification(mibspiBASE_t *mibspi, uint32 group);
void mibspiEnableLoopback(mibspiBASE_t *mibspi, loopBackType_t Loopbacktype);
void mibspiDisableLoopback(mibspiBASE_t *mibspi);
void mibspiPmodeSet(mibspiBASE_t *mibspi, mibspiPmode_t Pmode, mibspiDFMT_t DFMT);

/** @fn void mibspiNotification(mibspiBASE_t *mibspi, uint32 flags)
*   @brief Error interrupt callback
*   @param[in] mibspi   - mibSpi module base address
*   @param[in] flags - Copy of error interrupt flags
*
* This is a error callback that is provided by the application and is call upon
* an error interrupt.  The paramer passed to the callback is a copy of the error
* interrupt flag register.
*/
void mibspiNotification(mibspiBASE_t *mibspi, uint32 flags);


/** @fn void mibspiGroupNotification(mibspiBASE_t *mibspi, uint32 group)
*   @brief Transfer complete notification callback
*   @param[in] mibspi   - mibSpi module base address
*   @param[in] group - Transfer group
*
* This is a callback function provided by the application.  It is call when
* a transfer is complete.  The parameter is the transfer group that triggered
* the interrupt.
*/
void mibspiGroupNotification(mibspiBASE_t *mibspi, uint32 group);

/* USER CODE BEGIN (1) */
/* USER CODE END */

/**@}*/
#ifdef __cplusplus
}
#endif

#endif
