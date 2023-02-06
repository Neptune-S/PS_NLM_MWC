/*
 * FreeRTOS Kernel V10.0.1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Copyright 2019-2020 NXP
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/******************************************************************************
 *
 * See the following URL for information on the commands defined in this file:
 * http://www.FreeRTOS.org/FreeRTOS-Plus/FreeRTOS_Plus_UDP/Embedded_Ethernet_Examples/Ethernet_Related_CLI_Commands.shtml
 *
 ******************************************************************************/

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* FreeRTOS+CLI includes. */
#include "FreeRTOS_CLI.h"
#include "debug_console.h"
#include "common.h"
//#include "common_stat.h"
//#include "ipiQueue.h"
//#include "mem_log.h"
//#include "wdl_error_codes.h"
#include "stdio.h"
#include "stdlib.h"
#include "cmd_ctrl_vspa_intfce.h"
#include "phytimer.h"
#include "nmm_app.h"
#if defined(CONFIG_DSPI_GPO)
#include "dspi_gpo.h"
#endif
#include "rfic_api.h"
#include "rfic_dac.h"
#include "rfic_demod.h"
#ifdef TESTFRAMEWORK_ENABLE
#include "test_framework.h"

#endif
#include "timers.h"

#include <errno.h>
#ifndef configINCLUDE_TRACE_RELATED_CLI_COMMANDS
#define configINCLUDE_TRACE_RELATED_CLI_COMMANDS 0
#endif

enum eRficApis
{
    RFIC_SETBAND,
    RFIC_ADJPLLFREQ,
    RFIC_LNA_CTRL,
    RFIC_DEMOD_GAIN,
    RFIC_VGA_GAIN
};

FILE *fopen (const char *__restrict __filename, const char *__restrict __modes);
size_t fwrite (const void *__restrict __ptr, size_t __size, size_t __n, FILE *__restrict __s);

static portBASE_TYPE prvRasterScanCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE prvSSBScanCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE prvEchoCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE prvMDCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE prvCellFollowCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE prvCellFollowStopCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE prvMibCellFollowCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

static const CLI_Command_Definition_t xMibDecodeCommand =
{
	"cell_follow_mib", 			             		/* The command string to type. */
	"\r\ncell_follow_mib \r\n Usage :Start Mib decoding on the input gscn given as user argument \r\n",
	prvMibCellFollowCommand,                    /* The function to run. */
	1				   					   /* The user can enter one argument. */
};

static const CLI_Command_Definition_t xCellFollowStopCommand =
{
	"cell_follow_stop", 					/* The command string to type. */
	"\r\ncell_follow_stop \r\n Usage :Stop the Cell Follow Process, No user argument \r\n",
	prvCellFollowStopCommand,              /* The function to run. */
	0				   					   /* The user cannot enter any arguments. */
};

static const CLI_Command_Definition_t xCellFollowCommand =
{
	"cell_follow", 						   /* The command string to type. */
	"\r\ncell_follow \r\n Usage :Follow the cell on the input GSCN, input arguemnt of GSCN to follow \r\n",
	prvCellFollowCommand,                  /* The function to run. */
	1				   					   /* The user has to enter 1 argument of GSCN to do cell follow. */
};

static const CLI_Command_Definition_t xRasterScanCommand =
{
	"band_scan_n_77", 					   /* The command string to type. */
	"\r\nband_scan_n_77 \r\n Usage : Scan N77 band at all GSCNs, No user argument \r\n",
	prvRasterScanCommand,                  /* The function to run. */
	0				                       /* No input argument. */
};

static const CLI_Command_Definition_t xSSBScanCommand =
{
	"ssbscan", 							   /* The command string to type. */
	"\r\nssbscan \r\n Usage : Start scanning of SSB Block, No user argument\r\n",
	prvSSBScanCommand,                     /* The function to run. */
	0				                       /*  The user cannot enter any arguments.*/
};

static const CLI_Command_Definition_t xMDCommand =
{
	"md", 								   /* The command string to type. */
	"\r\nmd <32 bit address> - memory display takes 32 bit address to be read and number of addresses to display\r\n Usage : md <address><no. of words>\r\n",
	prvMDCommand, 						   /* The function to run. */
	2 									   /* User can enter two arguments(address value and number of address fields to display). */
};

