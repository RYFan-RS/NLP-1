#coding:utf-8
import urllib
import sys
import os
import time
import pymysql
from mysql.mysql_lib import mysql
from http.server import BaseHTTPRequestHandler
from http.server import HTTPServer
from socketserver import ThreadingMixIn
from data_hlep.helper import helper
from chat.chat import chat

if sys.argv[1:]:
    hostIP = ''
    portNum= int(sys.argv[1])
else:
    hostIP = ''
    portNum = 8000 
        
class myServer(BaseHTTPRequestHandler):      
    chat_num = 0
    @classmethod
    def count(cls):
        cls.chat_num += 1
        return cls.chat_num       
                   
    def do_GET(self):
        try:
            self.send_response(200, message =None)
            self.send_header('Content-type','text/html')
            self.end_headers()
            res ='鏆傛椂鍙敮鎸丳OST璇锋眰!!!'
            self.wfile.write(res.encode(encoding ='utf_8', errors ='strict'))
        except IOError:
            self.send_error(404, message =None)

    def do_POST(self):
        try:
            answer="成功" 
            begin_time = time.clock()
            self.send_response(200, message =None)
            self.send_header('Content-type','text/html')
            self.end_headers()       
            length = int(self.headers['Content-Length'])
            post_data = urllib.parse.parse_qs(self.rfile.read(length).decode('utf-8'))           
            #quest = post_data['quest']
            if self.path == "/my_love":
                quest = post_data['quest']
                chat.start_to_chat(quest[0])
                self.wfile.write(answer.encode(encoding ='utf_8', errors ='strict'))
            else:
                self.send_error(404, message =None)
            print("chat_num:%d,http cost time:%s"%(self.count(),time.clock()-begin_time))
        except IOError:
            self.send_error(404, message =None)

class ThreadingHttpServer(ThreadingMixIn,HTTPServer):
    pass


def start_http_server():      
    myServer = ThreadingHttpServer((hostIP,portNum), myServer)
    print("Start to listen on:%s:%d"%(myServer.server_name,myServer.server_port))
    myServer.serve_forever()
    myServer.server_close() 
             
def init_chat(helper):
    return chat(helper)

if __name__=='__main__':  
    #数据预处理
    helper=helper()
    #启动聊天机器人
    chat=init_chat(helper)
    #启动http服务 
    start_http_server()


