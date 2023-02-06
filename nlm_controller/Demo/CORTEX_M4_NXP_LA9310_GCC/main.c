/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * Copyright 2017-2021 NXP
 * All rights reserved.
 */

#include "FreeRTOS.h"
#include "task.h"
#include "common.h"
#include "config.h"
#include "immap.h"
#include "la9310_main.h"
#include "la9310_irq.h"
#include "la9310_gpio.h"
#include "la9310_edmaAPI.h"
#include "la9310_i2cAPI.h"
#include "exceptions.h"
#include <la9310.h>
#include <fsl_dspi.h>
#include "la9310_pinmux.h"
#include "bbdev_ipc.h"
#include "la9310_dcs_api.h"
#include "nmm_app.h"
#include <phytimer.h>
#include <sync_timing_device.h>
#include <sync_timing_device_cli.h>
#include <sw_cmd_engine.h>
#include "la9310_v2h_if.h"
#include "UARTCommandConsole.h"
#include "rfic_api.h"
#include "rfic_dac.h"
#include "rfic_demod.h"
#include "semphr.h"
#include "cmd_ctrl_vspa_intfce.h"
#if NXP_ERRATUM_A_009410
#include "la9310_pci.h"
#endif
#include "la9310_avi.h"

#ifdef LA9310_CLOCK_SWITCH_ENABLE
static void *avihndl = NULL;
#endif
#include "pbch.h"

SemaphoreHandle_t xSemMIBTaskUnlock = NULL;
SemaphoreHandle_t xSemVSPABusy  = NULL;
TimerHandle_t xScheduler=NULL;
schedular_params_t* schedular_params;


void vRegisterSampleCLICommands(void);
uint32_t ulMemLogIndex;
struct la9310_info *pLa9310Info;
extern void vVSPAMboxInit();
uint32_t BootSource;

void v_main_Hif_Init(struct la9310_info *pLa9310Info)
{
    struct la9310_hif *pxHif = pLa9310Info->pHif;

    pxHif->hif_ver = LA9310_VER_MAKE(LA9310_HIF_MAJOR_VERSION, LA9310_HIF_MINOR_VERSION);
    log_dbg("%s: Initialized HIF - %d.%d", __func__, LA9310_HIF_MAJOR_VERSION,
            LA9310_HIF_MINOR_VERSION);

    pLa9310Info->stats = &pxHif->stats;
}

static int iCheckReadyState(struct la9310_info *pLa9310Info)
{
    int rc = HOST_NOT_READY;
    struct la9310_hif *pHif = pLa9310Info->pHif;

    log_info("Waiting for HOST ready 0x%x\n\r", pHif->host_ready);

    while (1)
    {
        dmb();

        if ((pHif->host_ready & LA9310_HOST_READY_MASK) ==
            LA9310_HOST_READY_MASK)
        {
            rc = SUCCESS;
            break;
        }
    }

    return rc;
}

