/** @file HL_sys_main.c 
*   @brief Application main file
*   @date 11-Dec-2018
*   @version 04.07.01
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com  
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


/* USER CODE BEGIN (0) */
/* USER CODE END */

/* Include Files */


/* USER CODE BEGIN (1) */
#include "HL_sys_common.h"
#include "HL_gio.h"
#include "HL_sci.h"
#include "HL_etpwm.h"
#include "HL_system.h"

#include <string.h>
#include <stdio.h>
#define UART        sciREG1

void sci_display(sciBASE_t *sci, uint8 *text, uint32 len);
void pwm_init(void);
void wait(int);

uint32 receive_data;
uint32 value;
uint32 tmp;
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */
    char buf[256];
    unsigned int buf_len;
    gioInit();
    gioSetDirection(gioPORTA,0xfffffff);
    gioSetBit(gioPORTA,7,0);
    gioSetBit(gioPORTA,1,0);
    gioSetBit(gioPORTA,2,0);
    sciInit();
    etpwmInit();

    etpwmStartTBCLK();
    wait(10000);

    sprintf(buf, "SCI Configure Success!\n\r\0");
    buf_len = strlen(buf);
    sci_display(sciREG1, (uint8 *)buf, buf_len);

    sprintf(buf, "Press Proper Number!\n\r\0");
    buf_len = strlen(buf);
    sci_display(sciREG1, (uint8 *)buf, buf_len);

    for(;;)
    {
        tmp = sciReceiveByte(UART);

        receive_data = tmp - 48;

        sprintf(buf, "receive_data = %d\n\r\0", receive_data);
        buf_len = strlen(buf);
        sci_display(sciREG1, (uint8 *)buf, buf_len);

        pwm_init();

        sprintf(buf, "PWM Duty = %d\n\r\0", value);
        buf_len = strlen(buf);
        sci_display(sciREG1, (uint8 *)buf, buf_len);
    }



/* USER CODE END */

    return 0;
}


/* USER CODE BEGIN (4) */
void pwm_init(void)
{
    if(tmp == 48)  //ascii 0 :  0.4ms  전체주기 20ms    duty : 2%
    {
        gioSetBit(gioPORTA,7,0);
        gioSetBit(gioPORTA,1,0);
        gioSetBit(gioPORTA,2,0);

        gioSetBit(gioPORTA,1,1);

        etpwmSetCmpA(etpwmREG3, 1200);
        value = 1200;
        wait(10000);


    }
    else if(tmp == 49)
    {
        gioSetBit(gioPORTA,7,0);
        gioSetBit(gioPORTA,1,0);
        gioSetBit(gioPORTA,2,0);

        gioSetBit(gioPORTA,2,1);

        etpwmSetCmpA(etpwmREG3, 1190);
        value = 1195;
        wait(10000);
    }
    else if(tmp == 50)  // ascii 2 : 0.8ms  전체주기 20ms   duty : 4%
    {
        gioSetBit(gioPORTA,7,0);
        gioSetBit(gioPORTA,1,0);
        gioSetBit(gioPORTA,2,0);

        gioSetBit(gioPORTA,1,1);
        gioSetBit(gioPORTA,2,1);

        etpwmSetCmpA(etpwmREG3, 1189);
        value = 1190;
        wait(10000);
    }

    else if(tmp == 51)  // ascii 2 : 0.8ms  전체주기 20ms   duty : 4%
    {
        gioSetBit(gioPORTA,7,0);
        gioSetBit(gioPORTA,1,0);
        gioSetBit(gioPORTA,2,0);
        etpwmSetCmpA(etpwmREG3, 1188);
        value = 1185;
        wait(10000);
    }

    else if(tmp == 52)  // ascii 2 : 0.8ms  전체주기 20ms   duty : 4%
    {
        gioSetBit(gioPORTA,7,0);
        gioSetBit(gioPORTA,1,0);
        gioSetBit(gioPORTA,2,0);
        etpwmSetCmpA(etpwmREG3, 1187);
        value = 1182;
        wait(10000);
    }

    else if(tmp == 53)  // ascii 2 : 0.8ms  전체주기 20ms   duty : 4%
    {
        gioSetBit(gioPORTA,7,0);
        gioSetBit(gioPORTA,1,0);
        gioSetBit(gioPORTA,2,0);
        etpwmSetCmpA(etpwmREG3, 500);
        value = 500;
        wait(10000);
    }


}

void sci_display(sciBASE_t *sci, uint8 *text, uint32 len)
{
    while(len--)
    {
        while((UART->FLR & 0x4) == 4)
            ;
        sciSendByte(UART, *text++);
    }
}

void wait(int delay)
{
    int i;

    for(i = 0; i < delay; i++)
        ;
}
/* USER CODE END */
