# ==============================================================================
#! @file            vspa.tcl
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

#! @defgroup        GROUP_TCL CodeWarrior VSPA Debugger Tcl Procedures
#! @{

# ------------------------------------------------------------------------------
#! @brief           Initialize the debugger.
#! @return          void.
#!
#! This procedure configures the debugger for subsequent use
#! VSPA debugger helper procedures.
#!
#! The debugger is configured as follows:
#!  - runControlSync is on;
#!  - Hexadecimal values are prefixed with '0x';
#!  - Binary values are prefixed with '0b';
#!  - Hexadecimal, binary and decimal values are not displayed with grouping;
#!  - Hexadecimal padding is on;
#!  - Decimal padding is off;
#!  - Memory read access range is set to 1 GiB;
#!  - Memory access is configured for 32-bit accesses.
# ------------------------------------------------------------------------------
proc dbgInit {} {
    config runControlSync on
    config hexPrefix 0x
    config binPrefix 0b
    config showCommas off
    config hexPadding on
    config decPadding off
    config memReadMax 1073741824
    config memAccess 32
    config memWidth 32
    display off all
    config echoCmd off
}

# ------------------------------------------------------------------------------
#! @brief           Initialize the cycle accurate simulator.
#!
#! This procedure configures the cycle accurate simulator.
#!
#! @param[in]       vcpu specifies the number of data memory vectors on VCPU.
#! @param[in]       ippu specifies the number of data memory vectors on IPPU.
#! @return          void.
# ------------------------------------------------------------------------------
proc casInit { vcpu ippu } {
    set_dmem_partition [expr $vcpu] [expr $ippu]
}

# ------------------------------------------------------------------------------
#! @brief           Initialize the single-precision core cycle accurate simulator.
#!
#! This procedure initializes the cycle accurate simulator for the data memory
#! map of a single precision core.
#!
#! @return          void.
# ------------------------------------------------------------------------------
proc casInitCoreFloat32 {} {
    casInit 248 8
}

# ------------------------------------------------------------------------------
#! @brief           Initialize the double-precision core cycle accurate simulator.
#!
#! This procedure initializes the cycle accurate simulator for the data memory
#! map of a double precision core.
#!
#! @return          void.
# ------------------------------------------------------------------------------
proc casInitCoreFloat64 {} {
    casInit 1007 17
}

# ------------------------------------------------------------------------------
#! @brief           Clear memory.
#!
#! This procedure clears @a size 32-bit words in memory
#! starting from address @a addr, using the memory space @a space.
#! If the argument @a space is not specified, the memory space
#! defaults to VCPU data memory.
#!
#! @param[in]       addr    specifies the memory address.
#! @param[in]       size    specfies the number of 32-bit words to clear.
#! @param[in]       space   (optional) specifies the memory space.
#!                          -  x: VCPU program memory
#!                          -  m: VCPU data memory (default)
#!                          -  r: VCPU register file
#!                          - ip: IP register map
#!                          - ix: IPPU program memory
#!                          - im: IPPU data memory
#!                          -  s: SoC debug address space
#!                          -  p: AXI address space
#! @return          void.
# ------------------------------------------------------------------------------
proc memClear { addr size {space "m"} } {
    if { [catch {mem $space:$addr [expr $size] =0} msg] } {
        logErr $msg
    }
}

# ------------------------------------------------------------------------------
#! @brief           Randomize memory.
#!
#! This procedure randomizes @a size 32-bit words in memory
#! starting from address @a addr, using the memory space @a space.
#! If the argument @a space is not specified, the memory space
#! defaults to VCPU data memory.
#!
#! @param[in]       addr    specifies the memory address.
#! @param[in]       size    specfies the number of 32-bit words to randomize.
#! @param[in]       space   (optional) specifies the memory space.
#!                          -  x: VCPU program memory
#!                          -  m: VCPU data memory (default)
#!                          -  r: VCPU register file
#!                          - ip: IP register map
#!                          - ix: IPPU program memory
#!                          - im: IPPU data memory
#!                          -  s: SoC debug address space
#!                          -  p: AXI address space
#! @return          void.
# ------------------------------------------------------------------------------
proc memRand { addr size {space "m"} } {
    for { set ptr $addr } { $ptr < [expr $addr+$size] } { incr ptr } {
        if { [catch {mem $space:$ptr=[expr { int( 2147483647 * rand() ) }]} msg] } {
            logErr $msg
            break
        }
    }
}

