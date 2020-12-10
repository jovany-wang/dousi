# Dousi Protocol
Dousi RPC Request协议格式如上:  
| service name str | funcName str |  paramTypeList(64bits)  | array of args  |  
  
第1个item是一个字符串，表示所调用的service名称;  
第2个item是一个字符串，表示所调用的方法名称;  
第3个item是一个64位的数字，表示所期望的形参类型列表, 目前仅支持基础类型;
第4个item是一个tuple来表示调用的实际参数。

Dousi RPC Response协议格式如上:  
｜ error code ｜ return value ｜  
第1个item是一个msgpack int，表示错误码;  
第2个item是返回值。  

