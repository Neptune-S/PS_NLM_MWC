// ===========================================================================
//! @file            ipreg_types.h
//! @copyright       Copyright (C) 2012-2013 Freescale Semiconductor, Inc.
//! @brief           DMA controller driver interface definition.
//!
//! The header ipreg_types.h defines macros and bitfields to describe
//! hardware registers from internal peripherals.
// ===========================================================================

#ifndef __IPREG_TYPES_H__
#define __IPREG_TYPES_H__

#ifndef __ASSEMBLER__
#include <stdbool.h>
#include <vspa/intrinsics.h>
#endif

//! @defgroup        GROUP_IPREG IP Registers
//! @addtogroup      GROUP_IPREG
//! @{

#define __IP_WRITE( reg, mask, data )   __ip_write( reg, mask, data )
#define __IP_READ( reg, mask )          __ip_read( reg, mask )


// ===========================================================================
// Defines common to all IP register use
// ===========================================================================

// ---------------------------------------------------------------------------
//! @brief 32-bit mask to read or write all 32 bits from an IP register.
// ---------------------------------------------------------------------------
#define IPMASK_ALL   0xFFFFFFFF

// ===========================================================================
// Defines and typedefs for each IP register
// ===========================================================================

// ---------------------------------------------------------------------------
// HWVERSION register defines and bitfield typedef
// ---------------------------------------------------------------------------
#define HWVERSION_ADDR  0x0                  //!< Hardware version register address.
#define HWVERSION_MASK  0xFFFFFFFF           //!< Hardware version register bitmask.

#ifndef __ASSEMBLER__
typedef union HWVERSION_TYPE {
   int value;                                //!< 32-bit hardware version register value.
   struct{
      unsigned int vspa_hw_version:32;       //!< 32-bit hardware version value.
   } field;                                  //!< Hardware register bitfields.
} HWVERSION_TYPE;                            //!< Hardware version register type.
#endif

// ---------------------------------------------------------------------------
// SWVERSION register defines and bitfield typedef
// ---------------------------------------------------------------------------
#define SWVERSION_ADDR  0x1                  //!< Software version register address.
#define SWVERSION_MASK  0xFFFFFFFF           //!< Software version register bitmask.

#ifndef __ASSEMBLER__
typedef union SWVERSION_TYPE {
   int value;                                //!< 32-bit software version register value.
   struct{
      unsigned int prom_ucode_version:16;    //!< PROM microcode version value.
      unsigned int pram_ucode_version:16;    //!< PRAM microcode version value.
   } field;                                  //!< Software version register bitfields.
} SWVERSION_TYPE;                            //!< Software version register type.
#endif

// ---------------------------------------------------------------------------
// CONTROL register defines and bitfield typedef
// ---------------------------------------------------------------------------
#define CONTROL_ADDR                	0x2           	//!< VCPU control register address.
#define CONTROL_MASK                	0x8FF31BFF     	//!< VCPU control register - all valid bits bitmask.
#define CONTROL_MASK_RD_ONLY            0x0000010C      //!< VCPU control register - all read only bits bitmask.
#define CONTROL_MASK_WR                 0x8FF31AF3      //!< VCPU control register - all writable bits bitmask.
#define CONTROL_MASK_GO                 0x00F001FF      //!< All GO bitmask.

#define CONTROL_CLEAR_ALL_GO            0x00F001EF      //!< Used to clear all GO bits (bit4 is NOT w1c like others)
#define CONTROL_RESET_ZERO              0x00F001EF      //!< Used to reset CONTROL=0x0

#define CONTROL_MASK_HOST_GO        	(0x1U << 0)    	//!< Host GO bitmask (w1c)
#define CONTROL_MASK_IPPU_GO        	(0x1U << 1)    	//!< IPPU GO bitmask (w1c)
#define CONTROL_MASK_DMA_GO         	(0x1U << 2)    	//!< DMA GO bitmask (READ ONLY)
#define CONTROL_MASK_EXT_GO         	(0x1U << 3)		//!< External GO bitmask (READ ONLY)
#define CONTROL_MASK_VCPU_GO            (0x1U << 4)     //!< VCPU GO bitmask (write 0 to clear!)
#define CONTROL_MASK_DEBUG_MSG_GO   	(0x1U << 5)    	//!< Debug message GO bitmask (w1c)
#define CONTROL_MASK_HOST_VCPU_FLAGS_GO (0x1U << 6) 	//!< Host VCPU Flags GO bitmask (w1c)
#define CONTROL_MASK_FECU_GO            (0x1U << 7)     //!< FECU Go bitmask (w1c)
#define CONTROL_MASK_AXI_SLAVE_GO       (0x1U << 8)     //!< AXI Slave Go bitmask (READ ONLY)

#define CONTROL_DEBUG_MSG_GO_EN         (0x1U << 9)     //!< Enable debug message GO bitmask.
#define CONTROL_HOST_VCPU_FLAGS0_GO_EN  (0x1U << 11)    //!< Enable Host VCPU Flags0 GO bitmask.
#define CONTROL_HOST_VCPU_FLAGS1_GO_EN  (0x1U << 12)    //!< Enable Host VCPU Flags1 GO bitmask.

#define CONTROL_MASK_HOST_SENT_MSG0_GO 	(0x1U << 20)	//!< Host Sent Msg0 GO bitmask.
#define CONTROL_MASK_HOST_SENT_MSG1_GO 	(0x1U << 21)	//!< Host Sent Msg1 GO bitmask.

#define CONTROL_MASK_HOST_SENT_MSG_GO   (   CONTROL_MASK_HOST_SENT_MSG0_GO \
                                        |   CONTROL_MASK_HOST_SENT_MSG1_GO)

#define CONTROL_MASK_HOST_READ_MSG0_GO 	(0x1U << 22)	//!< Host Read Msg0 GO bitmask.
#define CONTROL_MASK_HOST_READ_MSG1_GO 	(0x1U << 23)	//!< Host Read Msg1 GO bitmask.

#define CONTROL_MASK_HOST_READ_MSG_GO   (   CONTROL_MASK_HOST_READ_MSG0_GO \
                                        |   CONTROL_MASK_HOST_READ_MSG1_GO)

#define CONTROL_HOST_SENT_MSG0_GO_EN    (0x1U << 24)    //!< Enable Host Sent Msg0 GO bitmask.
#define CONTROL_HOST_SENT_MSG1_GO_EN    (0x1U << 25)    //!< Enable Host Sent Msg1 GO bitmask.
#define CONTROL_HOST_READ_MSG0_GO_EN    (0x1U << 26)    //!< Enable Host Read Msg0 GO bitmask.
#define CONTROL_HOST_READ_MSG1_GO_EN    (0x1U << 27)    //!< Enable Host Read Msg1 GO bitmask.



#ifndef __ASSEMBLER__
typedef union CONTROL_TYPE {
   int value;                                   //!< 32-bit VCPU control register value.
   struct{
      unsigned int host_go:1;                   //!< bit0 = Host GO bitfield.
      unsigned int ippu_go:1;                   //!< bit1 = IPPU GO bitfield.
      unsigned int dma_go:1;                    //!< bit2 = DMA GO bitfield.
      unsigned int ext_go:1;                    //!< bit3 = External GO bitfield.
      unsigned int vcpu__go:1;                  //!< bit4 = VCPU GO ("SW int" for user/supervisor mode stuff)
      unsigned int debug_msg_go:1;              //!< bit5 = Debug message GO bitfield.
      unsigned int host_vcpu_flags_go:1;        //!< bit6 = Host VCPU Flags GO bitfield.
      unsigned int fecu_go:1;                   //!< bit7 = FECU GO bitfield.
      unsigned int axislv_go:1;                 //!< bit8 = AXI Slabe GO bitfield.
      unsigned int debug_msg_go_en:1;           //!< bit9 = Debug message GO enable bitfield.
      unsigned int _reserved0:1;                //!< bit10 = Reserved.
      unsigned int host_vcpu_flags0_go_en:1;    //!< bit11 = Host VCPU Flags0 GO enable bitfield.
      unsigned int host_vcpu_flags1_go_en:1;	//!< bit12 = Host VCPU Flags1 GO enable bitfield.
      unsigned int _reserved1:3;		        //!< bits 15:13 = Reserved.
      unsigned int vcpu_reset:1;				//!< bit16 = VCPU reset bitfield.
      unsigned int dma_halt_req:1;				//!< bit17 = DMA halt request bitfield.
      unsigned int _reserved2:2;                //!< bits 19:18 = Reserved.
      unsigned int host_sent_msg0_go:1;         //!< bit20 = Host Sent Msg0 GO bitfield.
      unsigned int host_sent_msg1_go:1;         //!< bit21 = Host Sent Msg1 GO bitfield.
      unsigned int host_read_msg0_go:1;         //!< bit22 = Host Read Msg0 GO bitfield.
      unsigned int host_read_msg1_go:1;         //!< bit23 = Host Read Msg1 GO bitfield.
      unsigned int host_sent_msg0_go_en:1;      //!< bit24 = Host Sent Msg0 GO enable bitfield.
      unsigned int host_sent_msg1_go_en:1;      //!< bit25 = Host Sent Msg1 GO enable bitfield.
      unsigned int host_read_msg0_go_en:1;      //!< bit26 = Host Read Msg0 GO enable bitfield.
      unsigned int host_read_msg1_go_en:1;      //!< bit27 = Host Read Msg1 GO enable bitfield.
      unsigned int _reserved3:3;                //!< bits 30:28 = Reserved.
      unsigned int pdn_en:1;                    //!< bit31 = Power down enable bitfield.
   } field;                                     //!< VCPU control register bitfields.
} CONTROL_TYPE;                                 //!< VCPU control register type.

