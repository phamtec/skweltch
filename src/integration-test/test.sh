#!/bin/sh

rm *.log
rm results.json
dist/monitor integration-test/test.json
sleep 3

# sink finds everything
cat results.json | grep "elapsed" > /dev/null
if [ "$?" != "0" ]
then
	echo "FAILED (results)"
	exit 1
fi

# at least a little work for each task.
cat work0.log | grep "...." > /dev/null
if [ "$?" != "0" ]
then
	echo "FAILED (work0)"
	exit 1
fi
cat work1.log | grep "...." > /dev/null
if [ "$?" != "0" ]
then
	echo "FAILED (work1)"
	exit 1
fi
cat work2.log | grep "...." > /dev/null
if [ "$?" != "0" ]
then
	echo "FAILED (work2)"
	exit 1
fi

echo "PASSED"
exit 0
