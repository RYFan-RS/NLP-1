#coding:utf-8
'''
Created on 2017��10��29��

@author:qiujiahao 

@email:997018209@qq.com
'''
import os
import  xml.dom.minidom as xml
from xml.etree import ElementTree as ET

class helperss:
    def __init__(self,conf_path='data/conf/root.conf'):
        self.conf_path=conf_path
        self.root_conf={}#根配置文件
        self.kb={}#知识库
        self.parse_conf()
    #解析配置文件    
    def parse_conf(self):
        with open(self.conf_path,'w',encoding='utf-8') as f:
            for line in f.readlines():
                line=line.strip()
                if(len(line)==0 or line=='#'):
                    continue
                if line.startwith('mode'):
                    self.root_conf['mode']=line.strip('=')[1]
                elif(line.startwith('kb')):
                    self.root_conf['kb']=line.strip('=')[1].strip(';')
    #解析知识库
    def parse_kb(self):
        #逐个解析，顺序越靠前的知识库越优先
        if('kb' not in self.root_conf or len(self.root_conf['kb'])==0):
            print("知识库kb配置不存在或者配置错误")
        for kb in self.root_conf['kb']:
            if False ==self.parse_xml_kb(kb):
                return False
            
    def parse_xml_kb(self,kb_path):
        #打开xml
        dom=xml.parse(kb_path)  
        #得到文档元素对象
        root = dom.documentElement()
        if('false'==root.getElementsByTagName('semantic')[0].firstChild.data):
            print("%s文件不进行语义解析"%(kb_path))
            return False
        type=root.getElementsByTagName('type')[0].firstChild.data
        if(type=='FAQ'):
            self.parse_FAQ_XML(root,kb_path)
        elif(type=='TREE'):
            self.parse_TREE_XML(root,kb_path)
    
    def parse_FAQ_XML(self,root,kb_path):           
        name=root.getElementsByTagName('name')[0].firstChild.data
        quests = root.getElementsByTagName('quest')
        answers = root.getElementsByTagName('answer')
        if(len(quests)!=len(answers)):
            print("%s文件钟问题与答案数量不匹配，配置错误"%(kb_path))
            return False
        for i in range(len(quests)):
            quest=quests[i].firstChild.data
            answer=answers[i].firstChild.data
            if(name not in self.kb):
                self.kb[name]=[quest+':'+answer]
            else:
                self.kb[name].append(quest+':'+answer)
    def parse_TREE_XML(self,root,kb_path):
        per=ET.parse(kb_path) 
        p=per.findall('./Tree') 
        for oneper in p:
            for child in oneper.getchildren():
                print child.tag,':',child.text
            
        
            
        
                
        
