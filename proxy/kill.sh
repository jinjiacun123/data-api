ps -ef | grep jim_proxy | grep -v grep|cut -c 9-15|xargs kill -9
