#!/bin/sh

#min 120, max 300

(echo "y"; echo "1837"; echo "n"; echo "1"; echo "1"; echo "y"; echo " ") | /usr/sbin/wlconf/configure-device.sh

sleep 3

#/usr/share/wl18xx/ap_start.sh
#./ap_start.sh
#perl test.pl
perl ap_start.pl
#/usr/share/wl18xx/ap_start.sh
