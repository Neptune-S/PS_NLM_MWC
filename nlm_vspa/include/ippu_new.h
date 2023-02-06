// =============================================================================
//! @file       ippu.h
//! @brief      Inter-vector permutation processing unit definitions.
//! @author     NXP Semiconductors
//! @copyright  Copyright (C) 2016 NXP Semiconductors.
//!
//! This file ippu.h defines the interface to monitor and control
//! the Inter-vector Permutation Processing Unit (IPPU).
//!
// =============================================================================

#ifndef __IPPU_H__
#define __IPPU_H__

#include "vspa.h"
#include "iohw.h"

//! @defgroup   GROUP_VSP_IPREG Internal Peripherals
//! @brief      Internal peripherals interface.
//! @{

//! @defgroup   GROUP_IPPU_REGS     IPPU Registers
//! @brief      This section provides definitions for the IPPU registers.
//! @{

//! @defgroup   GROUP_IPPU_REGS_CR  IPPU control register
//! @brief      IPPU control register definitions.
//! @{

#define IPPU_CR_FIELD_PROC  (0x0000FFFF)    //!< Procedure address.
#define IPPU_CR_FIELD_VCPU  (0x3 << 22)     //!< Completion trigger for VCPU.
#define IPPU_CR_FIELD_MODE  (0x1 << 24)     //!< Addressing mode.
#define IPPU_CR_FIELD_TRIG  (0x3 << 26)     //!< Completion trigger for peripherals.
#define IPPU_CR_FIELD_TYPE  (0x7 << 29)     //!< Start type.

#define IPPU_CR_VCPU_IRQ    (0x1 << 22)     //!< Assert interrupt on completion.
#define IPPU_CR_VCPU_GO     (0x1 << 23)     //!< Assert go event on completion.

#define IPPU_CR_MODE_32BIT  (0x0 << 24)     //!< 32-bit addressing mode.
#define IPPU_CR_MODE_16BIT  (0x1 << 24)     //!< 16-bit addressing mode.

#define IPPU_CR_TRIG_DMA    (0x1 << 27)     //!< Assert trigger to DMA on completion.
#define IPPU_CR_TRIG_FEC    (0x1 << 26)     //!< Assert trigger to DECU on completion.

#define IPPU_CR_TYPE_DMA    (0x1 << 29)     //!< Start IPPU on DMA transfer completion.
#define IPPU_CR_TYPE_FEC    (0x2 << 29)     //!< Start IPPU on FECU operation completion.
#define IPPU_CR_TYPE_NOW    (0x4 << 29)     //!< Start IPPU instantly.

#ifndef __ASSEMBLER__
#pragma cplusplus on
//! @brief      Test if the IPPU completed execution of a procedure.
//! @retval     zero        The IPPU did not complete the execution
//!                         of a procedure.
//! @retval     non-zero   The IPPU did complete the execution
//!                         of a procedure.
static inline uint32_t ippu_is_done(void)
{
    return iord(IPPUSTATUS, 0x1 << 28);
}

//! @brief  Write the IPPU control register.
//!
//! This function writes the value @a cr to the IPPU control register.
//!
//! @param  cr      value to write to the control register.
//! @return This function does not return a value.
static inline void __ippu_cr(uint32_t cr)
{
    __ipwr(0x700 >> 2, cr);
}

//! @brief  Write a field in the IPPU control register.
//!
//! This function writes the field identified with @a mask with the value @a cr
//! in the IPPU control register.
//!
//! @param  mask    mask indicating the field to write.
//! @param  cr      value to write to the control register.
//! @return This function does not return a value.
static inline void __ippu_cr_field(uint32_t mask, uint32_t cr)
{
    __ipwr(0x700 >> 2, mask, cr);
}

//! @brief  Read the IPPU control register.
//!
//! This function reads the IPPU control register value.
//!
//! @return The IPPU control register value.
static inline uint32_t __ippu_cr(void)
{
    return __iprd(0x700 >> 2);
}

//! @brief  Read a field in the IPPU control register.
//!
//! This function reads the field identified with @a mask
//! from the IPPU control register value.
//!
//! @param  mask    mask indicating the field to read.
//! @return The IPPU control register value.
static inline uint32_t __ippu_cr_field(uint32_t mask)
{
    return __iprd(0x700 >> 2, mask);
}
#pragma cplusplus reset
#endif // __ASSEMBLER__
//! @}

//! @defgroup   GROUP_IPPU_REGS_SR  IPPU status register
//! @brief      IPPU status register definitions.
//! @{

