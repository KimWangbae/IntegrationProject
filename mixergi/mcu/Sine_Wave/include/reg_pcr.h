/** @file reg_pcr.h
*   @brief PCR Register Layer Header File
*   @date 16.Feb.2015
*   @version 04.03.00
*   
*   This file contains:
*   - Definitions
*   - Types
*   .
*   which are relevant for the System driver.
*/

/* 
* Copyright (C) 2009-2015 Texas Instruments Incorporated - www.ti.com 
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


#ifndef __REG_PCR_H__
#define __REG_PCR_H__

#include "sys_common.h"

/* USER CODE BEGIN (0) */
/* USER CODE END */

/* Pcr Register Frame Definition */
/** @struct pcrBase
*   @brief Pcr Register Frame Definition
*
*   This type is used to access the Pcr Registers.
*/
/** @typedef pcrBASE_t
*   @brief PCR Register Frame Type Definition
*
*   This type is used to access the PCR Registers.
*/
typedef volatile struct pcrBase
{
    uint32 PMPROTSET0;    /* 0x0000 */
    uint32 PMPROTSET1;    /* 0x0004 */
    uint32   rsvd1[2U];    /* 0x0008 */
    uint32 PMPROTCLR0;    /* 0x0010 */
    uint32 PMPROTCLR1;    /* 0x0014 */
    uint32   rsvd2[2U];    /* 0x0018 */
    uint32 PPROTSET0;     /* 0x0020 */
    uint32 PPROTSET1;     /* 0x0024 */
    uint32 PPROTSET2;     /* 0x0028 */
    uint32 PPROTSET3;     /* 0x002C */
    uint32   rsvd3[4U];    /* 0x0030 */
    uint32 PPROTCLR0;     /* 0x0040 */
    uint32 PPROTCLR1;     /* 0x0044 */
    uint32 PPROTCLR2;     /* 0x0048 */
    uint32 PPROTCLR3;     /* 0x004C */
    uint32   rsvd4[4U];    /* 0x0050 */
    uint32 PCSPWRDWNSET0; /* 0x0060 */
    uint32 PCSPWRDWNSET1; /* 0x0064 */
    uint32   rsvd5[2U];    /* 0x0068 */
    uint32 PCSPWRDWNCLR0; /* 0x0070 */
    uint32 PCSPWRDWNCLR1; /* 0x0074 */
    uint32   rsvd6[2U];    /* 0x0078 */
    uint32 PSPWRDWNSET0;  /* 0x0080 */
    uint32 PSPWRDWNSET1;  /* 0x0084 */
    uint32 PSPWRDWNSET2;  /* 0x0088 */
    uint32 PSPWRDWNSET3;  /* 0x008C */
    uint32   rsvd7[4U];    /* 0x0090 */
    uint32 PSPWRDWNCLR0;  /* 0x00A0 */
    uint32 PSPWRDWNCLR1;  /* 0x00A4 */
    uint32 PSPWRDWNCLR2;  /* 0x00A8 */
    uint32 PSPWRDWNCLR3;  /* 0x00AC */
} pcrBASE_t;

/** @def pcrREG
*   @brief Pcr Register Frame Pointer
*
*   This pointer is used by the system driver to access the Pcr registers.
*/
#define pcrREG ((pcrBASE_t *)0xFFFFE000U)

/* USER CODE BEGIN (1) */
/* USER CODE END */


#endif
