#! /bin/bash
for (( ; ; ))
do
    amixer set PCM -- 95%
    vlc --play-and-exit "/home/pi/Desktop/Chanting/DS/DS1 01.wav"
    vlc --play-and-exit "/home/pi/Desktop/Chanting/DS/DS1 06.wav"
    amixer set PCM -- 100%
    vlc --play-and-exit "/home/pi/Desktop/Chanting/LA/LA 01.wav"
    vlc --play-and-exit "/home/pi/Desktop/Chanting/most/Maharishi Vedic Pandits chanting Shri Ganesh Atharvashirsha.m4a"
done