static inline unsigned int ipregControlRead(
   unsigned int const mask )
{
   return __IP_READ( CONTROL_ADDR, mask );
}

static inline bool ipregControlIsHostRequest(
   unsigned int flag )
{
   return 0 != (flag & CONTROL_MASK_HOST_GO);
}

static inline bool ipregControlIsIppuThread(
   unsigned int flag )
{
   return 0 != (flag & CONTROL_MASK_IPPU_GO);
}

static inline bool ipregControlIsDmaChannel(
   unsigned int flag )
{
   return 0 != (flag & CONTROL_MASK_DMA_GO);
}

static inline bool ipregControlIsExtRequest(
   unsigned int flag )
{
   return 0 != (flag & CONTROL_MASK_EXT_GO);
}

static inline bool ipregControlIsDebugCommand(
   unsigned int flag )
{
   return 0 != (flag & CONTROL_MASK_DEBUG_MSG_GO);
}

static inline void ipregControlClearHostRequest(
   void )
{
   __IP_WRITE( CONTROL_ADDR, CONTROL_MASK_HOST_GO, CONTROL_MASK_HOST_GO );
}

static inline void ipregControlClearIppuThread(
   void )
{
   __IP_WRITE( CONTROL_ADDR, CONTROL_MASK_IPPU_GO, CONTROL_MASK_IPPU_GO );
}

static inline void ipregControlClearExtRequest(
   void )
{
   __IP_WRITE( CONTROL_ADDR, CONTROL_MASK_EXT_GO, CONTROL_MASK_EXT_GO );
}

static inline void ipregControlClearDebugCommand(
   void )
{
   __IP_WRITE( CONTROL_ADDR, CONTROL_MASK_DEBUG_MSG_GO, CONTROL_MASK_DEBUG_MSG_GO );
}

#endif

// ---------------------------------------------------------------------------
// IRQ_EN register defines
// ---------------------------------------------------------------------------
#define IRQEN_ADDR            		0x3           	//!< IRQ enable register address.
#define IRQEN_MASK            		0x0000F0FF     	//!< IRQ enable register bitmask.

#define IRQEN_MASK_DONE       		(0x1 << 0)     	//!< VCPU Done interrupt enable bitmask.
#define IRQEN_MASK_IPPU_DONE  		(0x1 << 1)     	//!< Reordering Unit Done interrupt enable bitmask.
#define IRQEN_MASK_FLAGS0     		(0x1 << 2)     	//!< Flags 0 interrupt enable bitmask.
#define IRQEN_MASK_FLAGS1     		(0x1 << 3)     	//!< Flags 1 interrupt enable bitmask.
#define IRQEN_MASK_DMA_CMP    		(0x1 << 4)     	//!< DMA Complete interrupt enable bitmask.
#define IRQEN_MASK_DMA_ERROR  		(0x1 << 5)     	//!< DMA Error interrupt enable bitmask.
#define IRQEN_MASK_FECU_DONE        (0x1 << 6)      //!< FECU Done interrupt enable bitmask.
#define IRQEN_MASK_VCPU_IIT			(0x1 << 7)     	//!< VCPU Illegal Instruction Trap interrupt enable bitmask.
#define IRQEN_MASK_VCPU_SENT_MSG0	(0x1 << 12)     //!< VCPU Sent Msg0 to Host interrupt enable bitmask.
#define IRQEN_MASK_VCPU_SENT_MSG1	(0x1 << 13)     //!< VCPU Sent Msg1 to Host interrupt enable bitmask.
#define IRQEN_MASK_VCPU_READ_MSG0	(0x1 << 14)     //!< VCPU Read Msg0 from Host interrupt enable bitmask.
#define IRQEN_MASK_VCPU_READ_MSG1	(0x1 << 15)		//!< VCPU Read Msg1 from Host interrupt enable bitmask.

// ---------------------------------------------------------------------------
// STATUS register defines
// ---------------------------------------------------------------------------
#define STATUS_ADDR                    0x4               //!< VCPU Status register address.
#define STATUS_MASK                    0xC002F1FF        //!< VCPU Status register bitmask.

#define STATUS_MASK_DONE               (0x1 << 0 )       //!< Done bitmask.
#define STATUS_MASK_IRQ_PEND_IPPU_DONE (0x1 << 1 )       //!< IRQ Pending Reordering Unit Done bitmask.
#define STATUS_MASK_IRQ_PEND_FLAGS0    (0x1 << 2 )       //!< IRQ Pending VCPU_HOST_FLAGS0 bitmask.
#define STATUS_MASK_IRQ_PEND_FLAGS1    (0x1 << 3 )       //!< IRQ Pending VCPU_HOST_FLAGS1 bitmask.
#define STATUS_MASK_IRQ_PEND_DMA_COMP  (0x1 << 4 )       //!< IRQ Pending DMA Complete bitmask.
#define STATUS_MASK_IRQ_PEND_DMA_ERROR (0x1 << 5 )       //!< IRQ Pending DMA Error bitmask.
#define STATUS_MASK_IRQ_PEND_FECU_DONE (0x1 << 6 )       //!< IRQ Pending FECU Done bitmask.
#define STATUS_MASK_IIT_DETECT		   (0x1 << 7 )       //!< Illegal Instruction Trap detected bitmask.
#define STATUS_MASK_BUSY               (0x1 << 8 )       //!< VCPU Busy bitmask.
#define STATUS_MASK_VCPU_SENT_MSG0	   (0x1 << 12)       //!< VCPU Sent Msg0 to Host bitmask.
#define STATUS_MASK_VCPU_SENT_MSG1	   (0x1 << 13)       //!< VCPU Sent Msg1 to Host bitmask.
#define STATUS_MASK_VCPU_READ_MSG0	   (0x1 << 14)       //!< VCPU Read Msg0 from Host bitmask.
#define STATUS_MASK_VCPU_READ_MSG1	   (0x1 << 15)       //!< VCPU Read Msg1 from Host bitmask.
#define STATUS_MASK_DMA_HALT_ACK       (0x1 << 17)       //!< DMA Halt ACK bitmask.
#define STATUS_MASK_PDN_DONE           (0x1 << 30)       //!< Power Down Complete bitmask.
#define STATUS_MASK_PDN_ACTIVE         (0x1 << 31)       //!< Power Down Active bitmask.

// ---------------------------------------------------------------------------
// Macro to be used for both VCPU_HOST_FLAGS0 and VCPU_HOST_FLAGS1:
// ---------------------------------------------------------------------------

//! @brief        VCPU to Host flag bitmask macro.
//! @param[in]    x  Specifies the flag number.
//! @return       The bitmask for flag @a x.
#define VCPU_HOST_FLAGS_MASK_FLAG( x ) (0x1 << (x))

// ---------------------------------------------------------------------------
// VCPU -> HOST FLAGS0 register defines
// ---------------------------------------------------------------------------
#define VCPU_TO_HOST_FLAGS0_ADDR	0x5            //!< VCPU to Host Flags 0 register address.
#define VCPU_TO_HOST_FLAGS0_MASK  	0xFFFFFFFF     //!< VCPU to Host Flags 0 register bitmask.

