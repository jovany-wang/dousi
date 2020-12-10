# Dousi Protocol

## 1. 问题描述

## 2. 协议格式
| service name str | funcName str |  paramTypeList(64bits)  | array of args  |  

Dousi RPC Request协议格式如上，  
第1个item是一个字符串，表示所调用的service名称;  
第2个item是一个字符串，表示所调用的方法名称;  
第3个item是一个64位的数字，表示所期望的形参类型列表, 目前仅支持基础类型;
第4个item是一个tuple来表示调用的实际参数。

## 3.
