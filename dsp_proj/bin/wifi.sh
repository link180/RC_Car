#!/bin/sh

ROOTDIR="/home/root"
WORKSPACE="$ROOTDIR/workspace"
WIFISCRIPT="$WORKSPACE/auto_wifi.sh"

echo "Bootstraping Wi-Fi"

if [ -e "$WIFISCRIPT" ]
then
	cd $WORKSPACE
	$WIFISCRIPT
else
	echo "There are no $WIFISCRIPT"
	exit 1
fi

echo "Wi-Fi Bootstraping Success"
