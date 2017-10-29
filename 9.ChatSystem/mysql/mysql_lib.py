#coding:utf-8
'''
Created on 2017��10��28��

@author:qiujiahao 

@email:997018209@qq.com
'''
import pymysql

class mysql:
    def __init__(self,database='test'):
        #数据库所在的服务器地址（这里是本地主机），登陆用户名，登陆密码，要连接的数据库
        #self.db = pymysql.connect("localhost","love","123456",database)
        self.db = pymysql.connect("localhost","love","123456",database,charset="utf8")
        self.cursor = self.db.cursor() #创建游标对象
    def __del__(self):
        self.db.close() 
    def execute(self,cmd):
        self.cursor.execute(cmd) #执行SQL语句
        data = self.cursor.fetchone() ##获取返回数据
        return data
    
if __name__=="__main__":
    data=mysql()
    print(data.execute("insert into person values(NULL,'邱佳豪','man');"))
    print(data.execute("select * from person;"))
    