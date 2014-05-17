#!/bin/bash

# *  $Revision$
# *  $Date$
# *  $Author$
# *  $HeadURL$

killall rl_glue 2>/dev/null
killall test_message_agent 2>/dev/null
killall test_message_environment 2>/dev/null
killall test_message_experiment 2>/dev/null
sleep 1
rl_glue &
sleep 1
./test_message_agent &
sleep 1
./test_message_environment &
sleep 1
./test_message_experiment

test_outcome=$?
exit $test_outcome