static void prvInitLa9310Info(struct la9310_info *pLa9310Info)
{
    int i = 0;
    uint32_t uMSIAddrVal = 0;
    uint32_t __IO *pMsiAddrReg;
    uint32_t __IO *pMsiDataAddr;
    struct la9310_msi_info *pMsiInfo;

    pLa9310Info->itcm_addr = (void *)TCML_PHY_ADDR;
    pLa9310Info->dtcm_addr = (void *)TCMU_PHY_ADDR;
    pLa9310Info->pcie_addr = (void *)PCIE_BASE_ADDR;
    pLa9310Info->msg_unit = (struct la9310_msg_unit *)MSG_UNIT_BASE_ADDR;

    pLa9310Info->pcie_obound = (void *)PCIE_PHY_ADDR;
    pLa9310Info->pHif = (struct la9310_hif *)((uint32_t)pLa9310Info->itcm_addr +
                                              LA9310_EP_HIF_OFFSET);
    pLa9310Info->pxDcr = (void *)DCR_BASE_ADDR;

    /* Initialize MSI */
    pMsiAddrReg = (uint32_t *)((uint32_t)pLa9310Info->pcie_addr +
                               PCIE_MSI_ADDR_REG);

    pMsiDataAddr = (uint32_t *)((uint32_t)pLa9310Info->pcie_addr +
                                PCIE_MSI_DATA_REG_1);

    pMsiInfo = &pLa9310Info->msi_info[MSI_IRQ_MUX];

    /* According to PCI bus standerd multiple MSIs are allocated consecutively*/
    uMSIAddrVal = (IN_32(pMsiAddrReg) & 0x3FF);
#ifdef LS1046_HOST_MSI_RAISE
    /*This method to initialize MSI structure is non-standard and dedicated for
         * LS1046 host */

    for (i = 0; i < LA9310_MSI_MAX_CNT; i++)
    {
        pMsiInfo[i].addr = (LA9310_EP_TOHOST_MSI_PHY_ADDR | uMSIAddrVal);
        val = IN_32(pMsiDataAddr);
        pMsiInfo[i].data = ((val + i) << 2);
        log_info("%s: MSI init[%d], addr 0x%x, data 0x%x\n\r", __func__,
                 i, pMsiInfo[i].addr, pMsiInfo[i].data);
    }
#else  /* ifdef LS1046_HOST_MSI_RAISE */
    for (i = 0; i < LA9310_MSI_MAX_CNT; i++)
    {
        pMsiInfo[i].addr = (LA9310_EP_TOHOST_MSI_PHY_ADDR | uMSIAddrVal);
        pMsiInfo[i].data = (IN_32(pMsiDataAddr) + i);
        log_info("%s: MSI init[%d], addr 0x%x, data 0x%x\n\r", __func__,
                 i, pMsiInfo[i].addr, pMsiInfo[i].data);
    }
#endif /* ifdef LS1046_HOST_MSI_RAISE */
}

/* iLa9310HostPreInit: Host and La9310 both do a handshake for clock configuration
 * and init synchronization vLa9310_do_handshake(). Host waits in while loop for
 * La9310 to indicate Host that it can proceed with Initialization
 * (LA9310_HOST_START_DRIVER_INIT).
 *
 * So Do all the initialization that you want in ratller which your host peer
 * code needs for initialization. If your host code wants some values
 * initialized by la9310 for it's initialization, then add call your init
 * function here.
 */
int iLa9310HostPreInit(struct la9310_info *pLa9310Info)
{
    int irc = 0;

    v_main_Hif_Init(pLa9310Info);

    irc = iLa9310IRQInit(pLa9310Info);

    if (irc)
    {
        goto out;
    }

#ifdef LA9310_CLOCK_SWITCH_ENABLE
    vVSPAMboxInit();
#endif

out:
    return irc;
}

/* iLa9310HostPostInit: Host and La9310 both do a handshake for clock configuration
 * and init synchronization vLa9310_do_handshake(). Host waits in while loop for
 * La9310 to indicate Host that it can proceed with Initialization
 * (LA9310_HOST_START_DRIVER_INIT).
 *
 * If your module code is dependent on any initialization that has to be done by
 * your peer code on host then add your post handskake init code here.
 */
int iLa9310HostPostInit(struct la9310_info *pLa9310Info)
{
    int irc = 0;

    irc = iEdmaInit();

    if (irc == 0)
    {
        log_info("%s: eDMA init DONE\n\r", __func__);
    }

    return irc;
}

void iGpioInitRFIC(void)
{
    int iCnt = 0;

    for (iCnt = 6; iCnt <= 12; iCnt++)
    {
        vGpioSetPinMuxSingle(iCnt, SET_MUX_GPIO_MODE);
        iGpioInit(iCnt, output, false);
    }
}

void vInitMsgUnit(void)
{
    NVIC_SetPriority(IRQ_MSG3, 3);
    NVIC_EnableIRQ(IRQ_MSG3);
}