#define IPPU_SR_FIELD_PC        (0x0000FFFF)    //!< Program counter.
#define IPPU_SR_FIELD_FULL      (0x1 << 20)     //!< Command FIFO full.
#define IPPU_SR_FIELD_BUSY      (0x1 << 21)     //!< Busy flag.
#define IPPU_SR_FIELD_DEPTH     (0x3 << 22)     //!< FIFO depth value.
#define IPPU_SR_FIELD_LAST      (0x3 << 25)     //!< Last IPPU command source.
#define IPPU_SR_FIELD_ERROR     (0x1 << 27)     //!< Error flag.
#define IPPU_SR_FIELD_DONE      (0x1 << 28)     //!< Done flag.
#define IPPU_SR_FIELD_ABORT     (0x1 << 29)     //!< Aborted flag.
#define IPPU_SR_FIELD_SUSPEND   (0x1 << 30)     //!< Suspended flag.
#define IPPU_SR_FIELD_ACTIVE    (0x1 << 31)     //!< Active flag.

#define IPPU_SR_FULL            (0x1 << 20)     //!< Command FIFO is full.
#define IPPU_SR_BUSY            (0x1 << 21)     //!< IPPU is busy.
#define IPPU_SR_ERROR           (0x1 << 27)     //!< An error occurred.
#define IPPU_SR_DONE            (0x1 << 28)     //!< IPPU is done.
#define IPPU_SR_ABORT           (0x1 << 29)     //!< IPPU is aborted.
#define IPPU_SR_SUSPEND         (0x1 << 30)     //!< IPPU is suspended.
#define IPPU_SR_ACTIVE          (0x1 << 31)     //!< IPPU is active.

#ifndef __ASSEMBLER__
#pragma cplusplus on
//! @brief  Read the IPPU status register.
//!
//! This function reads the IPPU status register value.
//!
//! @return The IPPU status register value.
static inline uint32_t __ippu_sr(void)
{
    return __iprd(0x704 >> 2);
}

//! @brief  Read a field from the IPPU status register.
//!
//! This function reads the field specified by @a mask
//! from the IPPU status register value.
//!
//! @param  mask    mask indicating the field to read.
//! @return The IPPU status register value.
static inline uint32_t __ippu_sr_field(uint32_t mask)
{
    return __iprd(0x704 >> 2, mask);
}
#pragma cplusplus reset
#endif // __ASSEMBLER__

//! @}

//! @defgroup   GROUP_IPPU_REGS_RC  Run control register
//! @{

#define IPPU_RC_FIELD_ABORT     (0x1 << 29)     //!< Abort current and pending processes.
#define IPPU_RC_FIELD_SUSPEND   (0x1 << 30)     //!< Suspend current process.
#define IPPU_RC_FIELD_CLEAR     (0x1 << 31)     //!< Clear command FIFO error bit.

#define IPPU_RC_ABORT           (0x1 << 29)     //!< Abort current and pending processes.
#define IPPU_RC_SUSPEND         (0x1 << 30)     //!< Suspend current process.
#define IPPU_RC_RESUME          (0x0 << 30)     //!< Resume current process.
#define IPPU_RC_CLEAR           (0x1 << 31)     //!< Clear command FIFO error bit.

#ifndef __ASSEMBLER__
#pragma cplusplus on
//! @brief  Write the IPPU run control register.
//!
//! This function writes the value @a rc to the IPPU run control register.
//!
//! @param  rc the value to write to the run control register.
//! @return This function does not return a value.
static inline void __ippu_rc(uint32_t rc)
{
    __ipwr(0x708 >> 2, rc);
}

//! @brief  Write a field in the IPPU run control register.
//!
//! This function writes the field identified with @a mask with the value @a rc
//! in the IPPU run control register.
//!
//! @param  mask    mask indicating the field to write.
//! @param  rc      value to write to the run control register.
//! @return This function does not return a value.
static inline void __ippu_rc_field(uint32_t mask, uint32_t rc)
{
    __ipwr(0x708 >> 2, mask, rc);
}

//! @brief  Read the IPPU run control register.
//!
//! This function reads the IPPU run control register value.
//!
//! @return The IPPU run control register value.
static inline uint32_t __ippu_rc(void)
{
    return __iprd(0x708 >> 2);
}

//! @brief  Read a field from the IPPU run control register.
//!
//! This function reads the field specified by @a mask
//! from the IPPU run control register value.
//!
//! @param  mask    mask indicating the field to read.
//! @return The IPPU status register value.
static inline uint32_t __ippu_rc_field(uint32_t mask)
{
    return __iprd(0x708 >> 2, mask);
}
#pragma cplusplus reset
#endif // __ASSEMBLER__

//! @}

//! @defgroup   GROUP_IPPU_REGS_AR  Argument base address register
//! @{