// ---------------------------------------------------------------------------
// VCPU -> HOST FLAGS1 register defines
// ---------------------------------------------------------------------------
#define VCPU_TO_HOST_FLAGS1_ADDR  	0x6            //!< VCPU to Host Flags 1 register address.
#define VCPU_TO_HOST_FLAGS1_MASK  	0xFFFFFFFF     //!< VCPU to Host Flags 1 register bitmask.

// ---------------------------------------------------------------------------
// HOST -> VCPU FLAGS0 register defines
// ---------------------------------------------------------------------------
#define HOST_TO_VCPU_FLAGS0_ADDR	0x7            //!< Host to VCPU Flags 0 register address.
#define HOST_TO_VCPU_FLAGS0_MASK  	0xFFFFFFFF     //!< Host to VCPU Flags 0 register bitmask.

// ---------------------------------------------------------------------------
// HOST -> VCPU FLAGS1 register defines
// ---------------------------------------------------------------------------
#define HOST_TO_VCPU_FLAGS1_ADDR  	0x8            //!< Host to VCPU Flags 1 register address.
#define HOST_TO_VCPU_FLAGS1_MASK  	0xFFFFFFFF     //!< Host to VCPU Flags 1 register bitmask.

// ---------------------------------------------------------------------------
// EXT_GO_ENA register defines
// ---------------------------------------------------------------------------
#define EXT_GO_ENA_ADDR    0xA               //!< External GO enable register address.
#define EXT_GO_ENA_MASK    0xFFFFFFFF        //!< External GO enable register bitmask.

// ---------------------------------------------------------------------------
// EXT_GO_STAT register defines
// ---------------------------------------------------------------------------
#define EXT_GO_STAT_ADDR   0xB               //!< External GO status register address.
#define EXT_GO_STAT_MASK   0xFFFFFFFF        //!< External GO status register bitmask.

// ---------------------------------------------------------------------------
// Macro to be used with EXT_GO_ENA and EXT_GO_STAT
// ---------------------------------------------------------------------------
//! @brief        External GO bitmask macro.
//! @param[in]    x  Specifies the external GO bit.
//! @return       The external GO bitmask.
#define EXT_GO_MASK( x )   (0x1 << (x))

// ---------------------------------------------------------------------------
// VCPU Illegal Opcode Address register defines
// ---------------------------------------------------------------------------
#define VCPU_ILLOP_STAT_ADDR	0xC			//!< VCPU Illegal Opcode Address register address.
#define VCPU_ILLOP_STAT_MASK	0x01FFFFFF	//!< VCPU Illegal Opcode Address register bitmask.

// ---------------------------------------------------------------------------
// PARAM0 register defines
// ---------------------------------------------------------------------------
#define PARAM0_ADDR  			0x10			//!< VSPA Parameters 0 register address.
#define PARAM0_MASK  			0xCFF6F07F      //!< VSPA Parameters 0 register bitmask.

#define PARAM0_MASK_ATID 		(0x7F  << 0)	//!< VSPA ID Number (ATID) bitmask.
#define PARAM0_MASK_ATAN_SUPPORT (0x1  << 12)	//!< Arctangent support bitmask.
#define PARAM0_MASK_RCP_SUPPORT (0x1  << 13)	//!< Reciprocal support bitmask.
#define PARAM0_MASK_RRT_SUPPORT (0x1  << 14)	//!< Reciprocal Square Root support bitmask.
#define PARAM0_MASK_SRT_SUPPORT (0x1  << 15)	//!< Square Root support bitmask.
#define PARAM0_MASK_NCO_SUPPORT (0x1  << 17)	//!< NCO support bitmask.
#define PARAM0_MASK_DMA_DEINT	(0x1  << 18)	//!< DMA De-Interleaving support bitmask.
#define PARAM0_MASK_LUT_CNT		(0xFF << 20)	//!< Look-Up Table (LUT) Count bitmask.
#define PARAM0_MASK_VCPU_ISO	(0x1  << 30)	//!< VCPU Isolation Status bitmask.
#define PARAM0_MASK_IPPU_ISO	(0x1  << 31)	//!< IPPU Isolation Status bitmask.

// ---------------------------------------------------------------------------
// PARAM1 register defines
// ---------------------------------------------------------------------------
#define PARAM1_ADDR           0x11           //!< VSPA Parameters 1 register address.
#define PARAM1_MASK           0xFFFFFFFF     //!< VSPA Parameters 1 register bitmask.

#define PARAM1_MASK_GP_IN     (0xFF << 0 )   //!< GP_IN Register Count bitmask.
#define PARAM1_MASK_GP_OUT    (0xFF << 8 )   //!< GP_OUT Register Count bitmask.
#define PARAM1_MASK_DMA_CNT   (0xFF << 16)   //!< DMA Channel Count bitmask.
#define PARAM1_MASK_AXI_ID    (0x0F << 24)   //!< AXI ID Width bitmask.
#define PARAM1_MASK_AXI_DATA  (0x07 << 28)   //!< AXI Data Width bitmask.
#define PARAM1_MASK_RSSE      (0x01 << 31)   //!< RSSE Module Present bitmask.

// ---------------------------------------------------------------------------
// PARAM2 register defines
// ---------------------------------------------------------------------------
#define PARAM2_ADDR              0x12           //!< VSPA Parameters 2 register address.
#define PARAM2_MASK              0x9FC3FFFF     //!< VSPA Parameters 2 register bitmask.

#define PARAM2_MASK_NAU          (0x0FF << 0 )  //!< Number of Arithmetic Units bitmask.
#define PARAM2_MASK_DMEM_SIZE    (0x3FF << 8 )  //!< VSPA DMEM Size bitmask.
#define PARAM2_MASK_HPFLOAT 	 (0x001 << 22)  //!< Half Float (16bit) Support bitmask.
#define PARAM2_MASK_FLOAT64      (0x001 << 23)  //!< Double Float (64bit) Support bitmask.
#define PARAM2_MASK_EXT_GO_WIDTH (0x01F << 24)  //!< External GO Width bitmask.
#define PARAM2_MASK_IPPU_PRESENT (0x001 << 31)  //!< Reordering Unit Present bitmask.

// ---------------------------------------------------------------------------
// DMEM Bytes register defines
// ---------------------------------------------------------------------------
#define DMEM_BYTES_ADDR			0x13		//!< DMEM Bytes register address.
#define DMEM_BYTES_MASK			0x000FFFFF	//!< DMEM Bytes register mask

// ---------------------------------------------------------------------------
// Thread Control & Status Register
// ---------------------------------------------------------------------------
#define THREAD_CTL_STAT_ADDR    0x14

// ---------------------------------------------------------------------------
// Protection Fault Status Register
// ---------------------------------------------------------------------------
#define PROT_FAULT_STAT_ADDR    0x15

// ---------------------------------------------------------------------------
// Exception Control Register
// ---------------------------------------------------------------------------
#define EXCEPTION_CTL_ADDR      0x16

// ---------------------------------------------------------------------------
// Exception Status Register
// ---------------------------------------------------------------------------
#define EXCEPTION_STAT_ADDR     0x17

// ---------------------------------------------------------------------------
// AXI Slave Flags 0 Register
// ---------------------------------------------------------------------------
#define AXI_SLAVE_FLAGS0_ADDR   0x18

// ---------------------------------------------------------------------------
// AXI Slave Flags 1 Register
// ---------------------------------------------------------------------------
#define AXI_SLAVE_FLAGS1_ADDR   0x19

// ---------------------------------------------------------------------------
// AXI Slave Go Enable 0 Register
// ---------------------------------------------------------------------------
#define AXI_SLAVE_GO_EN0_ADDR   0x1a

// ---------------------------------------------------------------------------
// AXI Slave Go Enable 1 Register
// ---------------------------------------------------------------------------
#define AXI_SLAVE_GO_EN1_ADDR   0x1b

// ---------------------------------------------------------------------------
// Platform Input Register
// ---------------------------------------------------------------------------
#define PLAT_IN_ADDR            0x1c

// ---------------------------------------------------------------------------
// Platform Input Register
// ---------------------------------------------------------------------------
#define PLAT_OUT_ADDR           0x1d

// ---------------------------------------------------------------------------
// Cycle Counter MSB register defines
// ---------------------------------------------------------------------------
#define CYCLE_CNT_MSB_ADDR		0x26		//!< Cycle Counter MSB register address.
#define CYCLE_CNT_MSB_MASK		0x0000FFFF	//!< Cycle Counter MSB register mask.

#define CYCLE_CNT_MASK_EN       (0x1 << 31) //!< Cycle Counter Enable bitmask.

