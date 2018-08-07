#!/bin/sh

(echo "y"; echo "1837"; echo "n"; echo "1"; echo "1"; echo "y") | /usr/sbin/wlconf/configure-device.sh

#/usr/share/wl18xx/ap_start.sh
#./ap_start.sh
perl test.pl
