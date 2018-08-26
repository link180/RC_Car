#!/bin/sh

echo "Wi-Fi Remote Controller Activation!"

PID=$(ps -ef | grep remote_controller | grep -v grep | awk '{print $2}')  

echo $PID 