# ------------------------------------------------------------------------------
#! @brief           Read a 32-bit word from VSPA memory.
#!
#! This function reads a 32-bit value from VSPA memory.
#!
#! @param[in]       addr    specifies the 32-bit word address.
#! @param[in]       space   (optional) specifies the memory space.
#!                          -  x: VCPU program memory
#!                          -  m: VCPU data memory (default)
#!                          -  r: VCPU register file
#!                          - ip: IP register map
#!                          - ix: IPPU program memory
#!                          - im: IPPU data memory
#! @return          the value read from memory.
# ------------------------------------------------------------------------------
proc memRead32 { addr {space "m"} } {
    if { [catch {set val [format "0x%08X" [byteSwap32 [mem $space:$addr %x -np]]]} msg] } {
        logErr $msg
        return 0
    } else {
        return $val
    }
}

# ------------------------------------------------------------------------------
#! @brief           Write a 32-bit word in VSPA memory.
#!
#! This function write a 32-bit value to VSPA memory.
#!
#! @param[in]       addr    specifies the 32-bit word address.
#! @param[in]       val     specifies the 32-bit value to write.
#! @param[in]       space   (optional) specifies the memory space.
#!                          -  x: VCPU program memory
#!                          -  m: VCPU data memory (default)
#!                          -  r: VCPU register file
#!                          - ip: IP register map
#!                          - ix: IPPU program memory
#!                          - im: IPPU data memory
#! @return          void.
# ------------------------------------------------------------------------------
proc memWrite32 { addr val {space "m"} } {
    if { [catch {mem $space:$addr =[byteSwap32 [expr $val]]} msg] } {
        logErr $msg
    } else {
        logMsg [format "Memory $space:0x%08x written with 0x%08X" $addr [expr $val]]
    }
}

# ------------------------------------------------------------------------------
#! @brief           Read a 32-bit word from AXI address space.
#!
#! This function reads a 32-bit value from AXI address space.
#!
#! @param[in]       addr    specifies the memory address.
#! @param[in]       space   (optional) specifies the AXI memory space.
#!                          -  s: SoC debug address space
#!                          -  p: AXI address space (default)
#! @return          void.
# ------------------------------------------------------------------------------
proc axiRead32 { addr {space "p"} } {
    config memSwap on
    if { [catch {set data [mem $space:$addr %x -np]} msg] } {
        config memSwap off
        logErr $msg
        return 0
    }
    config memSwap off
    return $data
}

# ------------------------------------------------------------------------------
#! @brief           Write a 32-bit word to AXI address space.
#!
#! This procedure writes a 32-bit value to the AXI address space.
#!
#! @param[in]       addr    specifies the memory address.
#! @param[in]       value   specifies the 32-bit value to write.
#! @param[in]       space   (optional) specifies the AXI memory space.
#!                          -  s: SoC debug address space
#!                          -  p: AXI address space (default)
#! @return          void.
# ------------------------------------------------------------------------------
proc axiWrite32 { addr value {space "p"} } {
    config memSwap on
    if { [catch {mem $space:$addr =[format "%08x" [expr $value]]} msg] } {
        config memSwap off
        logErr $msg
    }
    config memSwap off
}

# ------------------------------------------------------------------------------
#! @brief           Load a data file to AXI memory.
#!
#! This procedure loads an ASCII hexadecimal file to AXI address space.
#!
#! @param[in]       fhex    specifies the data file path and name.
#! @param[in]       addr    specifies the memory address.
#! @param[in]       space   (optional) specifies the AXI memory space.
#!                          -  s: SoC debug address space
#!                          -  p: AXI address space (default)
#! @return          void.
# ------------------------------------------------------------------------------
proc axiLoad { fhex addr {space "p"} } {

    # Make sure <fhex> is a file:
    if { ![file isfile $fhex] } {
        logErr "$fhex is not a file"
        return
    }

    # Create a binary file from the data file:
    set fbin [format "%s.bin" [file rootname $fhex]]
    set count [hex2bin $fhex $fbin]

    # Load the binary file into memory:
    if { [catch {restore -b $fbin $space:$addr} msg] } {
        file delete $fbin
        logMsg [format "Loading $space:0x%08X from $fhex ..." $addr]
        logErr $msg
    } else {
        logMsg [format "Loaded $count word(s) at $space:0x%08X from $fhex" $addr]
    }

    # Delete the binary file:
    file delete $fbin
}

