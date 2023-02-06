# Debug flag (0 or 1)                   
set DBG                    0

# Use Board or Simulator
set USE_BOARD			   0

# Launch configuration to debug/run
if {$USE_BOARD == 1} then {
	set TARGET "nlm_tester"
	set Delay_interval 2000
} else {
	set TARGET "nlm_simulator"
	set Delay_interval 1000
}

puts "${TARGET} in use"

#Unit testing mode, choose only one
set CELL_SEARCH			   1
set CELL_FOLLOW			   0
set MIB_DECODE			   0

# Testcase range to run (inclusive)    
set TC_NUM_START           0            
set TC_NUM_END             0