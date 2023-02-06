##
# @file         dbg.tcl
# @brief        Debugger interface.
# @author       NXP Semiconductors.
# @copyright    Copyright 2017 NXP

# NXP Confidential. This software is owned or controlled by NXP and may only be used strictly
# in accordance with the applicable license terms. By expressly accepting
# such terms or by downloading, installing, activating and/or otherwise using
# the software, you are agreeing that you have read, and that you agree to
# comply with and are bound by, such license terms. If you do not agree to
# be bound by the applicable license terms, then you may not retain,
# install, activate or otherwise use the software.


namespace eval dbg {
    set version 1.0

    namespace export init

    namespace export peek
    namespace export poke
    namespace export load
    namespace export save
}

package provide dbg $dbg::version

##
# @brief        Initialize a debug session.
# @note         This procedure must be called after the debug session is started.
#
proc dbg::init {} {
    radix x
	config MemIdentifier p
	config MemWidth 32
	config MemSwap off
    config runControlSync on
    config hexPrefix 0x
    config binPrefix 0b
    config showCommas off
    config hexPadding on
    config decPadding off
    config memReadMax 1073741824
    config memAccess 32
    display off all
}

##
# @brief        32-bit AXI read access.
#
# This function reads @c size 32-bit words(s) starting at address @c addr.
#
# @param    addr Specifies the word-aligned address to read from.
# @param    size Specifies the number of 32-bit words to read.
#                Defaults to 1.
#
# @return   The 32-bit word(s) read from AXI address space.
#
proc dbg::peek {addr {size 1}} {
    set data [mem p:[format "0x%lX" \
        [expr $addr]]..[format "0x%lX" [expr $addr+4*($size-1)]] -s -np]
    return $data
}

##
# @brief    32-bit AXI write access
#
# This procedure writes @c size 32-bit word(s) starting at address @c addr.
#
# @param    addr Specifies the word-aligned address to write to.
# @param    data Value to write.
# @param    size Specifies the number of 32-bit words to write.
#                Defaults to 1.
#
# @return   void.
#
proc dbg::poke {addr data {size 1}} {
    mem p:[format "0x%lX" [expr $addr]]..[format "0x%lX" \
        [expr $addr+4*($size-1)]] -s =[format "0x%8.8X" [expr $data]]
}

##
# @brief    Load a portion of the AXI address space from a file
#
# This procedure writes an array of 32-bit elements starting at address @c addr.
#
# If @c size is omitted or zero, the size of the array is defined by the number
# of 32-bit elements in the file @c path.
#
# If @c size is greater than zero, and less than the number of 32-bit elements
# in the file @c path, the size of the array is @c size.
#
# If @c size is greater than zero, and greater than the number of 32-bit elements
# in the file @c path, the size of the array is the number of 32-bit elements
# in the file @c path.
#
# @param    addr Specifies the word-aligned address to read from.
# @param    path Specifies the relative or absolute path for the input file.
# @param    size Specifies the number of 32-bit words to load (defaults to 0).
#
# @return   void.
#
proc dbg::load {addr path {size 0}} {
    if {![file isfile $path]} {
        error "$path is not a file."
    }

    if {[string equal -nocase [file extension $path] ".hex"] || [string equal -nocase [file extension $path] ".dma"]} {
        if {[catch {set fhex [open $path {RDONLY}]} msg]} {
            error $msg
        }
        set dhex [read -nonewline $fhex]
        close $fhex

        # accept elements with or without leading 0x
        set dhex [string map {"0x" ""} [string tolower $dhex]]

        set pbin [format "%s.bin" [file rootname $path]]
        if {[catch {set fbin [open $pbin {WRONLY CREAT}]} msg]} {
            error $msg
        }
        fconfigure $fbin -translation binary
        if {($size > 0) && ([expr $size] < [llength $dhex])} {
            set dhex [lrange $dhex 0 [expr $size-1]]
        }
        foreach val $dhex {
            puts -nonewline $fbin [binary format i* [scan $val "%08x\n"]]
        }
        close $fbin
        if {[catch {restore -b $pbin p:[format "0x%lX" [expr $addr]]} msg]} {
            error $msg
        }
        file delete $pbin
    } elseif {[string equal -nocase [file extension $path] ".bin"]} {
        if {[catch {restore -b $path p:[format "0x%lX" [expr $addr]]} msg]} {
            error $msg
        }
    } else {
        error "Unknown file extension!"
    }
}

