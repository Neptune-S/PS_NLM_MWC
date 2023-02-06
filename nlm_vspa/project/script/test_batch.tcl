# ==============================================================================
#! @file            test_run.tcl
#! @copyright       Copyright 2018 NXP
#! @brief           TCL script for testing implementation.
# ==============================================================================
# NXP Confidential. This software is owned or controlled by NXP and may only be used strictly
# in accordance with the applicable license terms. By expressly accepting
# such terms or by downloading, installing, activating and/or otherwise using
# the software, you are agreeing that you have read, and that you agree to
# comply with and are bound by, such license terms. If you do not agree to
# be bound by the applicable license terms, then you may not retain,
# install, activate or otherwise use the software.

# Change path to current script
cd [file dirname [info script]]
# Load TCL configuration (parameters)
source test_cfg.tcl

# Include common tcl scripts
source vspa.tcl
source passFail.tcl
source diffFiles.tcl
source jenkins.tcl
source dbg.tcl
# Initialize jenkins 
jenkinsInit


if {$CELL_SEARCH == 1} {
	set op_mode 1
	set no_iterations 3
	set Folder "cell_search"
} elseif {$CELL_FOLLOW == 1} {
	set op_mode 5
	# 76 is second sss; 150 is for reset
	set no_iterations 217
	set Folder "cell_follow"
} elseif {$MIB_DECODE == 1} {
	set op_mode 2
	set no_iterations 120
	set Folder "mib_decode"
} else {
	# NMM.Stop
	set op_mode 0
	set no_iterations 0
	set Folder "cell_search"
}

# Paths relative to this script
set ReportPath    "../../doc/Report.csv"
set DirPathIn     "../vector/${Folder}/in"
set DirPathOut    "../vector/${Folder}/out"
set DirPathRef    "../vector/${Folder}/ref"

if { [file exists $DirPathOut] == 0 } {file mkdir $DirPathOut} 
set core_a_ssb_idx 0

# Set the total error count
set TotalErrorCount  0

# Interpret sizes/addresses as decimal (for hex address use the prefix "0x") 
radix D

# List of failed testcases and failures
set Failures {}

# Open report file
set Report_File [open "${ReportPath}" "w"]
puts $Report_File  "Test Case,Test Status"

switchtarget -ResetIndex

proc sync_A_cores {} {
	#set file_num [format "%03d" $batchIdx]
	puts "Loading next batch ========================="
	restore -b "${tcDirPathIn}/inp${file_num}.bin" m:[evaluate #x ippu_buffer] 16bit
	dbg::wait_until "vspa::is_stopped \$test_run_2::core_a_ssb_idx" 60 1000
	
	
	# Resume receive cores:
	mc::go
}



# Debug target
debug $TARGET

var op_mode = $op_mode
if {$TARGET == "nlm_simulator"} then {
	var syncState = 0
	var stateSwitch = 0
	set input_buffer 0x20008000
	if {$CELL_FOLLOW == 1} {
	var cell_id = 106
	}
}
# Initialize debug
dbgInit
    
#=========================== START TESTCASE LOOP =================================
for {set batchIdx 0} {$batchIdx <= 300} {incr batchIdx} {
	
    #Set current testcase
    set TC_num [format "%03d" $batchIdx]
    
    puts "\n"
    puts "#########################################################################################"
    puts "####################################### BATCH $TC_num ###################################"
    puts "#########################################################################################"

    # Input/Output/Reference directories
	set tcDirPathIn  "$DirPathIn"
    set tcDirPathOut "$DirPathOut/TC${TC_num}" 
    set tcDirPathRef "$DirPathRef/TC${TC_num}" 
    if { [file exists $tcDirPathOut] == 0 } {file mkdir $tcDirPathOut}    
    
    radix d
    
    # ============================ Load input data =============================
    # Load input buffers
	set batch_num "${tcDirPathIn}/inp${TC_num}.bin"
	puts $batch_num
	
	#wait 5000
	
	if {$TARGET == "nlm_simulator"} then {
		restore -b "$batch_num" p:[evaluate #x 0x20008000] 16bit
	} else {
		restore -b "$batch_num" p:[evaluate #x inbuff] 16bit
	}
 
    if {$DBG != 0} then {
        puts "###################################";
        puts "########## DEBUG MODE #############";
        puts "###################################";
        puts "Press any key to continue ..."
        gets stdin
    }
    
	if {$batchIdx < $no_iterations} then {
		go
		wait ${Delay_interval}
	} else {
	puts "================Current batch is $batchIdx ====================="
		exit
		break
	}

}

# Store output
save -b p:[evaluate #x outbuff]#128 "${DirPathOut}/output_result.bin" -o 16bit
   
# Verify dot product
set diffResult [diffFiles "${DirPathOut}/output_result.bin" "${DirPathRef}/output_result.bin"]
if {$diffResult != $DIFF_FILE_MATCH} then {
     incr TotalErrorCount
}

# Finish debug process
kill 

# Print failures (if any)
for {set f_ind 0} {$f_ind < [llength $Failures]} {incr f_ind} {
    puts [lindex  $Failures  $f_ind]
}

# Print final error status
passFail $TotalErrorCount

# Close file
close $Report_File

# Send test report to Jenkins
jenkinsSendTestReport $TotalErrorCount