# ------------------------------------------------------------------------------
#! @brief           Save AXI memory to data file.
#!
#! This procedure saves an array from the AXI address space to an ASCII
#! hexadecimal file.
#!
#! @param[in]       fhex    specifies the data file path and name.
#! @param[in]       addr    specifies the memory address.
#! @param[in]       size    specfies the number of 32-bit words to save.
#! @param[in]       space   (optional) specifies the AXI memory space.
#!                          -  s: SoC debug address space
#!                          -  p: AXI address space (default)
#! @return          void.
# ------------------------------------------------------------------------------
proc axiSave { fhex addr size {space "p"} } {

    logMsg [format "Saving $space:0x%08X to $fhex ..." $addr]

    # Delete the data file if it already exists:
    if { [file exists $fhex] } {
        file delete $fhex
    }

    # Attemp to open the file in write modeL
    if { [catch {set fidhex [open $fhex {WRONLY CREAT}]} msg] } {
        logErr $msg
        return
    }

    # Switch endianness for AXI address space, read memory, restore endianness,
    # write the data file with hexadecimal values:
    config memSwap on
    set dmem [mem $space:$addr [expr $size] %x -np]
    config memSwap off
    set count 0
    foreach val $dmem {
        puts $fidhex [format "%08x" $val]
        incr count
    }
    logMsg [format "Saved $count word(s) from $space:0x%08X to $fhex" $addr]
    close $fidhex
}

# ------------------------------------------------------------------------------
#! @brief           Read a 32-bit variable from VSPA data memory.
#!
#! This function reads a 32-bit value from VSPA data memory.
#!
#! @param[in]       addr    specifies the VSPA memory address.
#! @param[in]       space   (optional) specifies the VSPA memory address space.
#!                          -  m: VCPU data memory (default)
#!                          - im: IPPU data memory
#! @return          the 32-bit value read from VSPA dat memory.
# ------------------------------------------------------------------------------
proc vspRead32 { addr {space "m"} } {
    return memRead32 $addr $space
}

# ------------------------------------------------------------------------------
#! @brief           Write a 32-bit variable to VSPA data memory.
#!
#! This procedure writes a 32-bit value to VSPA data memory space.
#!
#! @param[in]       addr    specifies the VSPA memory address.
#! @param[in]       val     specifies the 32-bit value to write.
#! @param[in]       space   (optional) specifies the VSPA memory address space.
#!                          -  m: VCPU data memory (default)
#!                          - im: IPPU data memory
#! @return          void.
# ------------------------------------------------------------------------------
proc vspWrite32 { addr val {space "m"} } {
    memWrite32 $addr $val $space
}

# ------------------------------------------------------------------------------
#! @brief           Load a data file to VSPA memory.
#!
#! This procedure loads VSPA data memory with values read from a file in ASCII
#! hexadecimal format (wihtout leading '0x').
#!
#! @param[in]       fhex    specifies the data file path and name.
#! @param[in]       addr    specifies the VSPA memory address.
#! @param[in]       space   (optional) specifies the VSPA memory address space.
#!                          -  x: VCPU program memory
#!                          -  m: VCPU data memory (default)
#!                          -  r: VCPU register file
#!                          - ip: IP register map
#!                          - ix: IPPU program memory
#!                          - im: IPPU data memory
#! @return          void.
# ------------------------------------------------------------------------------
proc vspLoad { fhex addr {space "m"} } {
    axiLoad $fhex $addr $space
}

