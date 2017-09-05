#!/bin/bash
sudo systemctl restart network-manager.service 
#nmcli networking off 
#nmcli networking on
#nmcli radio wifi off && sleep 5 && nmcli radio wifi on
#sudo modprobe -r iwlwifi && sudo modprobe iwlwifi
#echo "off" > /home/abszolut/app/speaker_switch/speaker.log
sleep 3
#sudo curl -X POST  -H "Content-Type: application/json" -d "{\"value1\":\"off\"}" https://maker.ifttt.com/trigger/speaker_on/with/key/neVVqg3UhlzDVPWArtzj_pA_ZM0tXWLmmZHry0G7BgT 2>&1 | tee /home/abszolut/app/speaker_switch/script.log
sudo curl -X POST  -H "Content-Type: application/json" -d "{\"value1\":\"off\"}" https://maker.ifttt.com/trigger/speaker_on/with/key/neVVqg3UhlzDVPWArtzj_pA_ZM0tXWLmmZHry0G7BgT 2>&1




