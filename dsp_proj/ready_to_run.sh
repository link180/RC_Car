#!/bin/bash

gcc dsp_wifi_can_serv.c -o remote_controller

cd ocv
g++ camera_recorder.cpp -o cam_record $(pkg-config opencv --libs)
cd ..

cp -r project_* /usr/share/matrix-gui-2.0/apps/
chmod -R 777 bin
cp bin/* /usr/bin/

mv /usr/share/wl18xx/hostapd.conf /usr/share/wl18xx/hostapd.conf.bak
mv /usr/share/wl18xx/wpa_supplicant.conf /usr/share/wl18xx/wpa_supplicant.conf.bak

cp wl18xx_config/* /usr/share/wl18xx/
