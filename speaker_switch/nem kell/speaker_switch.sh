#!/bin/bash

echo "on" > /home/abszolut/app/speaker_switch/speaker.log
echo "curl -X POST  -H \"Content-Type: application/json\" -d \"{\\\"value1\\\":\\\"on\\\"}\" https://maker.ifttt.com/trigger/speaker_on/with/key/neVVqg3UhlzDVPWArtzj_pA_ZM0tXWLmmZHry0G7BgT" > /home/abszolut/app/speaker_switch/speaker.sh



