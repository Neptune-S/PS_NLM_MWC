# ==============================================================================
#! @file            passFail.tcl
#! @brief           CodeWarrior VSPA debugger helper functions.
#! @copyright       Copyright (C) 2016 Freescale Semiconductor, Ltd.
# ==============================================================================
# NXP Confidential. This software is owned or controlled by NXP and may only be used strictly
# in accordance with the applicable license terms. By expressly accepting
# such terms or by downloading, installing, activating and/or otherwise using
# the software, you are agreeing that you have read, and that you agree to
# comply with and are bound by, such license terms. If you do not agree to
# be bound by the applicable license terms, then you may not retain,
# install, activate or otherwise use the software.
# ------------------------------------------------------------------------------
#! @brief           Simple function to summarize the results of a test.
#! @return          void.
# ------------------------------------------------------------------------------
proc passFail { err_count } {

    if {$err_count == 0} {
        puts "*************************************************************************" 
        puts "   PPPPP      AA        SSSS    SSSS " 
        puts "   PP  PP    AAAA      SS  SS  SS  SS" 
        puts "   PPPPP    AA  AA      SS      SS   " 
        puts "   PP      AAAAAAAA       SS      SS " 
        puts "   PP     AA      AA   SS  SS  SS  SS" 
        puts "   PP    AA        AA   SSSS    SSSS " 
        puts "*************************************************************************" 
        puts {   Test finished successfully }
    } else {
        puts "*************************************************************************" 
        puts "   FFFFFF     AA       II   LL     "
        puts "   FF        AAAA      II   LL     "
        puts "   FFFFF    AA  AA     II   LL     "
        puts "   FF      AAAAAAAA    II   LL     "
        puts "   FF     AA      AA   II   LL     "
        puts "   FF    AA        AA  II   LLLLLL "
        puts "*************************************************************************" 
        puts "   Test finished with $err_count errors "
    }
    return 1
}









