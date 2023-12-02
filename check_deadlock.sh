#!/bin/bash

pid="$(pidof make_deadlock)"
retVal=$?

if ((retVal==1)); then
    echo "Process not found"
    exit 1
fi

# Process probably has a deadlock if 2 or more of its threads are waiting for futex
futex_count=$(ps -p "$pid" -m -o wchan:32 | grep "futex" -c)

if ((futex_count < 2)); then
    echo "Passed (no deadlock)"
else
    echo "Failed (deadlocked)"
fi
