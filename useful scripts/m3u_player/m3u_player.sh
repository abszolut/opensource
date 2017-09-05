#! /bin/bash
cat data.txt | while read LINE
do
    #echo $LINE
    if [ "$LINE" = "9" ]
    then
        echo "1" > data.txt
    else
        echo "$(($LINE+1))" > data.txt
    fi
    amixer set PCM -- 85%
    echo playing $LINE.m3u
    audacious $LINE.m3u -q
    amixer set PCM -- 100%
done


