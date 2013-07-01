#!/bin/sh

rm *.out
monitor/bin/darwin-4.2.1/debug/threading-multi/monitor machine.json > monitor.out
sleep 5
cat sink.out | grep "Total elapsed time: " > /dev/null
if [ "$?" == "0" ]
then
	echo "PASSED"
else
	echo "FAILED"
fi