int iPcieBoot(void)
{
    int irc = 0;

    // #ifdef LA9310_CLOCK_SWITCH_ENABLE
    // static  void * avihndl = NULL;
    // #endif

    ulMemLogIndex = 0;

    iGpioInitRFIC();
    PRINTF("STARTING NLM.. Boot Source (PCIe) \n\r");
    vEnableExceptions();

    pLa9310Info = pvPortMalloc(sizeof(struct la9310_info));

    if (!pLa9310Info)
    {
        PRINTF("pLA9310info alloc failed. going for while(true)\n\r");
        irc = -pdFREERTOS_ERRNO_ENOMEM;
        goto out;
    }

    memset(pLa9310Info, 0, sizeof(struct la9310_info));

    if (sizeof(struct la9310_hif) > LA9310_EP_HIF_SIZE)
    {
        PRINTF("Invalid HIF size\r\n");
        irc = FAILURE;
        goto out;
    }

    /*XXX: DO NOT CALL log_*() before prvInitLa9310Info(), use PRINTF instead.*/
    prvInitLa9310Info(pLa9310Info);

    log_dbg("%s: Allocated pLa9310Info %#x\n\r\n", __func__, pLa9310Info);

    /* XXX:NOTE - Do all initialization that is required by Host driver to
     * function like IRQ MUX, IPC, DMA in iLa9310HostPreInit().
     */
    irc = iLa9310HostPreInit(pLa9310Info);

    if (irc)
    {
        log_err("%s: iLa9310HostPreInit Failed, rc %d\n\r", __func__, irc);
        goto out;
    }

    /*Till Here system is running at 100 Mhz*/
    vLa9310_do_handshake(pLa9310Info);

#if NXP_ERRATUM_A_009410
    vPCIEInterruptInit();
#endif

    vInitMsgUnit();

    if (lSwCmdEngineInit() != 0)
    {
        log_err("sw cmd engine init failed\r\n");
        irc = FAILURE;
        goto out;
    }

#ifdef __RFIC
    if (pdTRUE != iRficInit(pLa9310Info))
    {
        log_err("%s: RFIC Init Failed, rc %d\n\r", __func__, irc);
        irc = FAILURE;
        goto out;
    }
#endif

    irc = iLa9310HostPostInit(pLa9310Info);

    if (irc)
    {
        log_err("%s: iLa9310HostPostInit Failed, rc %d\n\r", __func__, irc);
        goto out;
    }

    if (bbdev_ipc_init(0, 0))
    {
        log_err("IPC Init failed\r\n");
        irc = FAILURE;
        goto out;
    }

    /* Till here Host should be ready */
    irc = iCheckReadyState(pLa9310Info);

    if (irc)
    {
        log_err("%s: iCheckReadyState Failed, rc %d\n\r", __func__, irc);
        goto out;
    }

    vPhyTimerReset();
    /* Run phy timer at PLAT_FREQ / 8 = ( 122.88 * 4 ) / 8 = 61.44MHz */
    vPhyTimerEnable(PHY_TMR_DIVISOR);

   
    /* Force RO1 always on */
    vPhyTimerComparatorForce(PHY_TIMER_COMP_R01, ePhyTimerComparatorOut1);

#ifdef LA9310_CLOCK_SWITCH_ENABLE
    /*VSPA AVI Init*/
    avihndl = iLa9310AviInit();

    if (NULL == avihndl)
    {
        log_err("ERR: %s: AVI Initialization Failed\n\r", __func__);
    }
#endif

#if __DCS
    vDcsInit(Half_Freq);
#endif

    pxSyncTimingDeviceInit();
    {
        SyncTimingDeviceContext_t *sync_device = pxSyncTimingDeviceGetContext();
        xSyncTimingDeviceFWAPIVariableOffsetDco(sync_device, 1, 0);
    }

    PRINTF("%s: pxSyncTimingDeviceInit Done\n\r", __func__);

    irc = SUCCESS;

    return irc;
out:
    return irc;
}

int iI2cBoot(void)
{
    PRINTF("STARTING NLM.. Boot Source (I2C) \n\r");

#ifdef LA9310_CLOCK_SWITCH_ENABLE
    vSocResetHandshake();
    dmb();
#endif

    /*Clock changes after handshake*/
    vBoardFinalInit();

    PRINTF("Handshake Completed \n\r");
    return SUCCESS;
}

/*
 *        La9310 Application Entry point
 */
