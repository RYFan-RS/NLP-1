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
bool Get_Basic(Request &request, Json::Value &root){
	std::string sTemp;
	bool bRight = true;;
	sTemp = request.get_param("jsonrpc");
	if(sTemp != "2.0"){
		Json::Value jErrorObj;		
		jErrorObj["code"] = -32603;
		jErrorObj["message"] = "jsonrpc版本错误,只支持2.0";
		root["error"] = jErrorObj;
		root["jsonrpc"] = "2.0";
		bRight = false;
	}else {
		root["jsonrpc"] = sTemp; 
	}
	sTemp = request.get_param("id");
	if(sTemp == "null"){
		Json::Value jErrorObj;		
		jErrorObj["code"] = -32603;
		jErrorObj["message"] = "id值错误";		
		root["error"] = jErrorObj;
		bRight = false;	
	}else{
		root["id"] = sTemp;
	}

	return bRight;
}

bool Use_Jieba(Request &request, Json::Value &root){
    std::vector<std::string> vRet;
    std::string sRet;
	std::map<std::string,std::vector<std::string> > subSentence;

	std::string sIn=request.get_param("data");
	if(cppjiebaParser!=NULL){
		(reinterpret_cast<cppjieba::Jieba const*>(cppjiebaParser))->CutForSearchEx(sIn,vRet,subSentence,true);
		for(size_t i = 0;i<vRet.size();i++){
			sRet += "/"+vRet[i];
		}
		root["result"] = sRet;
	}
	else {
		Json::Value jErrorObj;		
		jErrorObj["code"] = -32603;
		jErrorObj["message"] = "jieba分词错误";		
		root["error"] = jErrorObj;
		root["jsonrpc"] = "2.0";

		return false;
	}
	
	return true;
}
void run(Request &request, Json::Value &root){

	if(false == Get_Basic(request,root)){
		return;
	}

	if(false == Use_Jieba(request,root)){
		return;
	}

}
void manage(Request &request, Json::Value &root){
	if(false == Get_Basic(request,root)){
		return;
	}

	root["result"] = "manager_test";
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
