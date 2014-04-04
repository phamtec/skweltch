#!/bin/sh

INSTALL_DIR=../buildsys

rm -rf $INSTALL_DIR

if [ ! -d $INSTALL_DIR ]
then
	mkdir $INSTALL_DIR
fi

cp ../bin/blockinfo $INSTALL_DIR
cp ../bin/libbuildLib.so $INSTALL_DIR
cp ../bin/libconfigLib.so $INSTALL_DIR
cp ../bin/libcoreLib.so $INSTALL_DIR
cp ../bin/libmachineLib.so $INSTALL_DIR
cp ../bin/libmonitorLib.so $INSTALL_DIR
cp ../bin/libmsgLib.so $INSTALL_DIR
cp ../bin/listen $INSTALL_DIR
cp ../bin/machine $INSTALL_DIR
cp ../bin/machinecmd $INSTALL_DIR
cp ../bin/mockblock $INSTALL_DIR
cp ../bin/polldir $INSTALL_DIR
cp ../bin/pollgit $INSTALL_DIR
cp ../bin/send $INSTALL_DIR
cp ../bin/sinkbuild $INSTALL_DIR
cp ../bin/workbash $INSTALL_DIR
# cp -R "../bin/Skweltch Notifier.app" $INSTALL_DIR

cp build.json $INSTALL_DIR