// ---------------------------------------------------------------------------
// Cycle Counter LSB register defines
// ---------------------------------------------------------------------------
#define CYCLE_CNT_LSB_ADDR		0x27		//!< Cycle Counter LSB register address.
#define CYCLE_CNT_LSB_MASK		0xFFFFFFFF	//!< Cycle Counter LSB register mask.

// ===========================================================================
// Defines useful for multiple DMA control registers
// ===========================================================================
#define DMA_CHAN_0   (0x1U << 0 )            //!< DMA channel 0  bitmask.
#define DMA_CHAN_1   (0x1U << 1 )            //!< DMA channel 1  bitmask.
#define DMA_CHAN_2   (0x1U << 2 )            //!< DMA channel 2  bitmask.
#define DMA_CHAN_3   (0x1U << 3 )            //!< DMA channel 3  bitmask.
#define DMA_CHAN_4   (0x1U << 4 )            //!< DMA channel 4  bitmask.
#define DMA_CHAN_5   (0x1U << 5 )            //!< DMA channel 5  bitmask.
#define DMA_CHAN_6   (0x1U << 6 )            //!< DMA channel 6  bitmask.
#define DMA_CHAN_7   (0x1U << 7 )            //!< DMA channel 7  bitmask.
#define DMA_CHAN_8   (0x1U << 8 )            //!< DMA channel 8  bitmask.
#define DMA_CHAN_9   (0x1U << 9 )            //!< DMA channel 9  bitmask.
#define DMA_CHAN_10  (0x1U << 10)            //!< DMA channel 10 bitmask.
#define DMA_CHAN_11  (0x1U << 11)            //!< DMA channel 11 bitmask.
#define DMA_CHAN_12  (0x1U << 12)            //!< DMA channel 12 bitmask.
#define DMA_CHAN_13  (0x1U << 13)            //!< DMA channel 13 bitmask.
#define DMA_CHAN_14  (0x1U << 14)            //!< DMA channel 14 bitmask.
#define DMA_CHAN_15  (0x1U << 15)            //!< DMA channel 15 bitmask.
#define DMA_CHAN_16  (0x1U << 16)            //!< DMA channel 16 bitmask.
#define DMA_CHAN_17  (0x1U << 17)            //!< DMA channel 17 bitmask.
#define DMA_CHAN_18  (0x1U << 18)            //!< DMA channel 18 bitmask.
#define DMA_CHAN_19  (0x1U << 19)            //!< DMA channel 19 bitmask.
#define DMA_CHAN_20  (0x1U << 20)            //!< DMA channel 20 bitmask.
#define DMA_CHAN_21  (0x1U << 21)            //!< DMA channel 21 bitmask.
#define DMA_CHAN_22  (0x1U << 22)            //!< DMA channel 22 bitmask.
#define DMA_CHAN_23  (0x1U << 23)            //!< DMA channel 23 bitmask.
#define DMA_CHAN_24  (0x1U << 24)            //!< DMA channel 24 bitmask.
#define DMA_CHAN_25  (0x1U << 25)            //!< DMA channel 25 bitmask.
#define DMA_CHAN_26  (0x1U << 26)            //!< DMA channel 26 bitmask.
#define DMA_CHAN_27  (0x1U << 27)            //!< DMA channel 27 bitmask.
#define DMA_CHAN_28  (0x1U << 28)            //!< DMA channel 28 bitmask.
#define DMA_CHAN_29  (0x1U << 29)            //!< DMA channel 29 bitmask.
#define DMA_CHAN_30  (0x1U << 30)            //!< DMA channel 30 bitmask.
#define DMA_CHAN_31  (0x1U << 31)            //!< DMA channel 31 bitmask.

// ---------------------------------------------------------------------------
// DMA_DMEM_PRAM_ADDR register defines and bitfield typedef
// ---------------------------------------------------------------------------
#define DMA_DMEM_PRAM_ADDR_ADDR 0x2C
#define DMA_DMEM_PRAM_ADDR_MASK 0x00FFFFFF

#ifndef __ASSEMBLER__
typedef union DMA_DMEM_PRAM_ADDR_TYPE {
   int value;
   struct{
      unsigned int starting_address:24;
      unsigned int rsvd:8;
   } field;
} DMA_DMEM_PRAM_ADDR_TYPE;
#endif

// ---------------------------------------------------------------------------
// DMA_AXI_ADDRESS register defines and bitfield typedef
// ---------------------------------------------------------------------------
#define DMA_AXI_ADDRESS_ADDR    0x2D
#define DMA_AXI_ADDRESS_MASK    0xFFFFFFFF

#ifndef __ASSEMBLER__
typedef union DMA_AXI_ADDRESS_TYPE {
   int value;
   struct{
      unsigned int starting_address:32;
   } field;
} DMA_AXI_ADDRESS_TYPE;
#endif

// ---------------------------------------------------------------------------
// DMA_AXI_BYTE_CNT register defines and bitfield typedef
// ---------------------------------------------------------------------------
#define DMA_AXI_BYTE_CNT_ADDR   0x2E
#define DMA_AXI_BYTE_CNT_MASK   0x0000FFFF

#ifndef __ASSEMBLER__
typedef union DMA_AXI_BYTE_CNT_TYPE {
   int value;
   struct{
      unsigned int count:16;
      unsigned int rsvd:16;
   } field;
} DMA_AXI_BYTE_CNT_TYPE;
#endif

// ---------------------------------------------------------------------------
// DMA_XFR_CTL register defines and bitfield typedef
// ---------------------------------------------------------------------------
#define DMA_XFR_CTL_ADDR        0x2F
#define DMA_XFR_CTL_MASK        0xFFCFFF9F

#define DMA_TRANSMODE_AXI2DMEM_NO_CONVERT 0
#define DMA_TRANSMODE_AXI2DMEM_CONVERT    1
#define DMA_TRANSMODE_AXI2PRAM            2
#define DMA_TRANSMODE_AXI2IPRAM           3
#define DMA_TRANSMODE_DMEM2AXI_DEINT      4
#define DMA_TRANSMODE_DMEM2AXI_NO_CONVERT 6
#define DMA_TRANSMODE_DMEM2AXI_CONVERT    7

#define DMA_BURST_TYPE_INCREMENTING       0
#define DMA_BURST_TYPE_FIFO               1

#define DMA_XFR_CTL_MASK_CHANNEL_SELECT   0x0000001F
#define DMA_XFR_CTL_MASK_MULTI_BURST      0x00000080
#define DMA_XFR_CTL_MASK_TRANS_MODE       0x00000700
#define DMA_XFR_CTL_MASK_BURST_TYPE       0x00000800
#define DMA_XFR_CTL_MASK_IPPU_GO_EN       0x00001000
#define DMA_XFR_CTL_MASK_VCPU_GO_EN       0x00002000
#define DMA_XFR_CTL_MASK_IRQ_EN           0x00004000
#define DMA_XFR_CTL_MASK_PTR_RST          0x00008000
#define DMA_XFR_CTL_MASK_EXT_TRIG         0x00010000
#define DMA_XFR_CTL_MASK_IPPU_TRIG        0x00020000
#define DMA_XFR_CTL_MASK_DI_MODE          0x001C0000
#define DMA_XFR_CTL_MASK_FECU_GO_EN       0x00400000
#define DMA_XFR_CTL_MASK_FECU_TRIG        0x00800000
#define DMA_XFR_CTL_MASK_SIDEBAND         0xFF000000

#define DMA_XFR_CTL_LSB_CHANNEL_SELECT    0
#define DMA_XFR_CTL_LSB_MULTI_BURST       7
#define DMA_XFR_CTL_LSB_TRANS_MODE        8
#define DMA_XFR_CTL_LSB_BURST_TYPE        11
#define DMA_XFR_CTL_LSB_IPPU_GO_EN        12
#define DMA_XFR_CTL_LSB_VCPU_GO_EN        13
#define DMA_XFR_CTL_LSB_IRQ_EN            14
#define DMA_XFR_CTL_LSB_PTR_RST           15
#define DMA_XFR_CTL_LSB_EXT_TRIG          16
#define DMA_XFR_CTL_LSB_IPPU_TRIG         17
#define DMA_XFR_CTL_LSB_DI_MODE           18
#define DMA_XFR_CTL_LSB_FECU_GO_EN        22
#define DMA_XFR_CTL_LSB_FECU_TRIG         23
#define DMA_XFR_CTL_LSB_FECU_SIDEBAND     24

