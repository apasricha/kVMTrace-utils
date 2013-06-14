#! /usr/bin/env tcsh

# Check usage.
if (${#argv} != 1) then

    printf "hits-to-misses.sh <workload name>\n"
    exit 1

endif

# Extract command line argument(s).
set workload = ${argv[1]}

# Perform the extraction/summation.
psql --quiet --no-align --tuples-only --field-separator " " --command "select qpos, refs from lru where workload = '${workload}';" kVMTrace \
    | ./hits-to-misses.py \
    > ${workload}.misses
