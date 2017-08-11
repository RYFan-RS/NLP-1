#include "SemanticSearchService.h"
#include "Common/lib4/IO.hpp"
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>
#include "Common/libxml2/XmlParser.hpp"
#include "Common/lib4/File.hpp"
#include <stdlib.h>

#ifndef PRINT_LOG
#define PRINT_LOG
#endif
#include "Common/lib5/WHL_LogInstance.hpp"
whl::mutex_lock whl::logger::m_lock;
whl::logger* whl::logger::m_instance = NULL;

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using boost::shared_ptr;

bool writeContext(std::string& sRequestXml, const std::string& sContext){
	robosay::XmlParser requestXml;
	if(requestXml.parse(sRequestXml) == false){
		ELOGGER->print(true, FAILE_CALL_PARAM("writeContext","[requestXml.parse(sRequestXml) == false]", sRequestXml.c_str()));
		return false;
	}
	requestXml.setXPathCDataValue("//chat/Context",sContext);
	sRequestXml = requestXml.toStr();
	return true;
}

bool readContext(const std::string& sResponseXml, std::string& sContext){
	robosay::XmlParser responseXml;
	if(responseXml.parse(sResponseXml) == false){
		ELOGGER->print(true, FAILE_CALL_PARAM("readContext","[responseXml.parse(sResponseXml) == false]", sResponseXml.c_str()));
		return false;
	}
	sContext = responseXml.getXPathValue("//chat/robot_info/context");
	return true;
}

std::string sKBName="";
void testSemanticList(SemanticSearchServiceClient& client){
std::string sXmlRequest =
"<SemanticListRequest>\
  <SessionID>test111111</SessionID>     <!--32位sessionID-->\
  <NewFlag>true</NewFlag>        <!--true:新建的知识库 false:已有的知识库-->\
  <KBType>FAQ</KBType>          <!--0:FAQ 1:Table 2:Line 3:Tree-->\
  <KBIndustry>0</KBIndustry>  <!--0:机器人简历 1:公司简介 0:机票查询预订 1:酒店预订 2:旅行服务 3: 3:Chat-->\
  <KBName>" + sKBName + ".xml</KBName>          <!--64位知识库名称-->\
  <Input></Input>            <!--128位用户输入-->\
</SemanticListRequest>";
std::cout<<"SemanticListRequest:"<<sXmlRequest<<std::endl;
std::string sXmlResponse;

//client.getSemanticList(sXmlResponse, sXmlRequest);

std::cout<<sXmlResponse<<std::endl;

}

void testChat(SemanticSearchServiceClient& client){
	acl::File fIn,fOut;
	std::string sContext;
	std::string sXmlRequest,sXmlResponse;
	fIn.read("test.txt");
	for(size_t i=0; i < fIn.size(); i++){
		acl::Line sXmlRequest = fIn[i];
		if(sXmlRequest.empty() || sXmlRequest.isBeginWith("#"))
		{
			ELOGGER->print(true, FAILE_CALL_PARAM("testChat","[sXmlRequest.empty() || sXmlRequest.isBeginWith('#')]", sXmlRequest.c_str()));
			continue;
		}
		if(writeContext(sXmlRequest,sContext) == false){
			ELOGGER->print(true, "testChat [writeContext(sXmlRequest,sContext) == false] Write context failed!\nsXmlRequest[%s]\nsContext[%s]\n", sXmlRequest.c_str(),sContext.c_str());
			continue;
		}

		std::cout<<"--INPUT-------------------------------------------"<<std::endl;
		std::cout<<sXmlRequest<<std::endl<<std::endl;
		client.engineChat(sXmlResponse, sXmlRequest);
		if(readContext(sXmlResponse,sContext) == false){
			ELOGGER->print(true, "testChat [readContext(sXmlResponse,sContext) == false] Read context failed!\nsXmlResponse[%s]\nsContext[%s]\n", sXmlResponse.c_str(),sContext.c_str());
			continue;
		}
		std::cout<<"--OUTPUT-------------------------------------------"<<std::endl;
		std::cout<<sXmlResponse<<std::endl<<std::endl;
	}
	fOut.write("fOut.txt");
}

