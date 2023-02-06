#ifndef NR_PROC_ACCELERATION_H_
#define NR_PROC_ACCELERATION_H_

#define DATA_RE_BUFF_SZ  (448)	// 144*3 + 16 (to make multiple of 64) 
#define	DMRS_RE_BUFF_SZ  (192)    // 144   + 48 (to make multiple of 64) 

#define REF_DMRS_SZ     		(144*8*4)   // In bytes 
#define RX_DMRS_SZ     (144*4)   // In bytes 
#define PBCH_CH_EST_SZ     (432*4)   // In bytes 

void pbch_dmrs_extraction();
void pbch_ls_estimate();
void pbch_equalizer();

#endif /* NR_PROC_ACCELERATION_H_ */
