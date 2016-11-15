# data-api<br/>
v1.0<br/>
<b>代理模式实现</b><br/>
直接原始数据代理<br/>
1.多进程接受客户端模式；<br/>
2.实时数据请求模式的，单品种请求；<br/>
3.请求与接受包头json长度暂定为50个字节；<br/>
4.历史数据多条请求;<br/>
5.use poll deal data;<br/>
6.use pthread condition controll auto_push data<br/>
7.sort data by pipe<br/>
<br/>
<br/>
<br/>
请求格式:<br/>
	<b>实时请求格式</b>：<br/>
	<br/>
<br/>
	<b>分时数据请求格式</b>:<br/>
	<br/>
<br/>
<br/>
	<b>历史数据请求格式</b>:<br/>
	<br/>
<br/>
