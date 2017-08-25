#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <stdlib.h>
#include "ehttp/simple_log.h"
#include "ehttp/http_server.h"
#include "ehttp/threadpool.h"
#include "lib4/Url.hpp"
#include "cppjieba/Jieba.hpp"



static void* cppjiebaParser = NULL;

int g_total_num = 4;
pthread_key_t g_tp_key;

void usage(char** argv){
	std::cout<<"USAGE:"<<std::endl;
	std::cout<<argv[0]<<" [ip] [port]"<<std::endl;
	std::cout<<std::endl;
	std::cout<<"EXAMPLE:"<<std::endl;
	std::cout<<argv[0]<<" 192.168.1.2 8080"<<std::endl;
}
void a_test_fn() {
    pthread_t t = pthread_self();
    LOG_INFO("start thread data function , tid:%u", t);
    unsigned long *a = new unsigned long();
    *a = t;

    pthread_setspecific(g_tp_key, a);
}
std::string Use_Jieba(std::string &sIn){
    std::vector<std::string> vRet;
    std::string sRet;
	std::map<std::string,std::vector<std::string> > subSentence;

	if(cppjiebaParser!=NULL){
		(reinterpret_cast<cppjieba::Jieba const*>(cppjiebaParser))->CutForSearchEx(sIn,vRet,subSentence,true);
		for(size_t i = 0;i<vRet.size();i++){
			sRet += "/"+vRet[i];
		}
	}
	return sRet;
}

void Set_Error(std::string message,Request &request, Json::Value &root){
	Json::Value jErrorObj;	
	Json::Value jDataObj;	
	root["id"] = request.get_param("id");
	root["jsonrpc"] = "2.0";
	jErrorObj["success"] = false;
	jErrorObj["message"] = message;
	jErrorObj["data"] = jDataObj;
	root["result"]=jErrorObj;
}
bool Check_Basic(Request &request, Json::Value &root){
	std::string sTemp;
	bool bRight = true;;
	sTemp = request.get_param("jsonrpc");
	if(sTemp != "2.0"){
		Set_Error("jsonrpc版本错误,只支持2.0："+sTemp,request,root);
		bRight = false;
	}
	sTemp = request.get_param("id");
	if(sTemp == "null"){
		Set_Error("id值错误",request,root);	
		bRight = false;	
	}

	return bRight;
}
void Process_Live(Request &request, Json::Value &root){
	Json::Value jResultObj;	
	Json::Value jDataObj;
	
	root["id"] = request.get_param("id");
	root["jsonrpc"] = "2.0";
	jResultObj["success"] = true;
	jResultObj["message"] = "Jieba分词功能正常";
	jDataObj["name"] = "null";
	jDataObj["version"] = "null";
	jDataObj["md5"] = "null";	
	jResultObj["data"] = jDataObj;
	root["result"]=jResultObj;
	return;
}
void Process_List(Request &request, Json::Value &root){
	Json::Value jResultObj;	
	Json::Value jDataObj;
	Json::Value jManageObj;
	
	root["id"] = request.get_param("id");
	root["jsonrpc"] = "2.0";
	jResultObj["success"] = true;
	jResultObj["message"] = "返回接口列表";

	//介绍live接口
	jManageObj["method1"] = "live";
	jManageObj["params1"] = "{jsonrpc=2.0,method=live,params=[],id=1}";
	//介绍List接口
	jManageObj["method2"] = "list";
	jManageObj["params2"] = "{jsonrpc=2.0,method=list,params=[],id=1}";
	//介绍debug接口
	jManageObj["method3"] = "debug";
	jManageObj["params3"] = "{jsonrpc=2.0,method=debug,params=[],id=1}";
	//介绍config接口
	jManageObj["method4"] = "config";
	jManageObj["params4"] = "{jsonrpc=2.0,method=config,params=[],id=1}";
	//介绍process接口
	jManageObj["method5"] = "process";
	jManageObj["params5"] = "{jsonrpc=2.0,method=process,params=[待分词内容],id=1}";
	//介绍test接口
	jManageObj["method6"] = "test";
	jManageObj["params6"] = "{jsonrpc=2.0,method=test,params=[],id=1}";
	
	jDataObj["manage"] = jManageObj;	
	jResultObj["data"] = jDataObj;
	root["result"]=jResultObj;
}
void Process_Debug(Request &request, Json::Value &root){
	Json::Value jResultObj;	
	Json::Value jDataObj;

	root["id"] = request.get_param("id");
	root["jsonrpc"] = "2.0";
	jResultObj["success"] = true;
	jResultObj["message"] = "Jieba分词没有debug开关";	
	jResultObj["data"] = jDataObj;
	root["result"]=jResultObj;	
}
void Process_Config(Request &request, Json::Value &root){
	Json::Value jResultObj;	
	Json::Value jDataObj;

	root["id"] = request.get_param("id");
	root["jsonrpc"] = "2.0";
	jResultObj["success"] = true;
	jResultObj["message"] = "Jieba分词暂时没有config功能";	
	jResultObj["data"] = jDataObj;
	root["result"]=jResultObj;	
}
void Process_Process(Request &request, Json::Value &root){
	Json::Value jResultObj;	
	Json::Value jDataObj;
	std::string sTemp;
	sTemp = request.get_param("params");
	root["id"] = request.get_param("id");
	root["jsonrpc"] = "2.0";
	jDataObj["result"] = Use_Jieba(sTemp);
	jResultObj["success"] = true;
	jResultObj["message"] = "使用Jieba分词";	
	jResultObj["data"] = jDataObj;
	root["result"]=jResultObj;	
}
void Process_Test(Request &request, Json::Value &root){
	Json::Value jResultObj;	
	Json::Value jDataObj;
	std::string sTemp = "你好,很高兴认识你";
	root["id"] = request.get_param("id");
	root["jsonrpc"] = "2.0";
	jDataObj["result"] = Use_Jieba(sTemp);
	jResultObj["success"] = true;
	jResultObj["message"] = sTemp;	
	jResultObj["data"] = jDataObj;
	root["result"]=jResultObj;	
}

