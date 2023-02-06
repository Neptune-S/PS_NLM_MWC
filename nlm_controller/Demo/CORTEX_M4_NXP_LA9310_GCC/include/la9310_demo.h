/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * Copyright 2017-2018, 2021 NXP
 * All rights reserved.
 */

#ifndef __LA9310_DEMO_H__
#define __LA9310_DEMO_H__

#include <la9310.h>
extern void vEvtIrqRaiseDemo( struct la9310_info * pLa9310Info );
extern void vLa9310DemoIrqEvtRegister( struct la9310_info * pLa9310Info );
extern void vLa9310EdmaDemo( uint32_t * pulEDMAInfo );
extern void vLa9310DemoIpcRawTest( bool latency_test );
extern void vLa9310AviDemo( void );
extern void vWdogDemo( struct la9310_info * pLa9310Info );
extern void vLa9310GpioTest( uint8_t iGpioNum,
                             uint8_t iDir,
                             uint8_t iHighLow );
extern void vLa9310I2CTest( uint8_t uI2C_Controller,
                            uint8_t rw,
                            uint8_t ucDev_Addr,
                            uint32_t ulDev_Offset,
                            uint8_t ulDev_Offset_len,
                            uint8_t uNum_Byte,
                            uint32_t uByteToWrite );

extern void vAVIDemo( uint32_t ulNumIteration );


/*****************************************************************************
 * @vGenerateExceptions
 *
 * Generates exceptions acoording to ulID provided.
 *
 * ulID - [IN][M]  Exception ID which need to be generated
 *
 ****************************************************************************/
extern void vGenerateExceptions( uint32_t ulID );

/*****************************************************************************
 * @vDspiTest
 *
 * Test DSPI peripheral devices
 *
 * ulTestID - [IN][M] DSPI peripheral identifier
 *            0 - LTC5586
 *            1 - LMX2582
 *            2 - ADRF6520
 *
 * ulMode - [IN][M] CS driving mode
 *            0 - Drive LMX2582 vs ADRF6520 cs using GPIO_12
 *            1 - Drive LMX2582 vs ADRF6520 cs using PA_EN (through phy timer)
 *
 ****************************************************************************/
void vDspiTest( uint32_t ulTestID, uint32_t ulMode );

#endif /* ifndef __LA9310_DEMO_H__ */
