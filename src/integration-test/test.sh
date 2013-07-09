#!/bin/sh

rm *.out
dist/monitor integration-test/test.json > monitor.out
sleep 3

# sink finds everything
cat sink.out | grep "Total elapsed time: " > /dev/null
if [ "$?" != "0" ]
then
	echo "FAILED"
	exit 1
fi

# at least a little work for each task.
cat task0.out | grep "...." > /dev/null
if [ "$?" != "0" ]
then
	echo "FAILED"
	exit 1
fi
cat task1.out | grep "...." > /dev/null
if [ "$?" != "0" ]
then
	echo "FAILED"
	exit 1
fi
cat task2.out | grep "...." > /dev/null
if [ "$?" != "0" ]
then
	echo "FAILED"
	exit 1
fi

echo "PASSED"
exit 0