#ifndef __ASSEMBLER__
typedef union DMA_XFR_CTL_TYPE {
   int value;
   struct{
      unsigned int channel_select: 5;
      unsigned int _reserved0: 2;
      unsigned int multi_burst: 1;
      unsigned int trans_mode_select: 3;
      unsigned int burst_type: 1;
      unsigned int ippu_go_en: 1;
      unsigned int vcpu_go_en: 1;
      unsigned int irq_en: 1;
      unsigned int ptr_rst: 1;
      unsigned int ext_trig: 1;
      unsigned int ippu_trig: 1;
      unsigned int di_mode: 3;
      unsigned int _reserved1: 1;
      unsigned int fecu_go_en: 1;
      unsigned int fecu_trig: 1;
      unsigned int sideband: 8;
   } field;
} DMA_XFR_CTL_TYPE;
#endif

// ---------------------------------------------------------------------------
// DMA_STAT_ABORT register defines and bitfield typedef  (VERY TBD if these define and bitfield structures will
// ever be useful for registers like this one.  If we get autogeneration of them for free, then they
// don't hurt anything if unused, and add to consistency if they are only used sometimes.)
// ---------------------------------------------------------------------------
#define DMA_STAT_ABORT_ADDR         0x30

#define DMA_STAT_ABORT_LSB_CHAN_0      0
#define DMA_STAT_ABORT_LSB_CHAN_1      1
#define DMA_STAT_ABORT_LSB_CHAN_2      2
#define DMA_STAT_ABORT_LSB_CHAN_3      3
#define DMA_STAT_ABORT_LSB_CHAN_4      4
#define DMA_STAT_ABORT_LSB_CHAN_5      5
#define DMA_STAT_ABORT_LSB_CHAN_6      6
#define DMA_STAT_ABORT_LSB_CHAN_7      7
#define DMA_STAT_ABORT_LSB_CHAN_8      8
#define DMA_STAT_ABORT_LSB_CHAN_9      9
#define DMA_STAT_ABORT_LSB_CHAN_10     10
#define DMA_STAT_ABORT_LSB_CHAN_11     11
#define DMA_STAT_ABORT_LSB_CHAN_12     12
#define DMA_STAT_ABORT_LSB_CHAN_13     13
#define DMA_STAT_ABORT_LSB_CHAN_14     14
#define DMA_STAT_ABORT_LSB_CHAN_15     15
#define DMA_STAT_ABORT_LSB_CHAN_16     16
#define DMA_STAT_ABORT_LSB_CHAN_17     17
#define DMA_STAT_ABORT_LSB_CHAN_18     18
#define DMA_STAT_ABORT_LSB_CHAN_19     19
#define DMA_STAT_ABORT_LSB_CHAN_20     20
#define DMA_STAT_ABORT_LSB_CHAN_21     21
#define DMA_STAT_ABORT_LSB_CHAN_22     22
#define DMA_STAT_ABORT_LSB_CHAN_23     23
#define DMA_STAT_ABORT_LSB_CHAN_24     24
#define DMA_STAT_ABORT_LSB_CHAN_25     25
#define DMA_STAT_ABORT_LSB_CHAN_26     26
#define DMA_STAT_ABORT_LSB_CHAN_27     27
#define DMA_STAT_ABORT_LSB_CHAN_28     28
#define DMA_STAT_ABORT_LSB_CHAN_29     29
#define DMA_STAT_ABORT_LSB_CHAN_30     30
#define DMA_STAT_ABORT_LSB_CHAN_31     31

#define DMA_STAT_ABORT_MASK_CHAN_0     (1 << DMA_STAT_ABORT_LSB_CHAN_0 )
#define DMA_STAT_ABORT_MASK_CHAN_1     (1 << DMA_STAT_ABORT_LSB_CHAN_1 )
#define DMA_STAT_ABORT_MASK_CHAN_2     (1 << DMA_STAT_ABORT_LSB_CHAN_2 )
#define DMA_STAT_ABORT_MASK_CHAN_3     (1 << DMA_STAT_ABORT_LSB_CHAN_3 )
#define DMA_STAT_ABORT_MASK_CHAN_4     (1 << DMA_STAT_ABORT_LSB_CHAN_4 )
#define DMA_STAT_ABORT_MASK_CHAN_5     (1 << DMA_STAT_ABORT_LSB_CHAN_5 )
#define DMA_STAT_ABORT_MASK_CHAN_6     (1 << DMA_STAT_ABORT_LSB_CHAN_6 )
#define DMA_STAT_ABORT_MASK_CHAN_7     (1 << DMA_STAT_ABORT_LSB_CHAN_7 )
#define DMA_STAT_ABORT_MASK_CHAN_8     (1 << DMA_STAT_ABORT_LSB_CHAN_8 )
#define DMA_STAT_ABORT_MASK_CHAN_9     (1 << DMA_STAT_ABORT_LSB_CHAN_9 )
#define DMA_STAT_ABORT_MASK_CHAN_10    (1 << DMA_STAT_ABORT_LSB_CHAN_10)
#define DMA_STAT_ABORT_MASK_CHAN_11    (1 << DMA_STAT_ABORT_LSB_CHAN_11)
#define DMA_STAT_ABORT_MASK_CHAN_12    (1 << DMA_STAT_ABORT_LSB_CHAN_12)
#define DMA_STAT_ABORT_MASK_CHAN_13    (1 << DMA_STAT_ABORT_LSB_CHAN_13)
#define DMA_STAT_ABORT_MASK_CHAN_14    (1 << DMA_STAT_ABORT_LSB_CHAN_14)
#define DMA_STAT_ABORT_MASK_CHAN_15    (1 << DMA_STAT_ABORT_LSB_CHAN_15)
#define DMA_STAT_ABORT_MASK_CHAN_16    (1 << DMA_STAT_ABORT_LSB_CHAN_16 )
#define DMA_STAT_ABORT_MASK_CHAN_17    (1 << DMA_STAT_ABORT_LSB_CHAN_17 )
#define DMA_STAT_ABORT_MASK_CHAN_18    (1 << DMA_STAT_ABORT_LSB_CHAN_18 )
#define DMA_STAT_ABORT_MASK_CHAN_19    (1 << DMA_STAT_ABORT_LSB_CHAN_19 )
#define DMA_STAT_ABORT_MASK_CHAN_20    (1 << DMA_STAT_ABORT_LSB_CHAN_20)
#define DMA_STAT_ABORT_MASK_CHAN_21    (1 << DMA_STAT_ABORT_LSB_CHAN_21)
#define DMA_STAT_ABORT_MASK_CHAN_22    (1 << DMA_STAT_ABORT_LSB_CHAN_22)
#define DMA_STAT_ABORT_MASK_CHAN_23    (1 << DMA_STAT_ABORT_LSB_CHAN_23)
#define DMA_STAT_ABORT_MASK_CHAN_24    (1 << DMA_STAT_ABORT_LSB_CHAN_24)
#define DMA_STAT_ABORT_MASK_CHAN_25    (1 << DMA_STAT_ABORT_LSB_CHAN_25)
#define DMA_STAT_ABORT_MASK_CHAN_26    (1 << DMA_STAT_ABORT_LSB_CHAN_26 )
#define DMA_STAT_ABORT_MASK_CHAN_27    (1 << DMA_STAT_ABORT_LSB_CHAN_27 )
#define DMA_STAT_ABORT_MASK_CHAN_28    (1 << DMA_STAT_ABORT_LSB_CHAN_28 )
#define DMA_STAT_ABORT_MASK_CHAN_29    (1 << DMA_STAT_ABORT_LSB_CHAN_29 )
#define DMA_STAT_ABORT_MASK_CHAN_30    (1 << DMA_STAT_ABORT_LSB_CHAN_30)
#define DMA_STAT_ABORT_MASK_CHAN_31    (1 << DMA_STAT_ABORT_LSB_CHAN_31)


#ifndef __ASSEMBLER__
typedef union DMA_STAT_ABORT_TYPE {
   int value;
   struct{
      unsigned int chan_0:1;
      unsigned int chan_1:1;
      unsigned int chan_2:1;
      unsigned int chan_3:1;
      unsigned int chan_4:1;
      unsigned int chan_5:1;
      unsigned int chan_6:1;
      unsigned int chan_7:1;
      unsigned int chan_8:1;
      unsigned int chan_9:1;
      unsigned int chan_10:1;
      unsigned int chan_11:1;
      unsigned int chan_12:1;
      unsigned int chan_13:1;
      unsigned int chan_14:1;
      unsigned int chan_15:1;
      unsigned int chan_16:1;
      unsigned int chan_17:1;
      unsigned int chan_18:1;
      unsigned int chan_19:1;
      unsigned int chan_20:1;
      unsigned int chan_21:1;
      unsigned int chan_22:1;
      unsigned int chan_23:1;
      unsigned int chan_24:1;
      unsigned int chan_25:1;
      unsigned int chan_26:1;
      unsigned int chan_27:1;
      unsigned int chan_28:1;
      unsigned int chan_29:1;
      unsigned int chan_30:1;
      unsigned int chan_31:1;
   } field;
} DMA_STAT_ABORT_TYPE;
#endif

