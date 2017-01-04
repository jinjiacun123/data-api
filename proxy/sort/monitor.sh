#!/bin/bash
while true
	do
	procnum=`ps -ef|grep "main"|grep -v grep|wc -l`
	if [ $procnum -eq 0 ]; then
		/home/jim/source_code/data-api/proxy/sort/main &
	fi
	sleep 30
done
