/*
    FreeRTOS V8.2.2 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "os_task.h"
#include "os_semphr.h"

/* FreeRTOS+CLI includes. */
#include "FreeRTOS_CLI.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

/* Demo app includes. */
#include "UDPCommandConsole.h"

/* Dimensions the buffer into which input characters are placed. */
#define cmdMAX_INPUT_SIZE	60

/* Dimensions the buffer into which string outputs can be placed. */
#define cmdMAX_OUTPUT_SIZE	1024

/* Dimensions the buffer passed to the recvfrom() call. */
#define cmdSOCKET_INPUT_BUFFER_SIZE 60

/* DEL acts as a backspace. */
#define cmdASCII_DEL		( 0x7F )

/* The socket used by the CLI and debug print messages. */
static Socket_t xSocket = FREERTOS_INVALID_SOCKET;

/* The socket be used for TX. */
static Socket_t tSocket;
/*
 * The task that runs FreeRTOS+CLI.
 */
static void prvUDPCommandInterpreterTask( void *pvParameters );

/*
 * Open and configure the UDP socket.
 */
static Socket_t prvOpenUDPServerSocket( uint16_t usPort );

/* Stores the UDP port on which the CLI will be presented. */
static uint32_t ulCLIPort = 0;

/* This is required as a parameter to maintain the sendto() Berkeley sockets
API - but it is not actually used so can take any value. */
static socklen_t xClientAddressLength = 0;

/*-----------------------------------------------------------*/

void vStartUDPCommandInterpreterTask( uint16_t usStackSize, uint32_t ulPort, UBaseType_t uxPriority )
{
	xTaskCreate( prvUDPCommandInterpreterTask, "UdpCmd", usStackSize, ( void * ) ulPort, uxPriority, NULL );
	ulCLIPort = ulPort;
}
/*-----------------------------------------------------------*/

void prvUDPCommandInterpreterTask( void *pvParameters )
{
int32_t lBytes, lByte;
char cRxedChar, cInputIndex = 0;
static char cInputString[ cmdMAX_INPUT_SIZE ], cOutputString[ cmdMAX_OUTPUT_SIZE ], cLocalBuffer[ cmdSOCKET_INPUT_BUFFER_SIZE ];
BaseType_t xMoreDataToFollow;
struct freertos_sockaddr xClient;

	memset( cInputString, 0x00, cmdMAX_INPUT_SIZE );

	/* Attempt to open the socket.  The port number is passed in the task
	parameter.  The strange casting is to remove compiler warnings on 32-bit
	machines. */
	xSocket = prvOpenUDPServerSocket( ( uint16_t ) ( ( uint32_t ) pvParameters ) & 0xffffUL );

	if( xSocket != FREERTOS_INVALID_SOCKET )
	{
		for( ;; )
		{
			/* Wait for incoming data on the opened socket. */
			lBytes = FreeRTOS_recvfrom( xSocket, ( void * ) cLocalBuffer, sizeof( cLocalBuffer ), 0, &xClient, &xClientAddressLength );

			if( lBytes > 0 )
			{
				/* Process each received byte in turn. */
				lByte = 0;
				while( lByte < lBytes )
				{
					/* The next character in the input buffer. */
					cRxedChar = cLocalBuffer[ lByte ];
					lByte++;

					/* Newline characters are taken as the end of the command
					string. */
					if( cRxedChar == '\n' )
					{
						/* Process the input string received prior to the
						newline. */
						do
						{
							/* Pass the string to FreeRTOS+CLI. */
							cOutputString[ 0 ] = 0x00;
							xMoreDataToFollow = FreeRTOS_CLIProcessCommand( cInputString, cOutputString, cmdMAX_OUTPUT_SIZE );

							/* Send the output generated by the command's
							implementation. */
							FreeRTOS_sendto( xSocket, cOutputString,  strlen( ( const char * ) cOutputString ), 0, &xClient, xClientAddressLength );

						  /* Until the command does not generate any more output. */
						} while( xMoreDataToFollow != pdFALSE );

						/* All the strings generated by the command processing
						have been sent.  Clear the input string ready to receive
						the next command. */
						cInputIndex = 0;
						memset( cInputString, 0x00, cmdMAX_INPUT_SIZE );

						/* Transmit a spacer, just to make the command console
						easier to read. */
						FreeRTOS_sendto( xSocket, "\r\n",  strlen( "\r\n" ), 0, &xClient, xClientAddressLength );
					}
					else
					{
						if( cRxedChar == '\r' )
						{
							/* Ignore the character.  Newlines are used to
							detect the end of the input string. */
						}
						else if( ( cRxedChar == '\b' ) || ( cRxedChar == cmdASCII_DEL ) )
						{
							/* Backspace was pressed.  Erase the last character
							in the string - if any. */
							if( cInputIndex > 0 )
							{
								cInputIndex--;
								cInputString[ ( int ) cInputIndex ] = '\0';
							}
						}
						else
						{
							/* A character was entered.  Add it to the string
							entered so far.  When a \n is entered the complete
							string will be passed to the command interpreter. */
							if( cInputIndex < cmdMAX_INPUT_SIZE )
							{
								cInputString[ ( int ) cInputIndex ] = cRxedChar;
								cInputIndex++;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		/* The socket could not be opened. */
		vTaskDelete( NULL );
	}
}
/*-----------------------------------------------------------*/

static Socket_t prvOpenUDPServerSocket( uint16_t usPort )
{
struct freertos_sockaddr xServer;
Socket_t xSocket = FREERTOS_INVALID_SOCKET;
TickType_t xSendTimeOut = 0;

	xSocket = FreeRTOS_socket( FREERTOS_AF_INET, FREERTOS_SOCK_DGRAM, FREERTOS_IPPROTO_UDP );
	if( xSocket != FREERTOS_INVALID_SOCKET)
	{
		/* Set to non-blocking sends with a timeout of zero as the socket might
		also be used for debug prints which should not block. */
		FreeRTOS_setsockopt( xSocket, 0, FREERTOS_SO_SNDTIMEO, &xSendTimeOut, sizeof( xSendTimeOut ) );

		/* Zero out the server structure. */
		memset( ( void * ) &xServer, 0x00, sizeof( xServer ) );

		/* Set family and port. */
		xServer.sin_port = FreeRTOS_htons( usPort );

		/* Bind the address to the socket. */
		if( FreeRTOS_bind( xSocket, &xServer, sizeof( xServer ) ) == -1 )
		{
			FreeRTOS_closesocket( xSocket );
			xSocket = FREERTOS_INVALID_SOCKET;
		}
	}

	return xSocket;
}
/*-----------------------------------------------------------*/




