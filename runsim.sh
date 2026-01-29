#!/bin/bash
# This shell script is for running simulations
# Type:
#    ./runsim.sh -h 
# for usage

num_jobs=500
skip_factor=2.0
cw_min=10       # consumer work range
cw_max=30
cds=50          # consumer delayed start (ms)
pde=1000        # init producer delayed end (ms)

: '
do: 
./runsim.sh (noop)
./runsim.sh lockvar 1 500
./runsim.sh peterson 1 500
./runsim.sh sem 1 500

All probably OK then do:

./runsim.sh lockvar 2 500 = violations
./runsim.sh sem 2 500 = no violations
./runsim.sh sem 4 500 = no violations
'

sem_prefix=sem
bwait_prefix=bwait
bin=./bin
mux_type=noop
proc_pairs=1

function usage_exit {
    echo "
usage: $0 [-h] [mutex_type process_pairs num_jobs skip_factor]
    -h prints usage message and exits
    mutex_type should be be one of:
        noop: to use uncontrolled access to shared objects
        lockvar: to use the busy waiting \"lock\" variable mutex
        peterson: to use Peterson's busy waiting solution
        sem: to use a semaphore solution
    process_pairs is the number of produce/consumer pairs and
    should be between 1 and 4.
    num_jobs is the number of jobs for each process to process
    skip_factor is the optional skip factor (2.0 * num_jobs by default_

To specify process_pairs you must also specify mutex_type and to
specify num_jobs you must also specify both mutex_type and
process_pairs.
Default values are:
    mutex_type: $mutex_type
    proc_pairs: 1
    num_jobs: $num_jobs
    skip_factor: $skip_factor

If you specify mutex_type peterson the process_pairs parameter
is ignored becausee Peterson's solution only works for 2 processes.
process_pairs is also restricted to the range 1 to 4.

"
exit
}

if [ "$1" = "-h" ]; then 
    usage_exit
fi

if [ $# -ge 1 ]; then
    mux_type=$1
fi

if [ $# -ge 2 -a $mux_type != "peterson" ]; then 
    proc_pairs=$2
fi

if [ $proc_pairs -lt 1 ]; then
    proc_pairs=1
fi

if [ $proc_pairs -gt 4 ]; then
    proc_pairs=4
fi

if [ $# -ge 3 ]; then
    num_jobs=$3
fi

if [ $# -ge 4 ]; then
    skip_factor=$4
fi

if [ $mux_type = "sem" ]; then
    cons=${bin}/${sem_prefix}_consumer
    prod=${bin}/${sem_prefix}_producer
else
    cons=${bin}/${bwait_prefix}_consumer_${mux_type}
    prod=${bin}/${bwait_prefix}_producer_${mux_type}
fi    

if [ ! -f $cons ]; then
    echo "$cons does not exist"
    usage_exit
fi

if [ ! -f $prod ]; then
    echo "$prod does not exist"
    usage_exit
fi

./runrmsho.sh
rm -rf ./out

echo "
-------------------------------------------------------
running simulation with following parameters
mutex type:                     $mux_type
process pairs:                  $proc_pairs
jobs per process:               $num_jobs
skip factor:                    $skip_factor
consumer work:                  $cw_min to $cw_max
producer work delays:           20 to 40
delay start for consumer:       $cds
delay exit for init producer:   $pde

When the processes have stopped, run the analyis of the 
logs in ./out by entering the following command:

python3 analyselogs.py out/*

or 

python3 analyselogs.py -v out/*
-------------------------------------------------------
"

$prod $num_jobs -i -sf $skip_factor -de $pde &

for ((i = 1; i < $proc_pairs + 1; i++))
do
    $cons $num_jobs -sf $skip_factor -ds $cds -w $cw_min $cw_max &
done

for ((i = 1; i < $proc_pairs; i++))
do
    $prod $num_jobs -sf $skip_factor &
done

