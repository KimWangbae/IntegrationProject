/** ***************************************************************************************************
 * @file main.c
 * @author Lovas Szilard <lovas.szilard@gmail.com>
 * @date 2016.02.22
 * @version 0.1
 * @copyright Lovas Szilard
 * GNU GENERAL PUBLIC LICENSE Version 2, June 1991
 *
 * Homepage: http://loszi.hu/works/ti_launchpad_freertos_demo/
 *
 * @brief FreeRTOS Demo project for Hercules LAUNCHXL2-570LC43 launchpad devboard.
 *
 * @details
 * Packages:
 * FreeRTOS+IO
 * FreeRTOS+CLI
 * FreeRTOS+TCP
 * FreeRTOS+FAT
 */

/* Include Files */
#include "HL_sys_common.h"

/* FreeRTOS headers */
#include "FreeRTOS.h"
#include "FreeRTOS_IO.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_semphr.h"

#include "math.h"
#include "stdio.h"
#include "stdlib.h"

/* HALCoGen generated headers has moved to sys_main.h*/
#include "sys_main.h"

/* Time related functions */
#include "rti_runtimestats.h"

/* CLI related headers */
#include "FreeRTOS_CLI.h"
#include "UARTCommandConsole.h"		/* FreeRTOS-Plus-UART-Console includes. */
#include "UDPCommandConsole.h"		/* FreeRTOS-Plus-UDP-Console includes. */
#include "CLI_commands.h"			/* CLI commands */

/* TCPIP related headers */
#include "FreeRTOSIPConfig.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "NetworkBufferManagement.h"
#include "FreeRTOS_TCP_server.h"

/* FreeRTOS+FAT includes. */
#include "ff_headers.h"
#include "ff_stdio.h"
#include "ff_ramdisk.h"

/* My Header */
#include "HL_etpwm.h"
#include "HL_gio.h"
#include "HL_sci.h"

uint8 emacAddress[6U] =	{0x00U, 0x08U, 0xEEU, 0x03U, 0xA6U, 0x6CU};
uint32 emacPhyAddress =	1U;
static const uint8_t ucIPAddress[4] = {configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3};
static const uint8_t ucNetMask[4] = {configNET_MASK0, configNET_MASK1, configNET_MASK2, configNET_MASK3};
static const uint8_t ucGatewayAddress[4] = {configGATEWAY_ADDR0, configGATEWAY_ADDR1, configGATEWAY_ADDR2, configGATEWAY_ADDR3};
static const uint8_t ucDNSServerAddress[4] = {configDNS_SERVER_ADDR0, configDNS_SERVER_ADDR1, configDNS_SERVER_ADDR2, configDNS_SERVER_ADDR3};

/* Task handlers */
xTaskHandle xTask1Handle, xTask2Handle, xServerWorkTaskHandle;
extern xTaskHandle xIPTaskHandle;

xTaskHandle xTask3Handle, xTask4Handle;

/* Tasks */
static void vTask1(void *pvParameters);
static void vTask2(void *pvParameters);

static void vTask3(void *pvParameters);
//static void vTask4(void *pvParameters);

static void vServerWorkTask(void *pvParameters);
void vStartNTPTask( uint16_t usTaskStackSize, UBaseType_t uxTaskPriority );

extern hdkif_t hdkif_data[MAX_EMAC_INSTANCE];
extern void vRegisterFileSystemCLICommands( void );
extern void vCreateAndVerifyExampleFiles( const char *pcMountPath );
extern void vStdioWithCWDTest( const char *pcMountPath );
float xConvertAdcValueToNtcTemperature(uint16_t xAdcValue, uint16_t xAdcMaxValue, float xR1, float xA, float xB, float xD);

/* Hook functions */
BaseType_t xApplicationDNSQueryHook( const char *pcName );
const char *pcApplicationHostnameHook( void );
void vApplicationTickHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName);

/* FTP and HTTP servers execute in the TCP server work task. */
#define mainTCP_SERVER_TASK_PRIORITY	( tskIDLE_PRIORITY + 2 )
#define	mainTCP_SERVER_STACK_SIZE		( configMINIMAL_STACK_SIZE * 12 )

