#!/bin/bash

# *  $Revision$
# *  $Date$
# *  $Author$
# *  $HeadURL$

killall rl_glue 2>/dev/null
killall test_empty_agent 2>/dev/null
killall test_empty_environment 2>/dev/null
killall test_empty_experiment 2>/dev/null
sleep 1
rl_glue &
sleep 1
./test_empty_agent &
sleep 1
./test_empty_environment &
sleep 1
./test_empty_experiment

test_outcome=$?
exit $test_outcome