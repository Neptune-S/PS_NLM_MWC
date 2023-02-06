/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * Copyright 2017, 2021 NXP
 * All rights reserved.
 */

#include <common.h>
#include "config.h"
#include "immap.h"
#include "soc.h"
#include "board.h"
#include <la9310.h>
#include "la9310_main.h"
#include "la9310_irq.h"
#include "la9310_pci.h"

void vLa9310_do_handshake( struct la9310_info * vLa9310Info )
{
    struct ccsr_dcr * pxDcr = ( struct ccsr_dcr * ) vLa9310Info->pxDcr;

    PRINTF( "%s: LA9310 -> Host: LA9310_HOST START_CLOCK_CONFIG\n\r", __func__ );
    OUT_32( &pxDcr->ulScratchrw[ 1 ], LA9310_HOST_START_CLOCK_CONFIG );
    dmb();

    while( IN_32( &pxDcr->ulScratchrw[ 1 ] ) !=
           LA9310_HOST_COMPLETE_CLOCK_CONFIG )
    {
    }

    PRINTF( "%s: Host -> LA9310: LA9310_HOST_COMPLETE_CLOCK_CONFIG\n\r", __func__ );
    dmb();

    #ifdef LA9310_CLOCK_SWITCH_ENABLE
        vSocResetHandshake();
        dmb();
    #endif

    /*Clock changes from 100 Mhz to 160Mhz after handshake*/
    vBoardFinalInit();

    PRINTF( "%s: LA9310 -> HOST: LA9310_HOST_START_DRIVER_INIT\n\r", __func__ );
    OUT_32( &pxDcr->ulScratchrw[ 1 ], LA9310_HOST_START_DRIVER_INIT );

    #ifndef LA9310_RESET_HANDSHAKE_POLLING_ENABLE
        vWaitForPCIeLinkStability();
        /*Raise Msi for Host handshaking*/
        vRaiseMsi( pLa9310Info, MSI_IRQ_HOST_HANDSHAKE );
    #endif
}

void vHardwareEarlyInit( void )
{
    #if NXP_ERRATUM_A_009531
        vPCIEIDOClear();
    #endif
    vSocInit();
    vBoardEarlyInit();
}