/* RAM disk parameters */
#define mainRAM_DISK_SECTOR_SIZE	512UL
#define mainRAM_DISK_SECTORS		((200UL*1024UL) / mainRAM_DISK_SECTOR_SIZE)
#define mainIO_MANAGER_CACHE_SIZE	(15UL * mainRAM_DISK_SECTOR_SIZE)
/* RAM disk mount point */
#define mainRAM_DISK_NAME			"/ram"
//static uint8_t ucRAMDisk[ mainRAM_DISK_SECTORS * mainRAM_DISK_SECTOR_SIZE ] __attribute__ ((section(".sdram")));
static uint8_t ucRAMDisk[ mainRAM_DISK_SECTORS * mainRAM_DISK_SECTOR_SIZE ];

/** ***************************************************************************************************
 * @fn		void main(void)
 * @brief	main function.
 */

void main(void)
{
	/* Initialize HALCoGen driver. */
	gioInit();
	gioSetDirection(hetPORT1, 0xAA07C821);
	gioSetDirection(hetPORT2, 0x00000000);
	sciInit();
	adcInit();
	adcMidPointCalibration(adcREG1);
//   세마포어 만들면 프로그램 멈춤
//  vSemaphoreCreateBinary(mutex);

	_enable_IRQ();

	/* Register some commands to CLI */
#if ( configGENERATE_RUN_TIME_STATS == 1 )
	FreeRTOS_CLIRegisterCommand( &xTaskStats );
	FreeRTOS_CLIRegisterCommand( &xRunTimeStats );
#endif
	// FreeRTOS_CLIRegisterCommand( &xMemTest ); /* Tests onboard 8M external memory on TMDX570LC43HDK devboard (and also ruins the ram disk) */
	FreeRTOS_CLIRegisterCommand( &xEmacStat );
	FreeRTOS_CLIRegisterCommand( &xPing );
	FreeRTOS_CLIRegisterCommand( &xNetStat );
	FreeRTOS_CLIRegisterCommand( &xReset );

	/* Register some more filesystem related commands, like dir, cd, pwd ... */
	//vRegisterFileSystemCLICommands();
    //tskIDLE_PRIORITY + 3  | portPRIVILEGE_BIT
	xTaskCreate(vTask1, "HeartBeat", configMINIMAL_STACK_SIZE * 10, NULL, tskIDLE_PRIORITY + 3  | portPRIVILEGE_BIT, &xTask1Handle);
    xTaskCreate(vTask3, "PWM", configMINIMAL_STACK_SIZE * 10,NULL, tskIDLE_PRIORITY + 4  | portPRIVILEGE_BIT, &xTask3Handle);

    FreeRTOS_IPInit(ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, emacAddress);
	/* Start the command interpreter */
	vStartUARTCommandInterpreterTask();

	vTaskStartScheduler();
	while(1);
}

/** ***************************************************************************************************
 * @fn		void vTask1(void *pvParameters)
 * @brief	Creates RAM disk and its contents, blink USER LED 2
 */
