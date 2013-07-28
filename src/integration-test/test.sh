#!/bin/sh

rm -rf log/*.log
rm -rf results.json
dist/monitor integration-test/test.json
sleep 1

# sink finds everything
cat results.json | grep "elapsed" > /dev/null
if [ "$?" != "0" ]
then
	echo "FAILED (results)"
	exit 1
fi

# at least a little work for each task.
cat log/run.log | grep "WorkSleep\[0\]" > /dev/null
if [ "$?" != "0" ]
then
	echo "FAILED (work0)"
	exit 1
fi
cat log/run.log | grep "WorkSleep\[1\]" > /dev/null
if [ "$?" != "0" ]
then
	echo "FAILED (work1)"
	exit 1
fi
cat log/run.log | grep "WorkSleep\[2\]" > /dev/null
if [ "$?" != "0" ]
then
	echo "FAILED (work2)"
	exit 1
fi

echo "PASSED"
exit 0
