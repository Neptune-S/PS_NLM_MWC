// =============================================================================
//! @file           bss.s
//! @brief          Assebly file to wipe out bss section.
//! @copyright      Copyright 2021 NXP
// =============================================================================
/*  NXP Confidential. This software is owned or controlled by NXP and may only be used strictly
 *  in accordance with the applicable license terms. By expressly accepting
 *  such terms or by downloading, installing, activating and/or otherwise using
 *  the software, you are agreeing that you have read, and that you agree to
 *  comply with and are bound by, such license terms. If you do not agree to
 *  be bound by the applicable license terms, then you may not retain,
 *  install, activate or otherwise use the software.
 */
	.file    "bss.s"

    /* Initialize the bss with zeros */
    .global  _wipe_bss
    .type    _wipe_bss, @function
_wipe_bss
    mv       a0, __bss_start;
    mv       a1, __bss_end;
    sub      a2, a1, a0;
    mv       g0, a2;
    cmp      g0, 64;
    fnop;
    jmp.lt   .Lsmall;
    mv       g2, 0;
    fnop;
.Lbig
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    st       [a0]+2, g2;
    sub      g0, g0, 64;
    cmp      g0, 64;
    fnop;
    jmp.ge   .Lbig;
    fnop;
    fnop;
.Lsmall
    cmp      g0, 0;
    fnop;
    jmp.eq   .Lfinish;
    fnop;
    fnop;
    jmp      .Lsmall;
    sth      [a0]+1, g2;
    sub      g0, g0, 1;
.Lfinish
    rts;
    fnop;
    fnop;
    .size    _wipe_bss, .-_wipe_bss
