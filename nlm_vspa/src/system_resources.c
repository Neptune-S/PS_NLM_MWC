#include <time.h>
#include "definitions.h"
#include "nr_pss.h"
#include "system_resources.h"
#include "all_drivers.h"

void decimator(void)
{
	cfixed16_t* ptrHist = decHist;		
	cfixed16_t* BUF_DEC = (cfixed16_t*)(ippu_buffer);	// main buffer for receiving samples from ADC, one batch @ 61.44 MSps
	pDcmOut_s1          = BUF_DEC;
	static uint32_t nco_phase  = 0;
	size_t nLineOut_dec = FFT_SIZE / SIZEVEC32; 
    
    /* First decimation stage: 61.44 -> 30.72 */

	decim2x_padd_asm( pDcmOut_s1, BUF_DEC, BUF_DEC, pDcmOut_s1, ptrHist + 29,SAMPLE_PER_BATCH_61_44<<1, nLineOut_dec, SAMPLE_PER_BATCH_30_72<<1);
    

	if(process_iteration != 0)
		pDcmOut_s2 = (pDcmOut_s2 >= CBUF_PROC + (CBUF_SZ - B_SZ)) ? pDcmOut_s2 + B_SZ - CBUF_SZ   : pDcmOut_s2 + B_SZ;
	else
	{
		pDcmOut_s2 	= CBUF_PROC;
		nco_phase   = 0;
	}
	nco_phase = compensate_cfo_cplx(pDcmOut_s1, pDcmOut_s1, (cfixed16_t*)BUF_DEC, SAMPLE_PER_BATCH_61_44<<1, -2097152,nco_phase,4); // 2097152 = 15KHz shift to avoid DC

	// Second decimation stage: 30.72 -> 15.36
	ptrHist += 32;
	nLineOut_dec >>= 1;
	
	decim2x_padd_asm( pDcmOut_s2, pDcmOut_s1, pDcmOut_s1, pDcmOut_s2, ptrHist+29 , (FFT_SIZE)*2, nLineOut_dec, (B_SZ)*2  );
}

void iq_capture(uint32_t cmd, uint32_t src)
{
	dma_generic(cmd, src, 2*(uint32_t)&ippu_buffer[0], AXIQ_UNIT_TXFR_BYTES);
}

uint32_t read_phytmr()
{
	return (__ip_read((0x500+ 0x10) >> 2));
}


static inline void custom_axiq_enable_rx2(void)
{
	__ip_write(LA9310_GPO_AXIQ_CR0, AXIQ_CR_FIELD_FIFO_RX2,
			(AXIQ_CR_ENABLE | AXIQ_THRESH) << AXIQ_CR_SHIFT_FIFO_RX2);
}

static inline void custom_axiq_disable_rx2(void)
{
	__ip_write(LA9310_GPO_AXIQ_CR0, AXIQ_CR_FIELD_FIFO_RX2, 0);
}

void enable_rx2(uint32_t channel_rx)
{	
	dma_ch_init(channel_rx); // abort and reset channel	
	custom_axiq_disable_rx2();
	RX_PTR_RESET; //force garbage
	dma_ch_busy_wait(channel_rx);
	__ip_write(0x590 >> 2 , 0x1 << 28 , 0x1 << 28);  // clear RX fifo overflow and underflow errors

	SYM_HALF_GET(load_cmd, 0, dma_load_addr);
	SYM_HALF_GET(load_cmd, 1, dma_load_addr); // will pend when exiting function
	custom_axiq_enable_rx2();	
	dma_ch_busy_wait(channel_rx);
}

// -----------------------------------------------------------------------------
//! @brief          System setup function.
//! @return         This function does not return.
// -----------------------------------------------------------------------------
void setup(void)
{
	SW_VERSION(V_MAJOR, V_MINOR);
	mbox0_send(0xF1000000, 0x0); // VSPA is up

	/* Enable MAILBOX 0*/
	do { mailbox_status = 0x4 & get_mbox_status(); } while (mailbox_status == 0);
	mbox0_get(&mboxMSB, &mboxLSB);
	
	if (mboxMSB == 0x70000000 && mboxLSB == 0x0)
	{
		mbox0_send(0xF0700000, 0x0);
	}
	/* Clear all DMA statuses */
	DMA_CLEAR_STATUS(0xffffffff);
	DMA_CLEAR_GO_STATUS(0xffffffff);
	DMA_ABORT_CHANNEL(0xffffffff);
	DMA_CLEAR_CFGERR(0xffffffff);

	/* Set RX always on */
	phy_tmr_configure();

	/* Enable MAILBOX VSPA-GO */
	__ip_write(0x8 >> 2, 0x03000001, 0x03000001); //CONTROL

	/* IPPU Argument address */
	//	__ip_write(IPPU_ARG_BASE_ADDR_ADDR, IPPU_ARG_BASE_ADDR_MASK, 0x1F80);

	/* Setup variable configurations */
#ifdef PERFORMANCE
	cycleInx = 0;
	cycleMeasureStart();
#endif

}