// ---------------------------------------------------------------------------
// DMA_IRQ_STAT register defines and bitfield typedef
// ---------------------------------------------------------------------------
#define DMA_IRQ_STAT_ADDR       0x31

// ---------------------------------------------------------------------------
// DMA_COMP_STAT register defines and bitfield typedef
// ---------------------------------------------------------------------------
#define DMA_COMP_STAT_ADDR      0x32

// ---------------------------------------------------------------------------
// DMA_XFRERR_STAT register defines and bitfield typedef
// ---------------------------------------------------------------------------
#define DMA_XFRERR_STAT_ADDR    0x33

// ---------------------------------------------------------------------------
// DMA_CFGERR_STAT register defines and bitfield typedef
// ---------------------------------------------------------------------------
#define DMA_CFGERR_STAT_ADDR    0x34

// ---------------------------------------------------------------------------
// DMA_XRUN_STAT register defines and bitfield typedef
// ---------------------------------------------------------------------------
#define DMA_XRUN_STAT_ADDR      0x35

// ---------------------------------------------------------------------------
// DMA_GO_STAT register defines and bitfield typedef
// ---------------------------------------------------------------------------
#define DMA_GO_STAT_ADDR        0x36

// ---------------------------------------------------------------------------
// DMA_FIFO_STAT register defines and bitfield typedef
// ---------------------------------------------------------------------------
#define DMA_FIFO_STAT_ADDR      0x37

// ---------------------------------------------------------------------------
// DMA_FIFO_STAT register defines and bitfield typedef
// ---------------------------------------------------------------------------
#define DMA_FIFO_STAT_ADDR      0x37

// ---------------------------------------------------------------------------
// Load Register File Control Register
// ---------------------------------------------------------------------------
#define LRF_CTL_ADDR            0x40

// ---------------------------------------------------------------------------
// Load Register File Table Coefficient Registers
// ---------------------------------------------------------------------------
#define LRF_TBL_REAL_0_ADDR     0x41
#define LRF_TBL_IMAG_0_ADDR     0x42
#define LRF_TBL_REAL_1_ADDR     0x43
#define LRF_TBL_IMAG_1_ADDR     0x44
#define LRF_TBL_REAL_2_ADDR     0x45
#define LRF_TBL_IMAG_2_ADDR     0x46
#define LRF_TBL_REAL_3_ADDR     0x47
#define LRF_TBL_IMAG_3_ADDR     0x48
#define LRF_TBL_REAL_4_ADDR     0x49
#define LRF_TBL_IMAG_4_ADDR     0x4A
#define LRF_TBL_REAL_5_ADDR     0x4B
#define LRF_TBL_IMAG_5_ADDR     0x4C
#define LRF_TBL_REAL_6_ADDR     0x4D
#define LRF_TBL_IMAG_6_ADDR     0x4E
#define LRF_TBL_REAL_7_ADDR     0x4F
#define LRF_TBL_IMAG_7_ADDR     0x50

// ---------------------------------------------------------------------------
// VCPU Mode 0 Register
// ---------------------------------------------------------------------------
#define VCPU_MODE_0_ADDR        0x100

// ---------------------------------------------------------------------------
// VCPU Mode 1 Register
// ---------------------------------------------------------------------------
#define VCPU_MODE_1_ADDR        0x101

// ---------------------------------------------------------------------------
// VCPU CREG 0 Register
// ---------------------------------------------------------------------------
#define VCPU_CREG_0_ADDR        0x102

// ---------------------------------------------------------------------------
// VCPU CREG 1 Register
// ---------------------------------------------------------------------------
#define VCPU_CREG_1_ADDR        0x103

//----------------------------------------------------------------------------
// General Purpose Input Registers:
//----------------------------------------------------------------------------
#define GP_IN_0_ADDR            0x140          //!< General purpose input register 0  address.
#define GP_IN_1_ADDR            0x141          //!< General purpose input register 1  address.
#define GP_IN_2_ADDR            0x142          //!< General purpose input register 2  address.
#define GP_IN_3_ADDR            0x143          //!< General purpose input register 3  address.
#define GP_IN_4_ADDR            0x144          //!< General purpose input register 4  address.
#define GP_IN_5_ADDR            0x145          //!< General purpose input register 5  address.
#define GP_IN_6_ADDR            0x146          //!< General purpose input register 6  address.
#define GP_IN_7_ADDR            0x147          //!< General purpose input register 7  address.
#define GP_IN_8_ADDR            0x148          //!< General purpose input register 8  address.
#define GP_IN_9_ADDR            0x149          //!< General purpose input register 9  address.
#define GP_IN_10_ADDR           0x14A          //!< General purpose input register 10 address.
#define GP_IN_11_ADDR           0x14B          //!< General purpose input register 11 address.
#define GP_IN_12_ADDR           0x14C          //!< General purpose input register 12 address.
#define GP_IN_13_ADDR           0x14D          //!< General purpose input register 13 address.
#define GP_IN_14_ADDR           0x14E          //!< General purpose input register 14 address.
#define GP_IN_15_ADDR           0x14F          //!< General purpose input register 15 address.
#define GP_IN_16_ADDR           0x150          //!< General purpose input register 16 address.
#define GP_IN_17_ADDR           0x151          //!< General purpose input register 17 address.
#define GP_IN_18_ADDR           0x152          //!< General purpose input register 18 address.
#define GP_IN_19_ADDR           0x153          //!< General purpose input register 19 address.
#define GP_IN_20_ADDR           0x154          //!< General purpose input register 20 address.
#define GP_IN_21_ADDR           0x155          //!< General purpose input register 21 address.
#define GP_IN_22_ADDR           0x156          //!< General purpose input register 22 address.
#define GP_IN_23_ADDR           0x157          //!< General purpose input register 23 address.
#define GP_IN_24_ADDR           0x158          //!< General purpose input register 24 address.
#define GP_IN_25_ADDR           0x159          //!< General purpose input register 25 address.
#define GP_IN_26_ADDR           0x15A          //!< General purpose input register 26 address.
#define GP_IN_27_ADDR           0x15B          //!< General purpose input register 27 address.
#define GP_IN_28_ADDR           0x15C          //!< General purpose input register 28 address.
#define GP_IN_29_ADDR           0x15D          //!< General purpose input register 29 address.
#define GP_IN_30_ADDR           0x15E          //!< General purpose input register 30 address.
#define GP_IN_31_ADDR           0x15F          //!< General purpose input register 31 address.

// ---------------------------------------------------------------------------
// General Purpose Output Registers:
// ---------------------------------------------------------------------------
#define GP_OUT_0_ADDR           0x160           //!< General purpose output register 0  address.
#define GP_OUT_1_ADDR           0x161           //!< General purpose output register 1  address.
#define GP_OUT_2_ADDR           0x162           //!< General purpose output register 2  address.
#define GP_OUT_3_ADDR           0x163           //!< General purpose output register 3  address.
#define GP_OUT_4_ADDR           0x164           //!< General purpose output register 4  address.
#define GP_OUT_5_ADDR           0x165           //!< General purpose output register 5  address.
#define GP_OUT_6_ADDR           0x166           //!< General purpose output register 6  address.
#define GP_OUT_7_ADDR           0x167           //!< General purpose output register 7  address.
#define GP_OUT_8_ADDR           0x168           //!< General purpose output register 8  address.
#define GP_OUT_9_ADDR           0x169           //!< General purpose output register 9  address.
#define GP_OUT_10_ADDR          0x16A           //!< General purpose output register 10 address.
#define GP_OUT_11_ADDR          0x16B           //!< General purpose output register 11 address.
#define GP_OUT_12_ADDR          0x16C           //!< General purpose output register 12 address.
#define GP_OUT_13_ADDR          0x16D           //!< General purpose output register 13 address.
#define GP_OUT_14_ADDR          0x16E           //!< General purpose output register 14 address.
#define GP_OUT_15_ADDR          0x16F           //!< General purpose output register 15 address.
#define GP_OUT_16_ADDR          0x170           //!< General purpose output register 16 address.
#define GP_OUT_17_ADDR          0x171           //!< General purpose output register 17 address.
#define GP_OUT_18_ADDR          0x172           //!< General purpose output register 18 address.
#define GP_OUT_19_ADDR          0x173           //!< General purpose output register 19 address.
#define GP_OUT_20_ADDR          0x174           //!< General purpose output register 20 address.
#define GP_OUT_21_ADDR	        0x175           //!< General purpose output register 21 address.
#define GP_OUT_22_ADDR	        0x176           //!< General purpose output register 22 address.
#define GP_OUT_23_ADDR	        0x177           //!< General purpose output register 23 address.
#define GP_OUT_24_ADDR	        0x178           //!< General purpose output register 24 address.
#define GP_OUT_25_ADDR	        0x179           //!< General purpose output register 25 address.
#define GP_OUT_26_ADDR	        0x17A           //!< General purpose output register 26 address.
#define GP_OUT_27_ADDR	        0x17B           //!< General purpose output register 27 address.
#define GP_OUT_28_ADDR	        0x17C           //!< General purpose output register 28 address.
#define GP_OUT_29_ADDR	        0x17D           //!< General purpose output register 29 address.
#define GP_OUT_30_ADDR	        0x17E           //!< General purpose output register 30 address.
#define GP_OUT_31_ADDR	        0x17F           //!< General purpose output register 31 address.

