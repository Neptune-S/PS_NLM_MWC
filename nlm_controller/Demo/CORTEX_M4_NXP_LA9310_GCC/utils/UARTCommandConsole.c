/*
 *  FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
 *  All rights reserved
 *
 *  VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.
 *
 *  This file is part of the FreeRTOS distribution.
 *
 *  FreeRTOS is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License (version 2) as published by the
 *  Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.
 *
 ***************************************************************************
 *  >>!   NOTE: The modification to the GPL is included to allow you to     !<<
 *  >>!   distribute a combined work that includes FreeRTOS without being   !<<
 *  >>!   obliged to provide the source code for proprietary components     !<<
 *  >>!   outside of the FreeRTOS kernel.                                   !<<
 ***************************************************************************
 *
 *  FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  Full license text is available on the following
 *  link: http://www.freertos.org/a00114.html
 *
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
 *
 *  http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
 *  the FAQ page "My application does not run, what could be wrong?".  Have you
 *  defined configASSERT()?
 *
 *  http://www.FreeRTOS.org/support - In return for receiving this top quality
 *  embedded software for free we request you assist our global community by
 *  participating in the support forum.
 *
 *  http://www.FreeRTOS.org/training - Investing in training allows your team to
 *  be as productive as possible as early as possible.  Now you can receive
 *  FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
 *  Ltd, and the world's leading authority on the world's leading RTOS.
 *
 *  http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
 *  including FreeRTOS+Trace - an indispensable productivity tool, a DOS
 *  compatible FAT file system, and our tiny thread aware UDP/IP stack.
 *
 *  http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
 *  Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.
 *
 *  http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
 *  Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
 *  licenses offer ticketed support, indemnification and commercial middleware.
 *
 *  http://www.SafeRTOS.com - High Integrity Systems also provide a safety
 *  engineered and independently SIL3 certified version for use in safety and
 *  mission critical applications that require provable dependability.
 *
 *  1 tab == 4 spaces!
 */

/* Standard includes. */
#include "string.h"
#include "stdio.h"
#include "stdint.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Example includes. */
#include "FreeRTOS_CLI.h"
#include "UARTCommandConsole.h"

#include "debug_console.h"

/* Dimensions the buffer into which input characters are placed. */
#define cmdMAX_INPUT_SIZE    50

/* The maximum time in ticks to wait for the UART access mutex. */
#define cmdMAX_MUTEX_WAIT    ( 200 / portTICK_PERIOD_MS )

/*-----------------------------------------------------------*/

/*
 * The task that implements the command console processing.
 */
static void prvUARTCommandConsoleTask( void * pvParameters );

/*-----------------------------------------------------------*/

/* Const messages output by the command console. */
static const char * const pcWelcomeMessage = "FreeRTOS command server.\r\nType Help to view a list of registered commands.\r\n\r\n>";
static const char * const pcEndOfOutputMessage = "\r\n[Press ENTER to execute the previous command again]\r\n>";
static const char * const pcNewLine = "\r\n";

/*-----------------------------------------------------------*/

void vUARTConsoleStart( uint16_t usStackSize,
                               unsigned portBASE_TYPE uxPriority )
{
    	int ret;
	/* Create that task that handles the console itself. */
	ret =  xTaskCreate( prvUARTCommandConsoleTask, /* The task that implements the command console. */
                 "CLI",                     /* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
                 usStackSize,               /* The size of the stack allocated to the task. */
                 NULL,                      /* The parameter is not used, so NULL is passed. */
                 uxPriority,                /* The priority allocated to the task. */
                 NULL );                    /* A handle is not required, so just pass NULL. */
   
     if( ret != pdPASS )
     {
	    log_info("Failed to create task: %s\n\r", prvUARTCommandConsoleTask);
     }
     else{
            log_info("UART Command Task created: %s\n\r", prvUARTCommandConsoleTask);
    }
}
/*-----------------------------------------------------------*/

static void prvUARTCommandConsoleTask( void * pvParameters )
{
    char cRxedChar, * pcOutputString;
    int cInputIndex = 0;
    static char cInputString[ cmdMAX_INPUT_SIZE ], cLastInputString[ cmdMAX_INPUT_SIZE ];
    portBASE_TYPE xReturned;

    ( void ) pvParameters;

    /* Obtain the address of the output buffer.  Note there is no mutual
     * exclusion on this buffer as it is assumed only one command console
     * interface will be used at any one time. */
    pcOutputString = FreeRTOS_CLIGetOutputBuffer();

    /* Send the welcome message. */
    debug_printf( ( const char * ) pcWelcomeMessage );

//    log_info("UARTCommandConsoleTask returned: %d\n\r", debug_getchar( ( unsigned char * ) &cRxedChar ));
   
     for( ; ; )
    {
        /* Only interested in reading one character at a time. */
        while( debug_getchar( ( unsigned char * ) &cRxedChar ) == pdFALSE )
        {
        }

        /* Echo the character back. */
        debug_putchar( cRxedChar );

        /* Was it the end of the line? */
        if( ( cRxedChar == '\n' ) || ( cRxedChar == '\r' ) )
        {
            /* Just to space the output from the input. */
            debug_printf( ( const char * ) pcNewLine );

            /* See if the command is empty, indicating that the last command is
             * to be executed again. */
            if( cInputIndex == 0 )
            {
                /* Copy the last command back into the input string. */
                strcpy( ( char * ) cInputString, ( char * ) cLastInputString );
            }

            /* Pass the received command to the command interpreter.  The
             * command interpreter is called repeatedly until it returns pdFALSE
             * (indicating there is no more output) as it might generate more than
             * one string. */
            do
            {
                /* Get the next output string from the command interpreter. */
                xReturned = FreeRTOS_CLIProcessCommand( cInputString, pcOutputString, configCOMMAND_INT_MAX_OUTPUT_SIZE );

                /* Write the generated string to the UART. */
                debug_printf( ( const char * ) pcOutputString );
            } while( xReturned != pdFALSE );

            /* All the strings generated by the input command have been sent.
             * Clear the input	string ready to receive the next command.  Remember
             * the command that was just processed first in case it is to be
             * processed again. */
            strcpy( ( char * ) cLastInputString, ( char * ) cInputString );
            cInputIndex = 0;
            memset( cInputString, 0x00, cmdMAX_INPUT_SIZE );
            debug_printf( ( const char * ) pcEndOfOutputMessage, strlen( ( char * ) pcEndOfOutputMessage ) );
        }
        else
        {
            if( cRxedChar == '\r' )
            {
                /* Ignore the character. */
            }
            else if( cRxedChar == '\b' )
            {
                /* Backspace was pressed.  Erase the last character in the
                 * string - if any. */
                if( cInputIndex > 0 )
                {
                    cInputIndex--;
                    cInputString[ cInputIndex ] = '\0';
                }
            }
            else
            {
                /* A character was entered.  Add it to the string
                 * entered so far.  When a \n is entered the complete
                 * string will be passed to the command interpreter. */
                if( ( cRxedChar >= ' ' ) && ( cRxedChar <= '~' ) )
                {
                    if( cInputIndex < cmdMAX_INPUT_SIZE )
                    {
                        cInputString[ cInputIndex ] = cRxedChar;
                        cInputIndex++;
                    }
                }
            }
        }
    }
}
/*-----------------------------------------------------------*/
