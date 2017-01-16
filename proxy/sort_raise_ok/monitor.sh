while true
	do
	procnum=`ps -ef|grep "main"|grep -v grep|wc -l`
	if [ $procnum -eq 0 ]; then
	        echo 'test'
		sh /home/jim/source_code/data-api/proxy/kill.sh
		/home/jim/source_code/data-api/proxy/jim_proxy
	        python /home/jim/source_code/data-api/proxy/sort_raise_ok/txt/my_read.py
		/home/jim/source_code/data-api/proxy/sort_raise_ok/main &
		echo 'start ...'
	fi
	sleep 30
	echo 'sleep'
done