##
# @brief    Save a portion of the AXI address space to a file
#
# This procedure saves @c size 32-bit words starting at address @c addr
# to the file @c path.
#
# @param    addr Specifies the word-aligned address to read from.
# @param    path Specifies the relative or absolute path for the output file.
# @param    size Specifies the number of 32-bit words to save (defaults to 1).
#
# @return   void.
#
# @note     The file specified by @c path is overwritten if it already exists.
#
proc dbg::save {addr path {size 1} } {
    set pbin [format "%s.bin" [file rootname $path]]
    file delete $pbin
    if {[catch {::save -b p:[format "0x%lX" \
         [expr $addr]]..[format "0x%lX" [expr $addr+4*$size-1]] $pbin -o} msg]} {
        error $msg
    }
    if {[catch {set fbin [open $pbin {RDONLY}]} msg]} {
        error $msg
    }
    fconfigure $fbin -translation binary
    set dbin [read $fbin]
    close $fbin
    file delete $pbin
    binary scan $dbin i* dhex
    file delete $path
    if {[catch {set fhex [open $path {WRONLY CREAT}]} msg]} {
        error $msg
    }
    foreach val $dhex {
        puts $fhex [format "%08X" $val]
    }
    close $fhex
}

proc dbg::dump {addr path {size 1} } {
    set pbin [format "%s.bin" [file rootname $path]]
    file delete $pbin
    if {[catch {::save -b x:[format "0x%lX" \
         [expr $addr]]..[format "0x%lX" [expr $addr+4*$size-1]] $pbin -o} msg]} {
        error $msg
    }
    if {[catch {set fbin [open $pbin {RDONLY}]} msg]} {
        error $msg
    }
    fconfigure $fbin -translation binary
    set dbin [read $fbin]
    close $fbin
    file delete $pbin
    binary scan $dbin i* dhex
    file delete $path
    if {[catch {set fhex [open $path {WRONLY CREAT}]} msg]} {
        error $msg
    }
    foreach val $dhex {
        puts $fhex [format "%08X" $val]
    }
    close $fhex
}

##
# @brief    Fill a portion of the AXI address space
#
# This procedure writes the value(s) in @c data to the array starting at @c addr.
#
# If @c size is omitted or zero, the number of elements in @c data specifies the
# size of the written array.
#
# If @c size is not zero and less than the size of @c data, then @c size specifies
# the size of the written array, and the array is written with the first @c size
# elements from @c data.
#
# Otherwise, only the number of elements in @c data are written.
#
# @param    addr Specifies the byte-aligned address to write to.
# @param    data List of data to write.
# @param    size Specifies the number of 32-bit words to write.
#                Defaults to 0.
#
# @return   void.
#
proc dbg::fill {addr data {size 0}} {
    if {([expr $size] > 0) && ([expr $size] < [llength $data])} {
        set data [lrange $data 0 [expr $size-1]]
    }
    foreach val $data {
        mem p:[format "0x%lX" [expr $addr]] -s =$val
        set addr [expr $addr+4]
    }
}

##
# @brief    Wait for condition to become true or until timeout
#
# If condition still evaluates to false on timeout - error is thrown.
# Example usage:
# dbg::wait_until "vspa::is_stopped 0" 5 1000
#
# @param condition
# @param attempts How many times procedure should evaluate condition.
#                 First attemt is done immediatelly.
# @param interval Time between attempts.
#
proc dbg::wait_until {condition {attempts 0} {interval 1}} {
    set iteration 0
    while {[eval $condition] == 0} {
        incr iteration [expr $attempts != 0]
        if {$iteration == $attempts} {
            error "wait_until($condition): timeout reached!"
        } else {
            wait $interval
        }
    }
}