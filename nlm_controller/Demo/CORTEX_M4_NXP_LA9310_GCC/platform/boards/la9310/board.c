/*
 * Copyright 2017, 2021 NXP
 * All rights reserved.
 */

#include <common.h>
#include "config.h"
#include "immap.h"
#include "board.h"
#include <la9310.h>
#include <la9310_i2cAPI.h>
#include "debug_console.h"

void vBoardEarlyInit( void )
{
    xDebugConsoleInit( ( void * ) UART_BASEADDR, EARLY_UART_CLOCK_FREQUENCY,
                       UART_BAUDRATE );
}

void vBoardFinalInit( void )
{
    xDebugConsoleInit( ( void * ) UART_BASEADDR, FINAL_UART_CLOCK_FREQUENCY,
                       UART_BAUDRATE );

    iLa9310_I2C_Init( LA9310_FSL_I2C1, FINAL_I2C_CLOCK_FREQUENCY, LA9310_I2C_FREQ );
}
