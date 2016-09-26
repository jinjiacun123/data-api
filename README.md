# data-api<br/>
v1.0<br/>
1.多进程接受客户端模式；<br/>
2.实时数据请求模式的，单品种请求；<br/>
3.请求与接受包头json长度暂定为50个字节；<br/>
4.历史数据多条请求;<br/>
v2.0<br/>
改造多进程为多线程模式<br/>
<br/>
<br/>
<br/>
请求格式:<br/>
	<b>实时请求格式</b>：<br/>
json1:	{"type":"000100010001","length":PACKAGE_LENGTH}<br/>
json2:	{"type":"000100010001","data":{"code_type":"CODE_TYPE","code":"CODE"}}<br/>
	<br/>
	PACKAGE_LENGTH:json2的数据的总长度<br/>
	CODE_TYPE:请求的市场编号<br/>
	CODE:请求的品种编号<br/>
	<br/>
	数据流顺序，json1在json2前面<br/>
	json1的总长度为50，实际数据不足50后面追加\0，补充完为50字节为止<br/>
<br/>
<br/>
	<b>分时数据请求格式</b>:<br/>
json1:  {"type":"000100030001","length":PACKAGE_LENGTH}<br/>
json2:  {"type":"000100030001",'data":{"code_type":"CODE_TYPE","code":"code"}}<br/>
	<br/>	
<br/>
<br/>
	<b>历史数据请求格式</b>:<br/>
json1:  {"type":"000100040001","length":PACKAGE_LENGTH}<br/>
json2:  {"type":"000100040001","data":{"code_type":"CODE_TYPE","code":"CODE","index":INDEX,"size":SIZE,"circle":"CIRCLE"}}<br/>
	<br/>
	SIZE:请求返回的条数<br/>
	INDEX:请求的第几个SIZE，从当前时间向前推第几个SIZE<br/>	
	CIRCLE:请求的周期(minite_1,minite_5,minite_15,minite_30,minite_60,minite_120,minite_180,minite_240,day,5day,week,month)
	<br/>
<br/>