# ------------------------------------------------------------------------------
#! @brief           Save VSPA memory to data file.
#!
#! This procedure saves VSPA data memory to a file in ASCII hexadecimal format
#! (without leading '0x').
#!
#! @param[in]       fhex    specifies the data file path and name.
#! @param[in]       addr    specifies the VSPA memory address.
#! @param[in]       size    specifies the number of 32-bit words to save.
#! @param[in]       space   (optional) specifies the VSPA memory address space.
#!                          -  x: VCPU program memory
#!                          -  m: VCPU data memory (default)
#!                          -  r: VCPU register file
#!                          - ip: IP register map
#!                          - ix: IPPU program memory
#!                          - im: IPPU data memory
#! @return          void.
# ------------------------------------------------------------------------------
proc vspSave { fhex addr size {space "m"} } {

    logMsg [format "Saving $space:0x%08X to $fhex ..." $addr]

    # Delete the data file if it already exists:
    if { [file exists $fhex] } {
        file delete $fhex
    }

    # Attempt to open the file in write mode:
    if { [catch {set fidhex [open $fhex {WRONLY CREAT}]} msg] } {
        logErr $msg
        return
    }

    # Read memory and write the data file with hexadecimal values:
    set dmem [mem $space:$addr [expr $size] %x -np]
    set count 0
    foreach val $dmem {
        puts $fidhex [format "%08x" [byteSwap32 $val]]
        incr count
    }
    logMsg [format "Saved $count word(s) from $space:%08X to $fhex" $addr]
    close $fidhex
}

# ------------------------------------------------------------------------------
#! @brief           Read a 32-bit register.
#!
#! This function returns the 32-bit value of a register.
#!
#! @param[in]       reg specifies the name of the register.
#! @return          the 32-bit value of the register.
# ------------------------------------------------------------------------------
proc regRead { reg } {
    if { [catch {set val [format "0x%08X" [display $reg -np]]} msg] } {
        logErr $msg
        return 0
    } else {
        return $val
    }
}

# ------------------------------------------------------------------------------
#! @brief           Write a 32-bit register.
#!
#! This procedure write a 32-bit value to a register.
#!
#! @param[in]       reg specifies the name of the register.
#! @param[in]       val specifies the 32-bit value to write.
#! @return          void.
# ------------------------------------------------------------------------------
proc regWrite { reg val } {
    if { [catch {reg $reg=[format "0x%8X" [expr $val]]} msg] } {
        logErr $msg
    } else {
        logMsg [format "Register $reg written with 0x%08X" [expr $val]]
    }
}

# ------------------------------------------------------------------------------
#! @brief           Get the address of a VSPA variable.
#!
#! This function returns the address of a VSPA variable.
#!
#! @attention       For arrays, the variable name is the name of the first
#!                  element in the array, i.e. array[0].
#!
#! @param[in]       var specifies the name of the variable.
#! @return          the address of the variable.
# ------------------------------------------------------------------------------
proc varAddr { var } {
    if { [catch {set addr [format "0x%08X" [display &$var %x -np]]} msg] } {
        logErr $msg
        return 0
    } else {
        return $addr
    }
}

# ------------------------------------------------------------------------------
#! @brief           Read a 32-bit VSPA variable.
#!
#! This function returns the value of a 32-bit variable in VSPA data memory.
#!
#! @param[in]       var specfies the variable name.
#! @return          the variable value.
# ------------------------------------------------------------------------------
proc varRead { var } {
    if { [catch {set val [format "0x%08X" [display $var %x -np]]} msg] } {
        logErr $msg
        return 0
    } else {
        return $val
    }
}

# ------------------------------------------------------------------------------
#! @brief           Write a 32-bit VSPA variable.
#!
#! This procedure write a value to a 32-bit variable in VSPA data memory.
#!
#! @param[in]       var specifies the variable name.
#! @param[in]       val specifies the value to write.
#! @return          void.
# ------------------------------------------------------------------------------
proc varWrite { var val } {
    if { [catch {mem m:[varAddr $var] =[byteSwap32 [expr $val]]} msg] } {
        logErr $msg
    } else {
        logMsg [format "Variable $var written with 0x%08X" [expr $val]]
    }
}

