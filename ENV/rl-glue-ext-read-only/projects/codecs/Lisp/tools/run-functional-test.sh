#!/bin/bash
#
# Runs a specified functional test with a selected Lisp implementation
# and prints the result to the standard output.
#
# $Revision: 862 $
# $Date: 2009-08-14 16:47:52 +0300 (Fri, 14 Aug 2009) $

if [ ${#} -ne 2 ]; then
    echo "Usage: ${0} <lisp-implementation> <testcase-name>"
    echo "Example: ${0} sbcl test-empty"
    exit -1
fi

tooldir="`dirname ${0}`"
source "${tooldir}/common.sh"

lispimpl="${1}"
testname="${2}"

###############################################################################

load_lisp_config ${lispimpl}
load_functional_test_config ${testname}

###############################################################################

${LISP} <<- EOF
    `lisp_init`
    `lisp_compile_asdf :rl-glue-tests`
    `lisp_quit`
EOF

###############################################################################

execute_rl_glue

{
${LISP} <<- EOF
  `lisp_init`
  `lisp_load_asdf :rl-glue-tests`
  (rl-glue-tests:start-${AGENT})
  `lisp_quit`
EOF
} &

{
${LISP} <<- EOF
  `lisp_init`
  `lisp_load_asdf :rl-glue-tests`
  (rl-glue-tests:start-${ENVIRONMENT})
  `lisp_quit`
EOF
} &

${LISP} <<- EOF 
  `lisp_init`
  `lisp_load_asdf :rl-glue-tests`
  (rl-glue-tests:start-${EXPERIMENT})
  `lisp_quit`
EOF

exit 0