bool Get_Run_Method(Request &request, Json::Value &root){
	std::string sTemp;
	sTemp = request.get_param("method");
	if(sTemp=="config"){
		Process_Config(request,root);
	}else if(sTemp=="process"){
		Process_Process(request,root);	
	}else if(sTemp=="test"){
		Process_Test(request,root);
	}else{
		Set_Error("URL错误",request,root);
	}
}
bool Get_Manage_Method(Request &request, Json::Value &root){
	std::string sTemp;
	sTemp = request.get_param("method");
	if(sTemp=="live"){
		Process_Live(request,root);
	}else if(sTemp=="list"){
		Process_List(request,root);	
	}else if(sTemp=="debug"){
		Process_Debug(request,root);
	}else{
		Set_Error("URL错误",request,root);
	}
}

void run(Request &request, Json::Value &root){

	if(false == Check_Basic(request,root)){
		return;
	}

	if(false == Get_Run_Method(request,root)){
		return;
	}
	
}
void manage(Request &request, Json::Value &root){
	if(false == Check_Basic(request,root)){
		return;
	}

	if(false == Get_Manage_Method(request,root)){
		return;
	}
}
void InitCppjieba()
{
	if(cppjiebaParser == NULL) {
		//词典位置
		const char* const DICT_PATH = "../../../data/dict/jieba.dict.utf8";
		const char* const HMM_PATH = "../../../data/dict/hmm_model.utf8";
		const char* const USER_DICT_PATH = "../../../data/dict/user.dict.utf8";
		const char* const IDF_PATH = "../../../data/dict/idf.utf8";
		//创建分词指针
		cppjiebaParser = new cppjieba::Jieba(DICT_PATH,HMM_PATH, USER_DICT_PATH, IDF_PATH);
	}
}

void DestroyCppjieba()
{
	if (NULL != cppjiebaParser)
	{
		delete (cppjieba::Jieba*)cppjiebaParser;
		cppjiebaParser = NULL;
	}
}



bool ProcessInit()
{

	//初始化静态分词引擎
	InitCppjieba();
	return true;
}

int main(int argc, char** argv)
{
	//判断输入参数的正确性
	if(argc != 3) {
		usage(argv);
		return -1;
	}
	
	if (!ProcessInit())
	{
		DestroyCppjieba();
		return -1;
	}

	//起线程池
    pthread_key_create(&g_tp_key,NULL);
    
    ThreadPool tp;
    tp.set_thread_start_cb(a_test_fn);
    tp.set_pool_size(g_total_num);

    HttpServer http_server;
	
    http_server.set_thread_pool(&tp);

	//创建HTTP服务
	http_server.add_bind_ip(argv[1]);
	http_server.add_mapping("/run", run,POST_METHOD);
	http_server.add_mapping("/manage", manage,POST_METHOD);
	
	http_server.set_port(atoi(argv[2]));
	http_server.set_backlog(100000);
	http_server.set_max_events(100000);
	http_server.start_async();
	http_server.join();

	DestroyCppjieba();
	
	return 0;
}