// ---------------------------------------------------------------------------
//! @brief Macros to read/write GP_OUT register based on GP_OUT Number (rather than address)
// ---------------------------------------------------------------------------
#define GP_OUT_WRITE(gpo_num, mask, data)   __ip_write( (GP_OUT_0_ADDR + gpo_num), mask, data )
#define GP_OUT_READ(gpo_num, mask)          __ip_read( (GP_OUT_0_ADDR + gpo_num), mask)

// ---------------------------------------------------------------------------
//! @brief Macro to read GP_IN register based on GP_IN Number (rather than address)
// ---------------------------------------------------------------------------
#define GP_IN_READ(gpi_num, mask)   __ip_read( (GP_IN_0_ADDR + gpi_num), mask)

// ---------------------------------------------------------------------------
// VCPU to DQM Trace 32-bit Mailbox register:
// ---------------------------------------------------------------------------
#define DQM_SMALL_ADDR        0x180            //!< Data Acquisition Module 25-bit Trace Outbox register address.
#define DQM_SMALL_MASK        0x01FFFFFF     //!< Data Acquisition Module 25-bit Trace Outbox register bitmask.

// ---------------------------------------------------------------------------
// VCPU to DQM Trace 64-bit Mailbox registers:
// ---------------------------------------------------------------------------
#define DQM_LARGE_MSB_ADDR    0x181            //!< Data Acquisition Module 57-bit Trace Outbox MSB register address.
#define DQM_LARGE_MSB_MASK    0x01FFFFFF     //!< Data Acquisition Module 57-bit Trace Outbox MSB register bitmask.

#define DQM_LARGE_LSB_ADDR    0x182            //!< Data Acquisition Module 57-bit Trace Outbox LSB register address.
#define DQM_LARGE_LSB_MASK    0xFFFFFFFF     //!< Data Acquisition Module 57-bit Trace Outbox LSB register bitmask.

// ---------------------------------------------------------------------------
// VCPU to Debugger 32-bit Mailbox register:
// ---------------------------------------------------------------------------
#define VCPU_DBG_OUT_32_ADDR    0x188            //!< VCPU to Debugger 32-bit Outbox register address.
#define VCPU_DBG_OUT_32_MASK    0xFFFFFFFF     //!< VCPU to Debugger 32-bit Outbox register bitmask.

// ---------------------------------------------------------------------------
// VCPU to Debugger 64-bit Mailbox registers:
// ---------------------------------------------------------------------------
#define VCPU_DBG_OUT_64_MSB_ADDR   0x189         //!< VCPU to Debugger 64-bit Outbox MSB register address.
#define VCPU_DBG_OUT_64_MSB_MASK   0xFFFFFFFF  //!< VCPU to Debugger 64-bit Outbox MSB register bitmask.

#define VCPU_DBG_OUT_64_LSB_ADDR   0x18A         //!< VCPU to Debugger 64-bit Outbox LSB register address.
#define VCPU_DBG_OUT_64_LSB_MASK   0xFFFFFFFF  //!< VCPU to Debugger 64-bit Outbox LSB register bitmask.

// ---------------------------------------------------------------------------
// Debugger to VCPU 32-bit Mailbox register:
// ---------------------------------------------------------------------------
#define VCPU_DBG_IN_32_ADDR     0x18B            //!< Debugger to VCPU 32-bit Inbox register address.
#define VCPU_DBG_IN_32_MASK     0xFFFFFFFF     //!< Debugger to VCPU 32-bit Inbox register bitmask.

// ---------------------------------------------------------------------------
// Debugger to VCPU 64-bit Mailbox registers:
// ---------------------------------------------------------------------------
#define VCPU_DBG_IN_64_MSB_ADDR 		0x18C       //!< Debugger to VCPU 64-bit Inbox MSB register address.
#define VCPU_DBG_IN_64_MSB_MASK 		0xFFFFFFFF  //!< Debugger to VCPU 64-bit Inbox MSB register bitmask.

#define VCPU_DBG_IN_64_LSB_ADDR 		0x18D       //!< Debugger to VCPU 64-bit Inbox LSB register address.
#define VCPU_DBG_IN_64_LSB_MASK 		0xFFFFFFFF	//!< Debugger to VCPU 64-bit Inbox LSB register bitmask.

// ---------------------------------------------------------------------------
// VCPU to Debugger Mailbox Status register:
// ---------------------------------------------------------------------------
#define VCPU_DBG_MBOX_STAT_ADDR     	0x18E      	//!< VCPU to Debugger Mailbox Status register address.
#define VCPU_DBG_MBOX_STAT_MASK       0x0000000F	//!< VCPU to Debugger Mailbox Status register bitmask.

#define VCPU_DBG_MBOX_STAT_32_OUT_VALID (0x1 << 0) //!< 32-bit message outbox valid bitmask.
#define VCPU_DBG_MBOX_STAT_64_OUT_VALID (0x1 << 1) //!< 64-bit message outbox valid bitmask
#define VCPU_DBG_MBOX_STAT_32_IN_VALID  (0x1 << 2) //!< 32-bit message inbox valid bitmask.
#define VCPU_DBG_MBOX_STAT_64_IN_VALID  (0x1 << 3) //!< 64-bit message inbox valid bitmask

// ---------------------------------------------------------------------------
// VCPU to Host 64-bit Outgoing Mailbox 0 registers:
// ---------------------------------------------------------------------------
#define VCPU_TO_HOST_OUTBOX0_MSB_ADDR	0x190       //!< VCPU to Host Outbox 0 MSB register address.
#define VCPU_TO_HOST_OUTBOX0_MSB_MASK	0xFFFFFFFF  //!< VCPU to Host Outbox 0 MSB register bitmask.

#define VCPU_TO_HOST_OUTBOX0_LSB_ADDR   0x191       //!< VCPU to Host Outbox 0 LSB register address.
#define VCPU_TO_HOST_OUTBOX0_LSB_MASK   0xFFFFFFFF  //!< VCPU to Host Outbox 0 LSB register bitmask.

// ---------------------------------------------------------------------------
// VCPU to Host 64-bit Outgoing Mailbox 1 registers:
// ---------------------------------------------------------------------------
#define VCPU_TO_HOST_OUTBOX1_MSB_ADDR	0x192       //!< VCPU to Host Outbox 1 MSB register address.
#define VCPU_TO_HOST_OUTBOX1_MSB_MASK	0xFFFFFFFF  //!< VCPU to Host Outbox 1 MSB register bitmask.

#define VCPU_TO_HOST_OUTBOX1_LSB_ADDR   0x193       //!< VCPU to Host Outbox 1 LSB register address.
#define VCPU_TO_HOST_OUTBOX1_LSB_MASK   0xFFFFFFFF  //!< VCPU to Host Outbox 1 LSB register bitmask.

// ---------------------------------------------------------------------------
// VCPU from Host 64-bit Incoming Mailbox 0 registers:
// ---------------------------------------------------------------------------
#define VCPU_FROM_HOST_INBOX0_MSB_ADDR	0x194       //!< VCPU from Host Inbox 0 MSB register address.
#define VCPU_FROM_HOST_INBOX0_MSB_MASK	0xFFFFFFFF  //!< VCPU from Host Inbox 0 MSB register bitmask.