void testHint(SemanticSearchServiceClient& client){
	acl::File fIn,fOut;
	std::string sContext;
	std::string sXmlRequest,sXmlResponse;
	fIn.read("test.txt");
	for(size_t i=0; i < fIn.size(); i++){
		acl::Line sXmlRequest = fIn[i];
		if(sXmlRequest.empty() || sXmlRequest.isBeginWith("#"))
		{
			ELOGGER->print(true, FAILE_CALL_PARAM("testHint","[sXmlRequest.empty() || sXmlRequest.isBeginWith('#')]", sXmlRequest.c_str()));
			continue;
		}
		if(writeContext(sXmlRequest,sContext) == false){
			ELOGGER->print(true, "testHint [writeContext(sXmlRequest,sContext) == false] Write context failed!\nsXmlRequest[%s]\nsContext[%s]\n", sXmlRequest.c_str(),sContext.c_str());
			continue;
		}
		std::cout<<"--INPUT-------------------------------------------"<<std::endl;
		std::cout<<sXmlRequest<<std::endl<<std::endl;
		client.getHint(sXmlResponse, sXmlRequest);
		if(readContext(sXmlResponse,sContext) == false){
			ELOGGER->print(true, "testHint [readContext(sXmlResponse,sContext) == false] Read context failed!\nsXmlResponse[%s]\nsContext[%s]\n", sXmlResponse.c_str(),sContext.c_str());
			continue;
		}
		std::cout<<"--Hint-------------------------------------------"<<std::endl;
		std::cout<<sXmlResponse<<std::endl<<std::endl;
	}
	fOut.write("fOut.txt");
}

void testRealteQ(SemanticSearchServiceClient& client){
	acl::File fIn,fOut;
	std::string sContext;
	std::string sXmlRequest,sXmlResponse;
	fIn.read("test.txt");
	for(size_t i=0; i < fIn.size(); i++){
		acl::Line sXmlRequest = fIn[i];
		if(sXmlRequest.empty() || sXmlRequest.isBeginWith("#"))
		{
			ELOGGER->print(true, FAILE_CALL_PARAM("testRealteQ","[sXmlRequest.empty() || sXmlRequest.isBeginWith('#')]", sXmlRequest.c_str()));
			continue;
		}
		if(writeContext(sXmlRequest,sContext) == false){
			ELOGGER->print(true, "testRealteQ [writeContext(sXmlRequest,sContext) == false] Write context failed!\nsXmlRequest[%s]\nsContext[%s]\n", sXmlRequest.c_str(),sContext.c_str());
			continue;
		}
		std::cout<<"--INPUT-------------------------------------------"<<std::endl;
		std::cout<<sXmlRequest<<std::endl<<std::endl;
		client.getRelate(sXmlResponse, sXmlRequest);
		if(readContext(sXmlResponse,sContext) == false){
			ELOGGER->print(true, "testRealteQ [readContext(sXmlResponse,sContext) == false] Read context failed!\nsXmlResponse[%s]\nsContext[%s]\n", sXmlResponse.c_str(),sContext.c_str());
			continue;
		}
		std::cout<<"--RELATIONQ-------------------------------------------"<<std::endl;
		std::cout<<sXmlResponse<<std::endl<<std::endl;
	}
	fOut.write("fOut.txt");
}


int main(int argc, char **argv) {
    char* sIP = "127.0.0.1";
    int nPort = 9000;
    if(argc == 3){
        sIP = argv[1];
        nPort = atoi(argv[2]);
    }
	boost::shared_ptr<TSocket> socket(new TSocket(std::string(sIP), nPort));
	boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	SemanticSearchServiceClient client(protocol);
	transport->open();
	testChat(client);
	//testHint(client);
	//testRealteQ(client);
	//testSemanticList(client);
	transport->close();
	return 0;
}
