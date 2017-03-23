sleep 20
cd /home/pi/DraginoLoRaGPShat/gpslogger
./gpsd_start.sh &
cd /home/pi/DraginoLoRaGPShat/examples/thethingsnetwork-send-v1/
./thethingsnetwork-send-v1