static const CLI_Command_Definition_t xEchoCommand =
{
	"echoChk", 							   /* The command string to type. */
	"\r\nechoChk \r\n Usage : Takes variable number of parameters, echos each in turn\r\n",
	prvEchoCommand, 					   /* The function to run. */
	-1									   /* The user can enter any number of arguments. */
};

/*-----------------------------------------------------------*/
void vRegisterSampleCLICommands(void)
{
	if (pdPASS != FreeRTOS_CLIRegisterCommand(&xEchoCommand))
	{
		log_info("Registration of %s cmd failed.\n\r",
				xEchoCommand.pcCommand);
	}

	if (pdPASS != FreeRTOS_CLIRegisterCommand(&xSSBScanCommand))
	{
		log_info("Registration of %s cmd failed.\n\r",
				xSSBScanCommand.pcCommand);
	}

	if (pdPASS != FreeRTOS_CLIRegisterCommand( &xMDCommand))
	{
		log_info("Registration of %s cmd failed.\n\r",
				xMDCommand.pcCommand);
	}

	if (pdPASS != FreeRTOS_CLIRegisterCommand( &xRasterScanCommand))
	{
		log_info("Registration of %s cmd failed.\n\r",
				xRasterScanCommand.pcCommand);
	}	

	if (pdPASS != FreeRTOS_CLIRegisterCommand( &xCellFollowCommand))
	{
		log_info("Registration of %s cmd failed.\n\r",
				xCellFollowCommand.pcCommand);
	}

	if (pdPASS != FreeRTOS_CLIRegisterCommand( &xCellFollowStopCommand))
	{
		log_info("Registration of %s cmd failed.\n\r",
				xCellFollowStopCommand.pcCommand);
	}

	if (pdPASS != FreeRTOS_CLIRegisterCommand( &xMibDecodeCommand))
	{
		log_info("Registration of %s cmd failed.\n\r",
				xMibDecodeCommand.pcCommand);
	}
}

static portBASE_TYPE prvMibCellFollowCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	const char *pcParam2;
	BaseType_t lParameterStringLength;
	uint32_t ulTempVal2 = 0;

	/* Remove compile time warnings about unused parameters, and check the
	   write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	   write buffer length is adequate, so does not check for buffer overflows. */
	(void)pcCommandString;
	(void)xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	pcParam2 = FreeRTOS_CLIGetParameter( pcCommandString, 1, &lParameterStringLength );
    ulTempVal2 = strtoul( pcParam2, ( char ** ) NULL, 10 );
	cell_acquistion_response->gscn = ulTempVal2;
	
	sendCliCommandToVspa(CMD_CELL_FOLLOW_MIB);
	memset(pcWriteBuffer, 0x00, xWriteBufferLen);
	return pdFALSE;
}

static portBASE_TYPE prvSSBScanCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	/* Remove compile time warnings about unused parameters, and check the
	   write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	   write buffer length is adequate, so does not check for buffer overflows. */
	(void)pcCommandString;
	(void)xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	sendCliCommandToVspa(CMD_SSBSCAN);
	memset(pcWriteBuffer, 0x00, xWriteBufferLen);
	return pdFALSE;
}

static portBASE_TYPE prvRasterScanCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	/* Remove compile time warnings about unused parameters, and check the
	   write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	   write buffer length is adequate, so does not check for buffer overflows. */
	(void)pcCommandString;
	(void)xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	sendCliCommandToVspa(CMD_RASTERSCAN);
	memset(pcWriteBuffer, 0x00, xWriteBufferLen);
	return pdFALSE;
}

static portBASE_TYPE prvCellFollowStopCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	/* Remove compile time warnings about unused parameters, and check the
	   write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	   write buffer length is adequate, so does not check for buffer overflows. */
	(void)pcCommandString;
	(void)xWriteBufferLen;
	configASSERT(pcWriteBuffer);
	
	sendCliCommandToVspa(CMD_SSBSCAN_CELLFOLLOWSTOP);
	memset(pcWriteBuffer, 0x00, xWriteBufferLen);
	return pdFALSE;
}

static portBASE_TYPE prvCellFollowCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	const char *pcParam2;
	BaseType_t lParameterStringLength;
	uint32_t ulTempVal2 = 0;

	/* Remove compile time warnings about unused parameters, and check the
	   write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	   write buffer length is adequate, so does not check for buffer overflows. */
	(void)pcCommandString;
	(void)xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	pcParam2 = FreeRTOS_CLIGetParameter( pcCommandString, 1, &lParameterStringLength );
    ulTempVal2 = strtoul( pcParam2, ( char ** ) NULL, 10 );
	cell_acquistion_response->gscn = ulTempVal2;
	
	sendCliCommandToVspa(CMD_CELLFOLLOW);
	memset(pcWriteBuffer, 0x00, xWriteBufferLen);
	return pdFALSE;
}

