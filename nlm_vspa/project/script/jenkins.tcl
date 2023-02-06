# ==============================================================================
#! @file            jenkins.tcl
#! @brief           Library of TCL functions for Jenkins automated testing.
#! @copyright       Copyright (C) 2012-2013 Freescale Semiconductor, Ltd.
# ==============================================================================
# NXP Confidential. This software is owned or controlled by NXP and may only be used strictly
# in accordance with the applicable license terms. By expressly accepting
# such terms or by downloading, installing, activating and/or otherwise using
# the software, you are agreeing that you have read, and that you agree to
# comply with and are bound by, such license terms. If you do not agree to
# be bound by the applicable license terms, then you may not retain,
# install, activate or otherwise use the software.

# Overall parameters
set JENKINS_MESSAGE_FILE  "run.watchdog"
set JENKINS_REPORT_FOLDER "report" 

# ------------------------------------------------------------------------------
#! @brief           Verify if Jenkins framwork is present or not.   
#!                  If present then the "JENKINS_URL" environment variable is defined.                
#! @return          0/1 for running without/with Jenkins.
# ------------------------------------------------------------------------------
proc jenkins { } {
    global env
    return [info exists env(JENKINS_URL)]
}

# ------------------------------------------------------------------------------
#! @brief           Jenkins initializations.                
# ------------------------------------------------------------------------------
proc jenkinsInit { } {
    # Detect if running with Jenkins
    if { [ jenkins ] == 1 } {
        global JENKINS_REPORT_FOLDER
        # If report directory does not exist then create
        if { [file exists $JENKINS_REPORT_FOLDER] == 0 } {file mkdir $JENKINS_REPORT_FOLDER} 
        # Open log files
        log c  "${JENKINS_REPORT_FOLDER}\\command.log"
        log s  "${JENKINS_REPORT_FOLDER}\\session.log"
        # Pretty print
        puts "#########################################################################################"
        puts "                                 RUNNING WITH JENKINS                                    "
        puts "#########################################################################################"
    } else {
        puts "Jenkins framework not detected."
    }
}

# ------------------------------------------------------------------------------
#! @brief           Send a messaje to Jenkins.
#! @param[in]       message    (optional) Message to send to Jenkins (character string).
#! @param[in]       folderPath (optional) Folder path where the message is written.                         
#! @return          Void.
#!
#! This function should be called periodically in order to report to Jenkins that 
#! the TCL based testing is still running and did not blocked somewhere (to avoid timeout).
# ------------------------------------------------------------------------------
proc jenkinsSendMessage { {message "running"}} {
    if { [ jenkins ] == 0} { return }
    global JENKINS_MESSAGE_FILE
    set jenkins_file [open $JENKINS_MESSAGE_FILE "w"]
    puts -nonewline $jenkins_file $message
    close $jenkins_file
}

# ------------------------------------------------------------------------------
#! @brief           Send the test report to Jenkins.
#! @param[in]       errorCount    Overall testing error count (number).
#! @param[in]       folderPath    (optional) Folder folderPath where the report is written. 
#! @return          Void.
#!
#! This function should be called at the end of the testing to report to 
#! Jenkins the overall error count.
# ------------------------------------------------------------------------------
proc jenkinsSendTestReport { errorCount } {
    if { [ jenkins ] == 0} { return }
    global JENKINS_MESSAGE_FILE
    set jenkins_file [open $JENKINS_MESSAGE_FILE "w"]
    if {$errorCount == 0} then {
        puts -nonewline $jenkins_file "finished passed"
    } else {
        puts -nonewline $jenkins_file "finished failed" 
    }
    close $jenkins_file
    # Print
    puts "Jenkins testing done ..."
    # Close all log files
    log off all
}

# ------------------------------------------------------------------------------
#! @brief           Write a report for Jenkins.
#! @param[in]       fileName      File name to write to.
#! @param[in]       report        Text to report.
#! @param[in]       folderPath    (optional) Folder folderPath where the report is written. 
#! @return          Void.
#!
#! This function writes a report file which Jenkins sends to the end user when testing is done.
# ------------------------------------------------------------------------------
proc jenkinsWriteReport { fileName report } {
    if { [ jenkins ] == 0} { return }
    global JENKINS_REPORT_FOLDER
    # If report directory does not exist then create
    if { [file exists $JENKINS_REPORT_FOLDER] == 0 } {file mkdir $JENKINS_REPORT_FOLDER} 
    # If file exists then append, otherwise create
    set filePath "${JENKINS_REPORT_FOLDER}\\${fileName}"
    if { [file exists $filePath] == 0 } {
        set jenkins_file [open $filePath "w"]
    } else {
        set jenkins_file [open $filePath "a"]
    } 
    puts $jenkins_file $report
    close $jenkins_file
}


