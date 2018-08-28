#!/bin/sh

ROOTDIR="/home/root"
WORKSPACE="$ROOTDIR/workspace"
REMOTECONTROL="$WORKSPACE/remote_controller"

echo "Kill Wi-Fi Remote Controller"

PID=$(ps -ef | grep remote_controller | grep -v grep | awk '{print $2}')  

echo "Process ID Check OK"
echo $PID 
echo "Check Finish"

sleep 3

if [ "$PID" = "" ]
then
	echo "There are no Server! finish it!"
elif [ $PID -ge 999 ]
then
	kill $(ps aux | grep 'remote_controller' | grep -v grep | awk '{print $2}')
fi

echo "Killing Wi-Fi Remote Controller Success!!!"
