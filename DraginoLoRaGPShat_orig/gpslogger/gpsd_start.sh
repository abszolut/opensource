#!/bin/sh
sudo gpsd /dev/ttyAMA0 -F /var/run/gpsd.sock
#sudo gpsd /dev/ttyS0 -F /var/run/gpsd.sock
sleep 5
python /home/pi/DraginoLoRaGPShat/gpslogger/gps.py

