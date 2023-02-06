# ==============================================================================
#! @file            diffFiles.tcl
#! @brief           CodeWarrior VSPA debugger helper functions.
#! @copyright       Copyright (C) 2012-2013 Freescale Semiconductor, Ltd.
# ==============================================================================
# NXP Confidential. This software is owned or controlled by NXP and may only be used strictly
# in accordance with the applicable license terms. By expressly accepting
# such terms or by downloading, installing, activating and/or otherwise using
# the software, you are agreeing that you have read, and that you agree to
# comply with and are bound by, such license terms. If you do not agree to
# be bound by the applicable license terms, then you may not retain,
# install, activate or otherwise use the software.

set DIFF_FILE_MATCH     0
set DIFF_FILE_MISMATCH  1
set DIFF_FILE_DIRNAME   [file normalize [file dirname [info script]]]

# ------------------------------------------------------------------------------
#! @brief           Function to compare two files for bit-exactness.
#!                  Uses the diff.exe executable for this purpose.
#!
#! @return          0/1 for match/mismatch
# ------------------------------------------------------------------------------
proc diffFiles { FilePath1 FilePath2 } {

    global DIFF_FILE_MATCH
    global DIFF_FILE_MISMATCH
    global DIFF_FILE_DIRNAME
    
    # Compare
    set errCode [catch { exec "${DIFF_FILE_DIRNAME}/diff.exe" -s $FilePath1 $FilePath2 } errMsg ]
    
    if { $errCode == 0 } then {
        if {[string first "identical" $errMsg] == -1} then {
            puts "************************************"
            puts "ERROR: Output data not correct!" 
            puts "************************************"
            return $DIFF_FILE_MISMATCH
        } else {
            puts "************************************"
            puts "OK: Output data correct."
            puts "************************************"
            return $DIFF_FILE_MATCH
        }
    } else {
        puts "ERROR: ${errMsg}!"
        return -1
    }
}