#ifndef __ASSEMBLER__
#pragma cplusplus on

//! @brief  Write the IPPU argument base address register.
//!
//! This function writes the value @a ar
//! to the IPPU argument base address register.
//!
//! @param  rc the value to write to the argument base address register.
//! @return This function does not return a value.
static inline void __ippu_ar(uint32_t ar)
{
    __ipwr(0x70C >> 2, ar);
}

//! @brief  Read the IPPU argument base address register.
//!
//! This function reads the IPPU argument base address register value.
//!
//! @return The IPPU argument base address register value.
static inline uint32_t __ippu_ar(void)
{
    return __iprd(0x70C >> 2);
}
#pragma cplusplus reset
#endif // __ASSEMBLER__

//! @}

//! @}

//! @defgroup   GROUP_IPPU_API  Logical interface
//! @{

#ifndef __ASSEMBLER__
//! @brief  IPPU procedure pointer type.
typedef void (*ippuProcT)(void);

//! @brief  Clear the IPPU process completion event.
//!
//! This function clears the IPPU go event
//!
//! @return This function does not return a value.
static inline void ippuClear(void)
{
    __ipwr(0x8 >> 2, 0x1 << 1, 0x1 << 1);
}

//! @brief  Enable the IPPU process.
//!
//! This function registers the IPPU process @a proc
//! with properties @a ctrl.
//!
//! @param  proc the IPPU procedure address in IPPU PRAM space.
//! @param  ctrl the IPPU control register value associated.
//! @return This function does not return a value.
static inline void ippuEnable(ippuProcT proc, uint32_t ctrl)
{
    __ippu_cr(ctrl | (uint32_t)proc);
}

//! @brief  Disable current and pending IPPU processes.
//!
//! This function disables current and pending registered IPPU processes.
//!
//! @return This function does not return a value.
static inline void ippuDisable(void)
{
    __ippu_rc_field(IPPU_RC_FIELD_ABORT, IPPU_RC_ABORT);
}

//! @brief  Abort current and pending IPPU processes.
//!
//! This function aborts current and pending registered IPPU processes.
//!
//! @return This function does not return a value.
static inline void ippuAbort(void)
{
    __ippu_rc_field(IPPU_RC_FIELD_ABORT, IPPU_RC_ABORT);
}

//! @brief  Suspend current IPPU process.
//!
//! This function suspends the current running IPPU process.
//!
//! @return This function does not return a value.
//! @see    ippu_resume()
static inline void ippuSuspend(void)
{
    __ippu_rc_field(IPPU_RC_FIELD_SUSPEND, IPPU_RC_SUSPEND);
}

//! @brief  Resume a suspended IPPU process.
//!
//! This function resumes a suspended IPPU process.
//!
//! @return This function does not return a value.
//! @see    ippu_suspend()
static inline void ippuResume(void)
{
    __ippu_rc_field(IPPU_RC_FIELD_SUSPEND, IPPU_RC_RESUME);
}

//! @brief  Test if the IPPU is running.
//!
//! This function indicates whether the IPPU is idle or running a process.
//!
//! @retval true the IPPU is executing a process.
//! @retval false the IPPU is not executing a process.
static inline uint32_t ippuIsBusy(void)
{
    return __ippu_sr_field(IPPU_SR_BUSY);
}

//! @brief  Test if the IPPU command queue is full.
//!
//! This function indicates whether the IPPU command queue is full,
//! meaning the IPPU cannot register a new process.
//!
//! @retval true the IPPU command queue is full.
//! @retval false the IPPU command queue is not full.
static inline uint32_t ippuIsFull(void)
{
    return __ippu_sr_field(IPPU_SR_FULL);
}

//! @brief  Test if the IPPU is done executing a procedure.
//!
//! This function indicates whether the IPPU finished executing a procedure.
//!
//! @note   This function may return true although a procedure is pending
//!         execution.
//!
//! @retval true the IPPU finished executing a procedure.
//! @retval false the IPPU is executing a procedure.
static inline uint32_t ippuIsDone(void)
{
    return __ippu_sr_field(IPPU_SR_DONE);
}

//! @brief  Test if the IPPU is executing a procedure or a procedure is pending.
//!
//! This function indicates whether the IPPU is currently executing a procedure,
//! or if a procedure is pending execution.
//!
//! @retval true the IPPU is executing a procedure or a procedure is pending.
//! @retval false the IPPU is not executing a procedure and there is no pending
//          procedure.
static inline uint32_t ippuIsActive(void)
{
    return __ippu_sr_field(IPPU_SR_ACTIVE);
}



#endif // __ASSEMBLER__

//! @}

#endif // __IPPU_H__