int main(void)
{   
    int irc = 0;
    /* Initialize hardware */
    vHardwareEarlyInit();

    BootSource = ((IN_32((uint32_t *)DCR_BASE_ADDR)) >>
                  LX9310_BOOT_SRC_SHIFT) &
                 LX9310_BOOT_SRC_MASK;

    if (BootSource == LA9310_BOOT_SRC_PCIE)
    {
        irc = iPcieBoot();
    }
    else if (BootSource == LA9310_BOOT_SRC_I2C)
    {
        irc = iI2cBoot();
    }
    else
    {
        log_info("Invalid Boot Source \n\r");
        goto out;
    }

    if (irc)
    {
        log_info("irc value is %d \n\r", irc);
        goto out;
    }

    irc = vBBDEVSetup();
    if (irc)
    {
        log_info("%s: vBBDEVSetup failed, rc %d\n\r", __func__, irc);
    }

    vRegisterSampleCLICommands();
    vUARTConsoleStart(mainUART_CONSOLE_STACK_SIZE, mainUART_CONSOLE_TASK_PRIORITY);

    xSemMIBTaskUnlock = xSemaphoreCreateBinary();

    if (xSemMIBTaskUnlock == NULL)
        log_info("\n Unable to create MIB task sempahore\n\r");
    
    xSemVSPABusy = xSemaphoreCreateBinary();

    if (xSemVSPABusy == NULL)
        log_info("\n Unable to create VSPA busy sempahore\n\r");

    schedular_params=(schedular_params_t*)malloc(sizeof(schedular_params_t));
        
	if (schedular_params== NULL)
	{
		log_err("ERR: Schedular_params memory not allocated\n\r");
		return 0;
	}
	
    memset (schedular_params,0,sizeof(schedular_params_t));

	ssb_stats = (ssb_stats_t*)malloc(sizeof(ssb_stats_t));
        
	if (ssb_stats == NULL)
	{
		log_err("ERR: Ssb_stats memory not allocated\n\r");
		return 0;
	}
	
    memset (ssb_stats,0,sizeof(ssb_stats_t));

	mib_info = (mib_info_t*)malloc(sizeof(mib_info_t));
        
	if (mib_info == NULL)
	{
		log_err("ERR: Mib_info memory not allocated\n\r");
		return 0;
	}
	
    memset (mib_info,0,sizeof(mib_info_t));

	cell_acquistion_response = (cell_acquistion_response_t*)malloc(sizeof(cell_acquistion_response_t));
        
	if (cell_acquistion_response== NULL)
	{
		log_err("ERR: Cell_acquistion_response memory not allocated\n\r");
		return 0;
	}
	
    memset (cell_acquistion_response,0,sizeof(cell_acquistion_response_t));

    irc = xTaskCreate(vRxLoop, "NMM Rx Polling Loop", configMINIMAL_STACK_SIZE * 2,
                      NULL, tskIDLE_PRIORITY + 1, NULL);
    if (irc != pdPASS)
    {
        log_info("Failed to create Rx polling task, not starting scheduler\n\r");
        goto out;
    }

    irc = xTaskCreate(vVSPALoop, "NMM VSPA Polling Loop", configMINIMAL_STACK_SIZE * 2,
                      NULL, tskIDLE_PRIORITY + 2, NULL);
    if (irc != pdPASS)
    {
        log_info("Failed to create VSPA polling task, not starting scheduler\n\r");
        goto out;
    }

    irc = xTaskCreate(vMibLoop, "NMM Mib Processing Loop", configMINIMAL_STACK_SIZE * 2*4,
                      NULL, tskIDLE_PRIORITY + 2, NULL);
    if (irc != pdPASS)
    {
        log_info("Failed to create MIB processing task, not starting scheduler\n\r");
        goto out;
    }

    irc = xTaskCreate(vSchedularLoop, "NMM Scheduler Loop", configMINIMAL_STACK_SIZE * 2,
                      NULL, tskIDLE_PRIORITY + 3, NULL);
    if (irc != pdPASS)
    {
        log_info("Failed to create VSPA polling task, not starting scheduler\n\r");
        goto out;
    }

    /* Start FreeRTOS scheduler */
    vTaskStartScheduler();

out:
    log_err("%s: Something terrible has happend, rc %d\n\r", __func__, irc);
    log_err("%s: Going for infitite loop of death\n\r", __func__);

    /* Should never reach this point */
    while (true)
    {
    }
}

void *getAviHandle()
{
    return avihndl;
}
