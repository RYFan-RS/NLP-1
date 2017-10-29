#coding:utf-8
'''
Created on 2017年10月28日

@author:qiujiahao 

@email:997018209@qq.com
'''
from semantic.semantic import semantic

class chat:
    def __init__(self,helper):
        self.helper=helper#数据处理
        self.quest=''#当前问题
        self.ans=''#当前答案
    def set_quest(self,quest):
        self.quest=quest
    def set_ans(self,ans):
        self.ans=ans
    def start_to_chat(self,quest):
        self.quest=quest
        if self.helper.root_conf['mode']=='1':
            #进入深度学习加语义理解模式
            self.start_deep_semantic()
        
    
    def start_deep_semantic(self):
        #解析kb文件
        self.helper.parse_kb()
        #先匹配语义理解
        ans=semantic.parse(self.quest)
    
               