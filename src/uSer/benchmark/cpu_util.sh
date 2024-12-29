#!/bin/bash

#CPU core pattern should be as follows:
# "^%Cpu3 "   --> for one CPU
#"^%Cpu0 \|^%Cpu1 \|^%Cpu2 \|^%Cpu3 " --> for multiple CPUs

if [ "$#" -ne 4 ] ; then
        echo "usage ./cpu_util.sh <no_of_sim_time_in_secs> <CPU core pattern> <process_id> <output file>"
        exit 1
fi
#ntimes=$(( $2 * 60 ))
ntimes=$1
out_file=$4
tmp_file="tmp"
#process_id=`pidof $3`
process_id=$3
#echo "PID: " $process_id
if [ -e $tmp_file ]; then  
	rm -f $tmp_file
fi

#top -bn $ntimes -p $3 -d 0.001 >> $tmp_file
top -bn $ntimes -p $process_id -d 1 >> $tmp_file

patt=$2
total_cpu=`cat $tmp_file| grep "$patt" |tr ',' ' ' |tr -s ' '|cut -d ' ' -f9 | awk '{s=100-$1; print s}'|awk '{s+=$1;}END{print s}'`
avg_cpu=$( echo "$total_cpu / $ntimes" | bc -l )

total_mem=`cat $tmp_file|grep $3|tr -s ' '|cut -d ' ' -f7|awk '{s+=$1;}END{print s}'`
avg_mem=$( echo "$total_mem / $ntimes" | bc -l )

total_wait=`cat $tmp_file| grep "$patt" |tr -s ':,' ' ' |cut -d ' ' -f10 | awk '{s+=$1;}END{print s}'`
avg_wait=$( echo "$total_wait / $ntimes" | bc -l )

if [ ! -f $out_file ]; then  
	echo "Avg%CPU AvgMem(KiB) Avg%Wait" > $out_file;
fi
printf "%0.2f %0.2f %0.2f\n" $avg_cpu $avg_mem $avg_wait>> $out_file