# ------------------------------------------------------------------------------
#! @brief           Load a data file to an array variable.
#!
#! This procedure loads an ASCII hex file to an array or structure in VSPA
#! data memory.
#!
#! @param[in]       var     specifies the array variable name.
#! @param[in]       fhex    specifies the data file path and name.
#! @return          void.
# ------------------------------------------------------------------------------
proc varLoad { var fhex } {
    # Make sure <fhex> is a file:
    if { ![file isfile $fhex] } {
        logErr "$fhex is not a file"
        return
    }

    # Create a binary file from the data file:
    set fbin [format "%s.bin" [file rootname $fhex]]
    set count [hex2bin $fhex $fbin]

    # Load the binary file into memory:
    if { [catch {restore -b $fbin m:[varAddr $var]} msg] } {
        file delete $fbin
        logMsg "Loading $var from $fhex ..."
        logErr $msg
    } else {
        logMsg "Loaded $var with $count word(s) from $fhex"
    }

    # Delete the binary file:
    file delete $fbin
}

proc varSave { var size fhex } {
    vspSave $fhex [varAddr $var] $size
}

# ------------------------------------------------------------------------------
#! @brief           Connect a DMA channel to file I/O.
#!
#! This procedure connects a DMA channel to a file in read access, and a file
#! in write access.
#! - Whenever the VCPU uses this DMA channel to read data from the AXI address
#!   space, the simulator reads data from the file in read access.
#! - Whenever the VCPU uses this DMA channel to write data to the AXI address
#!   space, the simulator writes data to the file in write access.
#!
#! This procedure takes the I/O file stem as input argument. For example,
#! if the stem is 'file', then the file in read access is assumed to be named
#! 'file_RD.dma' and the file in write access is assumed to be named
#! 'file_WR.dma'.
#!
#! @param[in]       chnl specifies the DMA channel number.
#! @param[in]       stem specifies the stem for the I/O file.
#! @return          void.
# ------------------------------------------------------------------------------
proc dmaConnect { chnl stem } {
    dma_channel [expr $chnl] $stem
    logMsg "DMA channel [format %2i [expr $chnl]]: $stem"
}

# ------------------------------------------------------------------------------
#! @brief           Configure a DMA channel external trigger.
#!
#! This procedure configures the external trigger properties for a DMA channel.
#!
#! @param[in]       chnl    specifies the DMA channel number.
#! @param[in]       delay   specifies the delay, in VCPU cycles, of the first
#!                          trigger.
#! @param[in]       period  specifies the period, in VCPU cycles, of the trigger.
#! @param[in]       count   specifies the number of triggers to generate.
#! @return          void.
# ------------------------------------------------------------------------------
proc dmaTrigger { chnl delay period count } {
    dma_trigger [expr $chnl] [expr $delay] [expr $period] [expr $count]
    logMsg "DMA trigger [format %2i [expr $chnl]]: [expr $delay] [expr $period] [expr $count]"
}

# ------------------------------------------------------------------------------
#! @brief           Program the DMA controller to perform a data transfer.
#!
#!
#!
#! @param[in]       ctrl
#! @param[in]       size
#! @param[in]       paxi
#! @param[in]       pvsp
#! @return          void
# ------------------------------------------------------------------------------
proc dmaTransfer { ctrl size paxi pvsp } {
    regWrite
}