#define VCPU_FROM_HOST_INBOX0_LSB_ADDR	0x195       //!< VCPU from Host Inbox 0 LSB register address.
#define VCPU_FROM_HOST_INBOX0_LSB_MASK  0xFFFFFFFF  //!< VCPU from Host Inbox 0 LSB register bitmask.

// ---------------------------------------------------------------------------
// VCPU from Host 64-bit Incoming Mailbox 1 registers:
// ---------------------------------------------------------------------------
#define VCPU_FROM_HOST_INBOX1_MSB_ADDR	0x196       //!< VCPU from Host Inbox 1 MSB register address.
#define VCPU_FROM_HOST_INBOX1_MSB_MASK	0xFFFFFFFF  //!< VCPU from Host Inbox 1 MSB register bitmask.

#define VCPU_FROM_HOST_INBOX1_LSB_ADDR	0x197       //!< VCPU from Host Inbox 1 LSB register address.
#define VCPU_FROM_HOST_INBOX1_LSB_MASK  0xFFFFFFFF  //!< VCPU from Host Inbox 1 LSB register bitmask.

// ---------------------------------------------------------------------------
// VCPU's VCPU/Host Mailbox Status register:
// ---------------------------------------------------------------------------
#define VCPU_HOST_MBOX_STAT_ADDR        0x198       //!< VCPU to Host Mailbox Status register address.
#define VCPU_HOST_MBOX_STAT_MASK        0x0000000F  //!< VCPU to Host Mailbox Status register bitmask.

#define VCPU_HOST_MBOX_STAT_OUT0_PEND   (0x1 << 0)  //!< VCPU Outbox0 Msg pending (unread by host) bitmask.
#define VCPU_HOST_MBOX_STAT_OUT1_PEND   (0x1 << 1)  //!< VCPU Outbox1 Msg pending (unread by host) bitmask.
#define VCPU_HOST_MBOX_STAT_IN0_PEND    (0x1 << 2)  //!< VCPU Inbox0 Msg pending bitmask.
#define VCPU_HOST_MBOX_STAT_IN1_PEND    (0x1 << 3)  //!< VCPU Inbox1 Msg pending bitmask.

// ---------------------------------------------------------------------------
// IPPU_CONTROL Reordering Unit control register.
// ---------------------------------------------------------------------------
#define IPPU_CONTROL_ADDR              	0x1C0       //!< IPPU control register address.
#define IPPU_CONTROL_MASK              	0xEDC0FFFF  //!< IPPU control register bitmask.
#define IPPU_CONTROL_MASK_ADDR			0x0000FFFF  //!< IPPU thread address.

#define IPPU_CONTROL_IPPU_IRQ_EN        (0x1U << 22)
#define IPPU_CONTROL_VCPU_GO_EN         (0x1U << 23)    //!< Enable VCPU GO upon IPPU completion
#define IPPU_CONTROL_LEG_ADDRESSING     (0x1U << 24)
#define IPPU_CONTROL_FECU_TRIG          (0x1U << 26)
#define IPPU_CONTROL_DMA_TRIG           (0x1U << 27)

#define IPPU_CONTROL_START_TYPE         (0x7U << 29)
#define IPPU_CONTROL_START_TYPE_DMA     (0x1U << 29)
#define IPPU_CONTROL_START_TYPE_FECU    (0x2U << 29)
#define IPPU_CONTROL_START_TYPE_NOW     (0x4U << 29)

#ifndef __ASSEMBLER__
typedef union IPPU_CONTROL_TYPE {
   int value;
   struct {
      unsigned int ippu_start_address:16;
      unsigned int :12;
      unsigned int dma_ippu_go_en:1;
      unsigned int ippu_abort:1;
      unsigned int ippu_suspend:1;
      unsigned int ippu_go:1;
   } field;
} IPPU_CONTROL_TYPE;
#endif

// ---------------------------------------------------------------------------
// IPPU_STATUS Reordering Unit status register.
// ---------------------------------------------------------------------------
#define IPPU_STATUS_ADDR             	0x1C1       //!< IPPU status register address.
#define IPPU_STATUS_MASK               	0xFC00FFFF  //!< IPPU status register bitmask.

#define IPPU_STATUS_ERROR              	(0x1 << 27) //!< IPPU processing error flag.
#define IPPU_STATUS_DONE               	(0x1 << 28) //!< IPPU processing completion flag.
#define IPPU_STATUS_ABORTED            	(0x1 << 29) //!< IPPU processing aborted flag.
#define IPPU_STATUS_SUSPENDED          	(0x1 << 30) //!< IPPU processing suspended flag.
#define IPPU_STATUS_BUSY               	(0x1 << 31) //!< IPPU processing on-going flag.

#ifndef __ASSEMBLER__
typedef union IPPU_STATUS_TYPE {
   int value;
   struct {
      unsigned int ippu_pc:16;
      unsigned int :10;
      unsigned int dma_ippu_go_last:1;
      unsigned int ippu_error:1;
      unsigned int ippu_done:1;
      unsigned int ippu_aborted:1;
      unsigned int ippu_suspended:1;
      unsigned int ippu_busy:1;
   } field;
} IPPU_STATUS_TYPE;
#endif

// ---------------------------------------------------------------------------
// IPPU Run Control Register
// ---------------------------------------------------------------------------
#define IPPU_RUN_CTL_ADDR       0x1C2

// ---------------------------------------------------------------------------
// IPPU_ARG_BASE_ADDR Reordering Unit argument base address register.
// ---------------------------------------------------------------------------
#define IPPU_ARG_BASE_ADDR_ADDR 		0x1C3       //!< IPPU argument base address register address.
#define IPPU_ARG_BASE_ADDR_MASK 		0x0001FFFF  //!< IPPU argument base address register bitmask.

#ifndef __ASSEMBLER__
typedef union IPPU_ARG_BASE_ADDR_TYPE {
   int value;
   struct {
      unsigned int ippu_arg_base_addr:17;
   } field;
} IPPU_ARG_BASE_ADDR_TYPE;
#endif

// ---------------------------------------------------------------------------
// IPPU_HW_VER Reordering Unit hardware version register.
// ---------------------------------------------------------------------------
#define IPPU_HW_VER_ADDR  0x1C4              //!< IPPU hardware version register address.
#define IPPU_HW_VER_MASK  0xFFFFFFFF         //!< IPPU hardware version register bitmask.

#ifndef __ASSEMBLER__
typedef union IPPU_HW_VER_TYPE {
   int value;
   struct {
      unsigned int ippu_hw_version:32;
   } field;
} IPPU_HW_VER_TYPE;
#endif

// ---------------------------------------------------------------------------
// IPPU_SW_VER Reordering Unit software version register.
// ---------------------------------------------------------------------------
#define IPPU_SW_VER_ADDR  0x1C5              //!< IPPU software version register address.
#define IPPU_SW_VER_MASK  0xFFFFFFFF         //!< IPPU software version register bitmask.

#ifndef __ASSEMBLER__
typedef union IPPU_SW_VER_TYPE {
   int value;
   struct {
      unsigned int ippu_sw_version:32;
   } field;
} IPPU_SW_VER_TYPE;
#endif

#ifndef __ASSEMBLER__
// -----------------------------------------------------------------------------
//! @brief          Read VSPA GO event(s) from system control IP register.
//! @return         GO status bits from system control IP register.
//! @see            vspaEventRead()
// -----------------------------------------------------------------------------
static inline unsigned int vspaControlGoRead(
    void )
{
    return __IP_READ( CONTROL_ADDR, CONTROL_MASK_GO );
}

// -----------------------------------------------------------------------------
//! @brief          Clear VSPA GO event(s) in system control IP register.
//! @param[in]      mask Specifies the event(s) to clear.
//! @return         Void.
//! @see            vspaEventClear()
// -----------------------------------------------------------------------------
static inline void vspaControlGoClear(
    unsigned int const mask )
{
    __IP_WRITE( CONTROL_ADDR, mask, mask );
}

// -----------------------------------------------------------------------------
//! @brief          Enable VSPA GO event(s) in system control IP register.
//! @param[in]      mask Specifies the GO event(s) to enable.
// -----------------------------------------------------------------------------
static inline void vspaControlGoEnable(
    unsigned int const mask )
{
    __IP_WRITE( CONTROL_ADDR, mask, mask );
}
#endif // __ASSEMBLER__

//! @} GROUP_IPREG
#endif // __IPREG_TYPES_H__
