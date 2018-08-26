#!/bin/sh

ROOTDIR="/home/root"
WORKSPACE="$ROOTDIR/workspace"
REMOTECONTROL="$WORKSPACE/remote_controller"

echo "Wi-Fi Remote Controller Activation!"

#PID=$(ps -ef | grep remote_controller | grep -v grep | awk '{print $2}')  

echo "Process ID Check OK"
echo $PID 
echo "Check Finish"

sleep 3

if [ "$PID" = "" ]
then
	echo "There are no Server! Just do it!"
elif [ $PID -ge 999 ]
then
	kill $(ps -ef | grep 'remote_controller' | grep -v grep | awk '{print $2}')
fi

echo "Server On!"

if [ -e "$REMOTECONTROL" ]
then
	cd $WORKSPACE
	$REMOTECONTROL 7777 &
else
	echo "There are no $REMOTECONTROL"
	exit 1
fi

echo "Wi-Fi Remote Controller Operation Success!!!"
