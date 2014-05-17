#!/bin/bash
#
# Runs a specified example with a selected Lisp implementation
# and prints the result to the standard output.
#
# $Revision$
# $Date$

if [ ${#} -ne 2 ]; then
    echo "Usage: ${0} <lisp-implementation> <example>"
    echo "Example: ${0} sbcl mines-sarsa"
    exit -1
fi

tooldir="`dirname ${0}`"
source "${tooldir}/common.sh"

lispimpl="${1}"
example="${2}"

###############################################################################

load_lisp_config ${lispimpl}

###############################################################################

${LISP} <<- EOF
    `lisp_init`
    `lisp_compile_asdf :rl-glue-examples`
    `lisp_quit`
EOF

###############################################################################

execute_rl_glue

{
${LISP} <<- EOF
  `lisp_init`
  `lisp_load_asdf :rl-glue-examples`
  (rl-glue-${example}:start-agent)
  `lisp_quit`
EOF
} &

{
${LISP} <<- EOF
  `lisp_init`
  `lisp_load_asdf :rl-glue-examples`
  (rl-glue-${example}:start-environment)
  `lisp_quit`
EOF
} &

${LISP} <<- EOF
  `lisp_init`
  `lisp_load_asdf :rl-glue-examples`
  (rl-glue-${example}:start-experiment)
  `lisp_quit`
EOF

exit 0

