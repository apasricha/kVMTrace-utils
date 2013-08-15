#! /bin/tcsh -f

# Check usage.
if (${#argv} != 2) then

    printf "USAGE: run.sh <traces directory> <trace name>\n"
    exit 1

endif

# Give the arguments names.
set tracesDirectory = ${argv[1]}
set traceName = ${argv[2]}

# Set some other useful variables for clarity.
set policy = sfhk_lru
set projectDirectory = ${RESEARCH_HOME}/page-replacement-project
set simulatorDirectory = ${projectDirectory}/simulators/${policy}
set simulatorPathname = ${simulatorDirectory}/lru-simulator.${HOSTTYPE}
set accumulatorPathname = ${simulatorDirectory}/hits-to-misses.${HOSTTYPE}
set sqlCommander = ${simulatorDirectory}/results-to-sql.awk
set tracePathname = ${tracesDirectory}/${traceName}/trace.gz
set databaseName = prp
set SQLServer = temp1.cs-res.amherst.edu

# Run the simulator/accumulator, inserting the results into the database.
printf "Running simulator and sending results to database..."
gunzip < ${tracePathname} \
       | ${simulatorPathname} \
       | ${accumulatorPathname} \
       | awk --assign trace_name=${traceName} --assign policy=${policy} -f ${sqlCommander} \
       | psql -h ${SQLServer} ${databaseName}
if (${status} != 0) then
    printf "ERROR: Failure in simulation or database insertion\n"
    exit 1
endif
printf "done.\n"

printf "SUCCESS\n"
