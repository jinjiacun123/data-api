# data-api<br/>
1.多进程接受客户端模式；<br/>
2.实时数据请求模式的，单品种请求；<br/>
3.请求与接受包头json长度暂定为50个字节；<br/>
4.历史数据多条请求;<br/>
<br/>
<br/>
<br/>
请求格式:<br/>
	实时请求格式：<br/>
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
	历史数据请求格式:<br/>
json1:  {"type":"000100040001","length":PACKAGE_LENGTH}<br/>
json2:  {"type":"000100040001","data":{"code_type":"CODE_TYPE","code":"CODE","index":INDEX,"size":SIZE}}<br/>
	<br/>
	SIZE:请求返回的条数<br/>
	INDEX:请求的第几个SIZE，从当前时间向前推第几个SIZE<br/>
	<br/>
<br/>