static portBASE_TYPE prvMDCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	const char *pcAddr;
	const char *pcLen;
	static UBaseType_t uxParameterNumber;
	BaseType_t xParameterStringLength;
	volatile uint32_t xVal[4], xCount, xAddrHex, xBuffSize;
	volatile uint32_t *xAddrList;
	uint32_t iterCount = 0;

	/* Remove compile time warnings about unused parameters, and check the
	* 	   write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	* 	   	   write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );
	/* Obtain the first parameter string i.e. Address to be read */
	uxParameterNumber = 1U;
	pcAddr = FreeRTOS_CLIGetParameter
	(
		 pcCommandString,		/* The command string itself. */
		 uxParameterNumber,		/* Return the first parameter. */
		 &xParameterStringLength	/* Store the parameter string length. */
	);

	/* Even if No parameters were found.  Make sure the write buffer does
	* 	   not contain a valid string. */
	uxParameterNumber = 2U;
	pcLen = FreeRTOS_CLIGetParameter(pcCommandString,		/* The command string itself. */
					uxParameterNumber,		/* Return the first parameter. */		
					&xParameterStringLength /* Store the parameter string length. */
	);

	if( pcAddr != NULL )
	{
		PRINTF( "Writing to Terminal .....\r\n" );
		/* Convert string to Hex Address */
		xAddrHex = strtoul( pcAddr, ( char ** )NULL, 16 );
		xBuffSize = strtoul( pcLen, ( char ** )NULL, 16 );

		xAddrList = ( volatile uint32_t * )xAddrHex;
		errno = 0;

		{	
			iterCount = 0;
			while(iterCount < xBuffSize)
			{
				for ( xCount = 0; xCount < 4; xCount++ ) {
					xVal[ xCount ] = ( *( volatile uint32_t * )xAddrList );
					xAddrList += 1;
					iterCount += 1;
				}
	
				PRINTF( " %d %d %d %d\r\n", xVal[0], xVal[1], xVal[2], xVal[3] );
			}
	
		}

		PRINTF( "Writing to Terminal is Complete\r\n" );
	}

	memset(pcWriteBuffer, 0x00, xWriteBufferLen);
	return pdFALSE;
}

static portBASE_TYPE prvEchoCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	const char *pcParameter;
	BaseType_t xParameterStringLength, xReturn;
	static UBaseType_t uxParameterNumber = 0;
 	//BaseType_t xReturn = 1;
	/* Remove compile time warnings about unused parameters, and check the
	   write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	   write buffer length is adequate, so does not check for buffer overflows. */
	(void)pcCommandString;
	(void)xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	if (uxParameterNumber == 0)
	{
		/* The first time the function is called after the command has been
		   entered just a header string is returned. */
		sprintf(pcWriteBuffer, "The parameters were:\r\n");

		/* Next time the function is called the first parameter will be echoed
		   back. */
		uxParameterNumber = 1U;

		/* There is more data to be returned as no parameters have been echoed
		   back yet. */
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter(
			pcCommandString,		/* The command string itself. */
			uxParameterNumber,		/* Return the next parameter. */
			&xParameterStringLength /* Store the parameter string length. */
		);

		if (pcParameter != NULL)
		{
			/* Return the parameter string. */
			memset(pcWriteBuffer, 0x00, xWriteBufferLen);
			sprintf(pcWriteBuffer, "%d: ", (int)uxParameterNumber);
			strncat(pcWriteBuffer, (char *)pcParameter, (size_t)xParameterStringLength);
			strncat(pcWriteBuffer, "\r\n", strlen("\r\n"));

			/* There might be more parameters to return after this one. */
			xReturn = pdTRUE;
			uxParameterNumber++;
		}
		else
		{
			/* No more parameters were found.  Make sure the write buffer does
			   not contain a valid string. */
			pcWriteBuffer[0] = 0x00;

			/* No more data to return. */
			xReturn = pdFALSE;

			/* Start over the next time this command is executed. */
			uxParameterNumber = 0;
		}
	}

	return xReturn;
}

/*-----------------------------------------------------------*/
