#!/bin/sh

#set -x

. "$LIBMESH_DIR"/examples/run_common.sh

example_name=reduced_basis_ex4

example_dir=examples/reduced_basis/$example_name

options="-online_mode 0"
run_example "$example_name" "$options"

options="-online_mode 1"
run_example "$example_name" "$options"