void vTask1(void *pvParameters)
{
#if 0
	FF_Disk_t *pxDisk;

	/* Create the RAM disk. */
	pxDisk = FF_RAMDiskInit(mainRAM_DISK_NAME, ucRAMDisk, mainRAM_DISK_SECTORS, mainIO_MANAGER_CACHE_SIZE);
	configASSERT(pxDisk);

	/* Print out information on the disk. */
	FF_RAMDiskShowPartition(pxDisk);

	/* Create example files and web pages on the disk */
	vCreateAndVerifyExampleFiles(mainRAM_DISK_NAME);

	/* Creates temperature log task  */
	xTaskCreate(vTask2, "LogTask", configMINIMAL_STACK_SIZE * 10,NULL,2 | portPRIVILEGE_BIT, &xTask2Handle);

#endif
	/* After creating disk and files high priority not needed anymore. */

	vTaskPrioritySet(NULL,tskIDLE_PRIORITY);

	while(1)
	{
		/* USER LED 2 */
		gioToggleBit(gioPORTB, 6);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

/** ***************************************************************************************************
 * @fn		void vTask2(void *pvParameters)
 * @brief	Creates /ram/logfile.txt with onchip temperature sensor log data
 */
void vTask2(void *pvParameters)
{
	FF_FILE *pxLogFIle = NULL;
	FF_TimeStruct_t xTimeStruct;
	time_t uxCurrentSeconds;
	adcData_t Adc1Results[2];
	while(1)
	{
		pxLogFIle = ff_fopen("/ram/logfile.txt", "a+");
		uxCurrentSeconds = FreeRTOS_time(NULL);
		FreeRTOS_gmtime_r( &uxCurrentSeconds, &xTimeStruct );
		ff_fprintf(pxLogFIle,"%d/%d/%02d,%2d:%02d:%02d,",
				xTimeStruct.tm_mday,
				xTimeStruct.tm_mon + 1,
				xTimeStruct.tm_year + 1900,
				xTimeStruct.tm_hour,
				xTimeStruct.tm_min,
				xTimeStruct.tm_sec);
		adcStartConversion(adcREG1, adcGROUP1);
		while(adcIsConversionComplete(adcREG1, adcGROUP1) == 0);
		adcGetData(adcREG1, adcGROUP1, Adc1Results);
		/* OnChip temperature sensor 1 */
		ff_fprintf(pxLogFIle,"%2.2f\r\n",xConvertAdcValueToNtcTemperature(Adc1Results[1].value, 4095, 1000.0, 10.59719290e-3, -23.65584544e-4, 266.0378436e-7));
		ff_fclose(pxLogFIle);
		vTaskDelay(pdMS_TO_TICKS(60000)); /* Log period = 60 sec */
	}
}

void vTask3(void *pvParameters)
{
    // VCLK3 set to 37.5Mhz
    // 쓰기권한 없는 상태에서 건들면 안됌.
    etpwmInit();
    etpwmREG1->TBPRD = 37499U;
    etpwmREG1->CMPA = 0;
    uint16 pwm = 0;

    //vTaskPrioritySet(NULL,tskIDLE_PRIORITY);
    while(1)
    {
        if(pwm>37500)
            pwm = 37500;

        gioToggleBit(gioPORTB,7);

        etpwmREG1->CMPA = pwm++;

        // 괄호안의 숫자가 ms단위임.
        vTaskDelay(pdMS_TO_TICKS(120));
    }
}

static void vServerWorkTask(void *pvParameters)
{
TCPServer_t *pxTCPServer = NULL;
const TickType_t xInitialBlockTime = pdMS_TO_TICKS(200UL);

static const struct xSERVER_CONFIG xServerConfiguration[] =
	{
	/* Server type,		port number,	backlog, 	root dir. */
	{ eSERVER_HTTP, 	80, 			10, 		"/ram/web" },
	{ eSERVER_FTP,  	21, 			10, 		 "" }
	};

	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;

	/* Configuring RTI timer for serving system time. */
	vConfigureTimerForSysTime();
	vStartNTPTask(configMINIMAL_STACK_SIZE * 2, 4);

	/* Create the servers defined by the xServerConfiguration array above. */
	pxTCPServer = FreeRTOS_CreateTCPServer( xServerConfiguration, sizeof( xServerConfiguration ) / sizeof( xServerConfiguration[ 0 ] ) );
	configASSERT( pxTCPServer );

	while(1)
	{
		FreeRTOS_TCPServerWork(pxTCPServer, xInitialBlockTime);
	}
}


/** ***************************************************************************************************
 * @fn		const char *pcApplicationHostnameHook(void)
 * @brief	DHCP hostname register hook function.
 * @details
 * Assign the name defined with "mainDEVICE_NICK_NAME" to this network node during DHCP.
 */
BaseType_t xApplicationDNSQueryHook(const char *pcName)
{
BaseType_t xReturn;

	/* Determine if a name lookup is for this node.  Two names are given
	to this node: that returned by pcApplicationHostnameHook() and that set
	by mainDEVICE_NICK_NAME. */
	if( strcmp( pcName, pcApplicationHostnameHook() ) == 0 )
	{
		xReturn = pdPASS;
	}
	else if( strcmp( pcName, mainDEVICE_NICK_NAME ) == 0 )
	{
		xReturn = pdPASS;
	}
	else
	{
		xReturn = pdFAIL;
	}

	return xReturn;
}

/** ***************************************************************************************************
 * @fn		float xConvertAdcValueToNtcTemperature(uint16_t xAdcValue, uint16_t xAdcMaxValue, float xR1, float xA, float xB, float xD)
 * @brief	Converts raw ADC value into NTC sensor temperature using Steinhart-Hart equation.
 * @details
 * Converts raw ADC value (comes from the following circuit) into NTC sensor temperature, using Steinhart-Hart equation.
 *
 * Vref ---|
 *        |-|
 *   Rntc | |
 *        |-|
 *         |   |-------|
 *         o---|  ADC  |---->>> AdcValue
 *         |   |-------|
 *        |-|
 *    R1  | |
 *        |-|
 *  GND ---|
 *
 * @param	xAdcValue Raw ADC value
 * @param	xAdcMaxValue Maximum ADC value
 * @param	xR1 value of R1 resistor [Ohm]
 * @param	xA "A" NTC model parameter
 * @param	xB "B" NTC model parameter
 * @param	xD "D" NTC model parameter
 * @return	temperature [Celsius]
 */
float xConvertAdcValueToNtcTemperature(uint16_t xAdcValue, uint16_t xAdcMaxValue, float xR1, float xA, float xB, float xD)
{
	float xRntc, xReturn;
	const float xKelvinOffset = 272.15;
	if(xAdcMaxValue <= xAdcValue || xAdcValue == 0 || xR1 == 0.0 )
		{
		return(NAN);
		}
	xRntc = xR1 * (((float)xAdcMaxValue / (float)xAdcValue) - 1.0);
	xReturn = 1.0 / (xA + xB * logf(xRntc) + xD * powf(logf(xRntc),3)) - xKelvinOffset;
	return(xReturn);
}

void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
{
static BaseType_t xTasksAlreadyCreated = pdFALSE;

if( eNetworkEvent == eNetworkUp )
    {
        if( xTasksAlreadyCreated == pdFALSE )
        {
        	/* Start the UDP command line on port 5001 */
        	vStartUDPCommandInterpreterTask( mainUDP_CLI_TASK_STACK_SIZE, mainUDP_CLI_PORT_NUMBER, mainUDP_CLI_TASK_PRIORITY );

        	/* Start TCP server task (HTTP, FTP) */
        	xTaskCreate(vServerWorkTask, "TCPSrv", mainTCP_SERVER_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3 | portPRIVILEGE_BIT, &xServerWorkTaskHandle);

        	xTasksAlreadyCreated = pdTRUE;
        }

    }
}

/** ***************************************************************************************************
 * @fn		const char *pcApplicationHostnameHook(void)
 * @brief	DHCP hostname register hook function.
 * @details
 * Assign the name defined with "mainDEVICE_NICK_NAME" to this network node during DHCP.
 */
const char *pcApplicationHostnameHook(void)
{
	return mainDEVICE_NICK_NAME;
}

/** ***************************************************************************************************
 * @fn		void vApplicationTickHook(void)
 * @brief	TICK hook function.
 */
void vApplicationTickHook(void)
{
}

/** ***************************************************************************************************
 * @fn		void vApplicationIdleHook(void)
 * @brief	IDLE hook function.
 */
void vApplicationIdleHook(void)
{
}

/** ***************************************************************************************************
 * @fn		void vApplicationMallocFailedHook(void)
 * @brief	Malloc() fail hook function.
 */
void vApplicationMallocFailedHook(void)
{
	volatile uint32_t ulMallocFailures = 0;
	ulMallocFailures++;
}

/** ***************************************************************************************************
 * @fn		void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
 * @brief	Stack overflow hook function.
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
{
	( void ) pcTaskName;
	configASSERT(0);
}

#if(ipconfigSUPPORT_OUTGOING_PINGS == 1)
/** ***************************************************************************************************
 * @fn		void vApplicationPingReplyHook(ePingReplyStatus_t eStatus, uint16_t usIdentifier)
 * @brief	Ping reply hook function.
 */
void vApplicationPingReplyHook(ePingReplyStatus_t eStatus, uint16_t usIdentifier)
{
static const char *pcSuccess = "Ping reply received - identifier %d\r\n";
static const char *pcInvalidChecksum = "Ping reply received with invalid checksum - identifier %d\r\n";
static const char *pcInvalidData = "Ping reply received with invalid data - identifier %d\r\n";

	switch(eStatus)
	{
		case eSuccess	:
			FreeRTOS_printf((pcSuccess, (int)usIdentifier));
			break;
		case eInvalidChecksum :
			FreeRTOS_printf((pcInvalidChecksum, (int)usIdentifier));
			break;
		case eInvalidData :
			FreeRTOS_printf((pcInvalidData, (int)usIdentifier));
			break;
		default :
			/* It is not possible to get here as all enums have their own case. */
			break;
	}
	/* Prevent compiler warnings in case FreeRTOS_debug_printf() is not defined. */
	(void) usIdentifier;
}
#endif