# ------------------------------------------------------------------------------
#! @brief           Compare two data files.
#!
#! This function compares two ASCII data files and returns a PASS/FAIL status.
#! The number of 32-bit values to compare must be specified.
#! An offset in any or both files may be specified.
#!
#! @param[in]       fvsp specifies the name of the ASCII data file obtained
#!                       from CodeWarrior.
#! @param[in]       fref specifies the name of the ASCII reference file
#!                       (e.g. obtained from Matlab).
#! @param[in]       size specifies the number of words to compare.
#! @param[in]       ovsp (optional) specifies an offset in the ASCII data file
#!                       from CodeWarrior.
#! @param[in]       oref (optional) specifies an offset in the ASCII reference
#!                       file.
#! @retval          0    indicates files are different.
#! @retval          1    indicates files are identical.
# ------------------------------------------------------------------------------
proc fileCheck { fvsp fref size {ovsp 0} {oref 0} } {

    puts "Checking $fvsp\[$ovsp\] against $fref\[$oref\] for $size words ..."

    # Attempt to open test output file in read-only mode:
    if { [catch {set fidvsp [open $fvsp {RDONLY}]} msg] } {
        error $msg
        return 0
    }

    # Attempt to open reference output file in read-only mode:
    if { [catch {set fidref [open $fref {RDONLY}]} msg] } {
        close $fidvsp
        error $msg
        return 0
    }

    # Skip <ovsp> lines from test output file:
    set offset [expr $ovsp]
    while { $offset > 0 } {
        if { [eof $fidvsp] } {
            error [format "$fvsp: end of file before offset %d!" [expr $ovsp]]
            return 0
        }
        gets $fidvsp dummy
        incr offset -1
    }

    # Skip <oref> lines from reference output file:
    set offset [expr $oref]
    while { $offset > 0 } {
        if { [eof $fidref] } {
            error [format "$fref: end of file before offset %d!" [expr $oref]]
            return 0
        }
        gets $fidref dummy
        incr offset -1
    }

    # Loop over <size> lines:
    set eflag 0
    set count 0
    while { $count < [expr $size] } {

        # Read hex value from test and reference output files:
        scan [gets $fidvsp] "%08x" hexvsp
        scan [gets $fidref] "%08x" hexref

        # Terminate loop earlier on end of file:
        if { [eof $fidvsp] || [eof $fidref] } {
            break
        }

        # Compare test and reference values:
        if { [expr $hexvsp] == [expr $hexref] } {
            incr count
            continue
        }

        # Compare 16-bit half-words, and compare only numbers
        # with absolute value greater than zero, to avoid mismatch on +/-0.

        # Least significant 16-bit half-word:
        if { ([expr [expr $hexvsp] & 0x0000FFFF] != [expr [expr $hexref] & 0x0000FFFF]) && \
            (([expr [expr $hexvsp] & 0x00007FFF] != 0) || ([expr [expr $hexref] & 0x00007FFF] != 0)) } {
            puts [format "FAIL: vsp\[%d\] = 0x%08x - ref\[%d\] = 0x%08x" [expr $count + [expr $ovsp]] $hexvsp [expr $count + [expr $oref]] $hexref]
            set eflag 1
            break
        }

        # Most significant 16-bit half-word:
        if { ([expr [expr $hexvsp] & 0xFFFF0000] != [expr [expr $hexref] & 0xFFFF0000]) && \
            (([expr [expr $hexvsp] & 0x7FFF0000] != 0) || ([expr [expr $hexref] & 0x7FFF0000] != 0)) } {
            puts [format "FAIL: vsp\[%d\] = 0x%08x - ref\[%d\] = 0x%08x" [expr $count + [expr $ovsp]] $hexvsp [expr $count + [expr $oref]] $hexref]
            set eflag 1
            break
        }

        # Increment line counter:
        incr count
    }
    close $fidref
    close $fidvsp

    # Return PASS/FAIL status:
    if { $eflag || ($count < [expr $size]) } {
        puts [format "FAIL \[%i/%i\]" $count [expr $size] [expr 100*[format "%f" $count]/[format "%f" [expr $size]]]]
        return 0
    } else {
        puts [format "PASS \[%i/%i\]" $count [expr $size] [expr 100*[format "%f" $count]/[format "%f" [expr $size]]]]
        return 1
    }
}

# ------------------------------------------------------------------------------
#! @brief           Convert an ASCII data file to a binary file.
#!
#! This functions converts an ASCII data file, containing 32-bit hexadecimal
#! values (without leading 0x), into a binary file, and returns the number of
#! 32-bit words read from the ASCII data file and written to the binary file.
#!
#! @param[in]       fhex specifies the name of the input ASCII data file.
#! @param[in]       fbin specifies the name of the output binary file.
#! @return          number of 32-bit words converted.
# ------------------------------------------------------------------------------
proc hex2bin { fhex fbin } {

    # Attempt to open the ASCII file in read-only mode:
    if { [catch {set fidhex [open $fhex {RDONLY}]} msg] } {
        error $msg
        return 0
    }

    # Delete the binary file if it already exists:
    if { [file exists $fbin] } {
        file delete $fbin
    }

    # Attempt to open the binary file in write mode:
    if { [catch {set fidbin [open $fbin {WRONLY CREAT}]} msg] } {
        close $fidhex
        error $msg
        return 0
    }

    # Configure binary file to be accessed in binary mode:
    fconfigure $fidbin -translation binary

    # Read the ASCII file line by line, extract the value, write the value
    # in binary file:
    set count 0
    while { [gets $fidhex hex] > 0 } {
        puts -nonewline $fidbin [binary format i [scan $hex "%08x"]]
        incr count
    }

    close $fidbin
    close $fidhex
    return $count
}

