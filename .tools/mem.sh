#!/bin/sh
#
# Simple script to monitor memory usage of a process 
# usage: ./mem.sh <process_name>

while [ 1 ]
do
clear
ps -o rss,vsz,cmd -C $1 | awk '
NR==1 { next }
{
    printf "==========================\n"
    printf " Memory Usage\n"
    printf "==========================\n"
    printf " Resident : %.2f MB\n", $1/1024
    printf " Virtual  : %.2f MB\n", $2/1024
}'
sleep 2.5
done