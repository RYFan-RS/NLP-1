#coding:utf-8
'''
Created on 2017年10月9日

@author: qiujiahao

@email:997018209@qq.com

'''
import requests

s = requests

data={"quest":"邱佳豪"}
r = s.post('http://127.0.0.1:8000/my_love', data)

print (r.status_code)
print (r.headers['content-type'])
r.encoding = 'utf-8'
print (r.text)