# ------------------------------------------------------------------------------
#! @brief           Swap bytes in a 16-bit half-word.
#!
#! This function swaps the 2 bytes of a 16-bit half-word and returns the swapped
#! version of the 16-bit half-word.
#!
#! @param[in]       x the 16-bit half-word to swap.
#! @return          the swapped version of the 16-bit half-word.
# ------------------------------------------------------------------------------
proc byteSwap16 { x } {
    set b0 0
    set b1 0
    scan [format "0x%04x" [expr $x]] "0x%02x%02x" b1 b0
    return [format "0x%02x%02x" $b0 $b1]
}

# ------------------------------------------------------------------------------
#! @brief           Swap bytes in a 32-bit word.
#!
#! This function modifies the endianness of a 32-bit word by swapping all bytes
#! in the 32-bit word @a x and returning the modified 32-bit word.
#!
#! The 32-bit word @a x consists in 4 bytes b0, b1, b2, b3, with b0 being
#! the least significant byte and b3 being the most significant byte.
#! This function returns the modified 32-bit word consisting in bytes b3, b2,
#! b1, b0 with b3 being the least significan byte and b0 being the most
#! significant byte.
#!
#! @param[in]       x the 32-bit word to swap.
#! @return          the swapped version of the 32-bit word.
# ------------------------------------------------------------------------------
proc byteSwap32 { x } {
    set b0 0
    set b1 0
    set b2 0
    set b3 0
    scan [format "0x%08x" [expr $x]] "0x%02x%02x%02x%02x" b3 b2 b1 b0
    return [format "0x%02x%02x%02x%02x" $b0 $b1 $b2 $b3]
}

# ------------------------------------------------------------------------------
#! @brief           Swap half-words in a 32-bit word.
#!
#! This function swaps the 2 half-words in a 32-bit word and returns the swapped
#! version of the 32-bit word.
#!
#! @param[in]       x the 32-bit word to swap.
#! @return          the swapped version of @a x.
# ------------------------------------------------------------------------------
proc hwordSwap32 { x } {
    set h0 0
    set h1 0
    scan [format "0x%08x" [expr $x]] "0x%04x%04x" h1 h0
    return [format "0x%04x%04x" $h0 $h1]
}

# ------------------------------------------------------------------------------
#! @brief           Log an informative message.
#! @param[in]       msg specifies the message to log.
#! @return          void.
# ------------------------------------------------------------------------------
proc logMsg { msg } {
    puts $msg
}

# ------------------------------------------------------------------------------
#! @brief           Log an error message.
#! @param[in]       msg specifies the error message to log.
#! @return          void.
# ------------------------------------------------------------------------------
proc logErr { msg } {
    error $msg
}

# ------------------------------------------------------------------------------
#! @brief           Wait for a list of cores to complete.
#!
#! This function waits for a list of cores to complete their process
#! and get to the 'stopped' state. This function blocks until all specified
#! cores are in the stopped state. The list of cores is specified
#! by their target index as assigned by the debugger.
#!
#! @param[in]       indexList   specifies the list of core indexes to wait for.
#! @param[in]       waitTime    (optional) specifies the time, in microseconds,
#!                              between each status check.
#! @return          1 after all cores complete (blocking function).
# ------------------------------------------------------------------------------
proc waitForCompletion { indexList {waitTime 1000} } {
    set mcstatus [switchtarget]
    set lines [split $mcstatus "\n"]
    if { [llength $indexList] > [llength $lines] } {
        error "Waiting for [llength $indexList] cores to complete but debugging only [llength $lines] cores!"
    }
    while { 1 } {
        set count 0
        foreach index $indexList {
            set status [lindex $lines $index]
            if { [string first "state=Stopped" $status] != -1 } {
                incr count
            }
        }
        if { $count == [llength $indexList] } {
            return 1
        }
        wait $waitTime
        set mcstatus [switchtarget]
        set lines [split $mcstatus "\n"]
    }
}

#! @}
