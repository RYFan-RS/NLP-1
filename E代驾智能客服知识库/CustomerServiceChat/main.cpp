#include "ChatEngine.h"
#include "Common/lib4/IO.hpp"
#include "Common/lib4/File.hpp"
#include "Common/json/reader.h"
#include "Common/json/writer.h"
#include "Common/json/elements.h"

#include "KeyWordScoreRecord.h"


#define SEARCH_KB_XML_HEADER_BEGIN  "<?xml version=\"1.0\" encoding=\"UTF-8\"?><scml name=\"testModule\" type=\"FAQ\">\n \
  <config>\n \
      	<Parser>\n \
      		<parser_conf>dic.conf</parser_conf>\n \
      	</Parser>\n\
      <SemanticSearch valid=\"false\">\n\
      	<semantic_dic>semantic.dat</semantic_dic>\n\
      </SemanticSearch>\n	\
      <BasicSearch valid=\"true\">\n		\
      	<wordnetwork_dic>WordNetwork.dic</wordnetwork_dic>\n	\
      	<useless_dic>UselessWords.dic</useless_dic>\n	\
      	<pinyin_dic>Hanzi2Pinyin.dic</pinyin_dic>\n		\
      </BasicSearch>\n	\
  </config>"

#define SEARCH_KB_XML_END   "</scml>"

robosay::base::ChatEngine ce;
namespace aisms{
        aisms::GOption gOption;
}

class Response{
public:
	std::string sAnswer;
	std::string sContext;
	std::vector<std::string> vRelatedQ;
public:
	bool isXmlAnswer() const{
		return (!sAnswer.empty() && sAnswer[0] == '<' && sAnswer[sAnswer.length()-1] == '>') ? true : false;
	}
	bool isXmlInAnswer() const;
	acl::String removeXmlChars(const acl::String& sIn) const;
	acl::String removeWhiteChars(const acl::String& sIn) const;
	std::string AnalyzeXml(const acl::String& sIn) const;
	std::string removeStrHead(std::string& sIn) const;
	std::string InsideTest(std::string sIn) const;
};

bool Response::isXmlInAnswer() const
{
	if(!sAnswer.empty())
	{
		bool bHasLeft = false;
		bool bHasRight = false;
		if(std::string::npos != sAnswer.find("<a"))
		{
			bHasLeft = true;
		}
		if(std::string::npos != sAnswer.find("</a>"))
		{
			bHasRight = true;
		}
		if((bHasLeft)&&(bHasRight))
		{
			return true;
		}
	}
	return false;
}
std::string Response::InsideTest(std::string sIn) const
{
	robosay::XmlParser xmlAnswer;
	acl::Utility::stringReplace(sIn,"_CDATA_BEG", "<![CDATA[");
    acl::Utility::stringReplace(sIn,"_CDATA_END", "]]>");
	if(xmlAnswer.parse(sIn) == false){
		PRINT(sIn<<" is not an standard XML")
		return "";
	}
	std::string sServiceName = xmlAnswer.getXPathValue("//WeChat/ServiceName");
	return xmlAnswer.getXPathValue("//WeChat/ServiceName");
}

acl::String Response::removeXmlChars(const acl::String& sIn) const
{
    acl::String sOut;
    bool bDoFlag = true;
    for(size_t i=0; i<sIn.length(); i++)
	{
		if(sIn[i]=='<'){
			bDoFlag = false;
		}else if((sIn[i]=='>')&&(bDoFlag == false)){
            bDoFlag = true;
			continue;
        }
        if((bDoFlag==true)){
            sOut += sIn[i];
		}
    }
    return sOut;
}

acl::String Response::removeWhiteChars(const acl::String& sIn) const
{
        acl::String sOut;
        bool bDoFlag = true;
        for(size_t i=0; i<sIn.length(); i++){
                if((sIn[i]==' ' || sIn[i]=='\t' || sIn[i]=='\r' || sIn[i]=='\n') && (bDoFlag==true)){
                        continue;
                }
		else if(sIn[i]=='<'){
                        bDoFlag = false;
                }else if(sIn[i]=='>'){
                        bDoFlag = true;
                }
                sOut += sIn[i];
        }
        return sOut;
}

std::string Response::removeStrHead(std::string& sIn) const
{
	if(("" == sIn)||("_CDATA_BEG_CDATA_END" == sIn))
	{
		return "";
	}
	size_t sBegPosition = sIn.find("_CDATA_BEG");
	size_t sEndPosition = sIn.find("_CDATA_END");
	if((sBegPosition != std::string::npos)&&(sEndPosition != std::string::npos))
	{
		size_t sLength = sIn.length();
		size_t sBegLen = strlen("_CDATA_BEG");
		return sIn.substr(sBegPosition + sBegLen,sEndPosition-sBegPosition-sBegLen);

	}else
	{
		return sIn;
	}
}

std::string Response::AnalyzeXml(const acl::String& sIn) const
{
	robosay::XmlParser xmlAnswer;
	std::string sXmlResponse = "";
	std::vector<xmlNodePtr> xmlNodeVec;
	if(xmlAnswer.parse(sIn) == false){
		PRINT(sIn<<" is not an standard XML")
		return "";
	}
	std::string sCount = xmlAnswer.getXPathValue("//WeChat/WeChatParam/ArticleCount");
	int Count = atoi(sCount.c_str());

	if(Count > 0)
	{
		std::string sServiceName = xmlAnswer.getXPathValue("//WeChat/ServiceName");
		sXmlResponse += sServiceName;
		for(int i = 0;i < Count;i++)
		{
			char chNum[5] = {'\0'};
			sprintf(chNum,"%d",i+1);
			std::string strNum = chNum;
			std::string strItem = "item["+ strNum + "]";

			std::string sItemTitle = xmlAnswer.getXPathValue("//WeChat/WeChatParam/Articles/"+strItem+"/Title");
			sItemTitle = removeStrHead(sItemTitle);
			std::string sItemDescription = xmlAnswer.getXPathValue("//WeChat/WeChatParam/Articles/"+strItem+"/Description");
			sItemDescription = removeStrHead(sItemDescription);
			std::string sItemPicUrl = xmlAnswer.getXPathValue("//WeChat/WeChatParam/Articles/"+strItem+"/PicUrl");
			sItemPicUrl = removeStrHead(sItemPicUrl);
			std::string sItemUrl = xmlAnswer.getXPathValue("//WeChat/WeChatParam/Articles/"+strItem+"/Url");
			sItemUrl = removeStrHead(sItemUrl);

			std::string strTmp = sItemTitle + sItemDescription + sItemPicUrl + sItemUrl;
			sXmlResponse += strTmp;
		}

	}
	else
	{

		std::string sServiceName = xmlAnswer.getXPathValue("//WeChat/ServiceName");
		sServiceName = removeStrHead(sServiceName);
		sXmlResponse += sServiceName;
		std::string sVideoId = xmlAnswer.getXPathValue("//WeChat/WeChatParam/Video/MediaId");
		sVideoId = removeStrHead(sVideoId);
		std::string sVideoTitle = xmlAnswer.getXPathValue("//WeChat/WeChatParam/Video/Title");
		sVideoTitle = removeStrHead(sVideoTitle);
		std::string sVideoDescription = xmlAnswer.getXPathValue("//WeChat/WeChatParam/Video/Description");
		sVideoDescription = removeStrHead(sVideoDescription);

		std::string sImageId = xmlAnswer.getXPathValue("//WeChat/WeChatParam/Image/MediaId");
		sImageId = removeStrHead(sImageId);
		std::string sImageTitle = xmlAnswer.getXPathValue("//WeChat/WeChatParam/Image/Title");
		sImageTitle = removeStrHead(sImageTitle);
		std::string sImageDescription = xmlAnswer.getXPathValue("//WeChat/WeChatParam/Image/Description");
		sImageDescription = removeStrHead(sImageDescription);

		std::string sVoiceId = xmlAnswer.getXPathValue("//WeChat/WeChatParam/Voice/MediaId");
		sVoiceId = removeStrHead(sVoiceId);
		std::string sVoiceTitle = xmlAnswer.getXPathValue("//WeChat/WeChatParam/Voice/Title");
		sVoiceTitle = removeStrHead(sVoiceTitle);
		std::string sVoiceDescription = xmlAnswer.getXPathValue("//WeChat/WeChatParam/Voice/Description");
		sVoiceDescription = removeStrHead(sVoiceDescription);

		std::string sMusicTitle = xmlAnswer.getXPathValue("//WeChat/WeChatParam/Music/Title");
		sMusicTitle = removeStrHead(sMusicTitle);
		std::string sMusicDescription = xmlAnswer.getXPathValue("//WeChat/WeChatParam/Music/Description");
		sMusicDescription = removeStrHead(sMusicDescription);
		std::string sMusicUrl = xmlAnswer.getXPathValue("//WeChat/WeChatParam/Music/MusicUrl");
		sMusicUrl = removeStrHead(sMusicUrl);
		std::string sHQMusicUrl = xmlAnswer.getXPathValue("//WeChat/WeChatParam/Music/HQMusicUrl");
		sHQMusicUrl = removeStrHead(sHQMusicUrl);
		std::string sThumbMediaId = xmlAnswer.getXPathValue("//WeChat/WeChatParam/Music/ThumbMediaId");
		sThumbMediaId = removeStrHead(sThumbMediaId);

		if("" != sVideoId)
		{
			sXmlResponse =  sServiceName + sVideoId + sVideoTitle + sVideoDescription;
			sVideoId = "";
		}else if("" != sImageId)
		{
			sXmlResponse =  sServiceName + sImageId + sImageTitle + sImageDescription;
			sImageId = "";
		}else if("" != sVoiceId)
		{
			sXmlResponse =  sServiceName + sVoiceId + sVoiceTitle + sVoiceDescription;
			sVoiceId = "";
		}else if("" != sMusicUrl)
		{
			sXmlResponse =  sServiceName + sMusicTitle + sMusicDescription + sMusicUrl + sHQMusicUrl + sThumbMediaId;
			sMusicUrl = "";
		}
		else
		{
			sXmlResponse = "Xml file is Error !!!";
		}
	}
	return sXmlResponse;
}

std::string transXml2Text(std::string sXml){

	std::string sOut;
	std::string sTmp = "(CALL_EXTERNAL_API:Java:";
	robosay::XmlParser xmlParser;
	xmlParser.parse(sXml);

	// 得到servicename
	acl::String sServiceName = xmlParser.getXPathValue("//WeChat/ServiceName");
	sServiceName.removeChars("\t\r\n");

	acl::String sQueryParam = xmlParser.getXPathValue("//WeChat/RobotMsg/QueryParams");
	sQueryParam.removeChars("\t\r\n");

	std::vector<std::string> vTitle = xmlParser.getXPathValueVec("//Title");
	std::string sCount = xmlParser.getXPathValue("//WeChat/WeChatParam/ArticleCount");

	if(sCount == "2"){
		if(!sServiceName.empty())
			sTmp += sServiceName + ":";
		if(!sQueryParam.empty())
			sTmp += sQueryParam + ")";
		if(vTitle.size() == 2){
			acl::String v0 = vTitle[0];
			v0.removeChars("\t\r\n ");
			acl::String v1 = vTitle[1];
			v1.removeChars("\t\r\n ");
			sOut = v0 + sTmp + v1;
		}else{
			PRINT("\"sCount=2\" input error, check the input !!!")
		}
	}else if(sCount == "3"){
		if(!sServiceName.empty())
			sTmp += sServiceName + ":";
		if(!sQueryParam.empty())
			sTmp += sQueryParam + ")";

		if(vTitle.size() == 3){
			acl::String v0 = vTitle[0];
			v0.removeChars("\t\r\n ");
			sOut = sTmp + v0;
		}else{
			PRINT("\"sCount=3\" input error, check the input !!!")
		}
	}else{
		PRINT("INPUT STRING ERROR!!!")
	}
	return sOut;
}

Response divideResponse(const std::string& s,size_t sFlag)
{
	Response response;
	std::string sAnswerContext;
	response.vRelatedQ = acl::Utility::separate(s,"<rtlqsep>");

	if(!response.vRelatedQ.empty()){

		sAnswerContext = response.vRelatedQ[response.vRelatedQ.size()-1];
		response.vRelatedQ.pop_back();
	}

	size_t nPos = sAnswerContext.find("<ChatStateContainer>");
	if(nPos == std::string::npos){
		sAnswerContext = response.removeWhiteChars(sAnswerContext);
		if(std::string::npos != sAnswerContext.find("WeChat"))
		{
			if(1 == sFlag)
			{
				sAnswerContext = response.InsideTest(sAnswerContext);
			}else
			{
				sAnswerContext = response.AnalyzeXml(sAnswerContext);
			}
		}
		response.sAnswer = sAnswerContext;
	}else{
		response.sAnswer = sAnswerContext.substr(0,nPos);
		response.sContext = sAnswerContext.substr(nPos);
	}
	return response;
}

class ScriptTest{
private:
        class TestSet : public std::vector<std::vector<std::string> >{
                public:
                        TestSet(){
                                nBegLineNumOfInFile = -1;
                                nEndLineNumOfInFile = -1;
                        }
                        int nBegLineNumOfInFile;
                        int nEndLineNumOfInFile;
                        std::string toStr() const{
                                std::stringstream ss;
                                for(size_t i=0; i<this->size(); i++){
                                        const std::vector<std::string>& v = this->at(i);
                                        for(size_t j=0; j<v.size(); j++){
                                                ss << v[j] <<std::endl;
                                        }
                                        ss << "RESET" <<std::endl;
                                }
                                return ss.str();
                        }
                        std::string getLineIndex() const{
                                std::stringstream ss;
                                ss << "源文件索引："<<nBegLineNumOfInFile<<"行--"<<nEndLineNumOfInFile<<"行";
                                return ss.str();
                        }
        };
     class Result{
                public:
                        Result(){
                                nGroupCount=0;
                                nInputCount=0;
                                nGroupPassCount=0;
                                nPassCount=0;
                        }
                        int nGroupCount;
                        int nInputCount;
                        int nGroupPassCount;
                        int nPassCount;
                        std::string toStr() const{
                                std::stringstream sstream;
                                sstream<<std::endl<<"=============测试报告============="<<std::endl;
                                sstream<<"总对话组: "<<nGroupCount<<", 通过组: "<<nGroupPassCount<<", 通过率："<<(double)nGroupPassCount/(double)nGroupCount*100<<"%"<<std::endl;
                                sstream<<"总输入文: "<<nInputCount<<", 通过数: "<<nPassCount<<", 通过率："<<(double)nPassCount/(double)nInputCount*100<<"%"<<std::endl;
                                return sstream.str();
                        }
        };
public:
        void test(const std::vector<acl::Line>& vRowGroup, int nBegLineNum, int nEndLineNum,size_t sFlag){
                std::vector<std::string> vError;
                TestSet tset = permutationAndCombination(vRowGroup);
				tset.nBegLineNumOfInFile = nBegLineNum;
                tset.nEndLineNumOfInFile = nEndLineNum;



                std::string sArg = "";
                for(size_t i=0; i<tset.size(); i++){
                        if(tset[i].empty()) continue;
                        std::vector<std::string> vTempResult;
                        bool bGroupPassed = true;
						std::string sPreContext;
										
                        for(size_t j=0; j<tset[i].size(); j++){
                                acl::Line sLine = tset[i][j];
                                std::vector<std::string> vElement = sLine.separate("\t");
                                const std::string& sQuestion = vElement[0];
                                const std::string sExpectAnswer = vElement[1].substr(8);
                                Result_info resultInfo;

								bool bRet = ce.process(sQuestion, true, sPreContext, resultInfo, "0");

								std::string sOut;
							    if(!bRet){
						        }	
								else
								{
	                                sOut =  resultInfo.m_answer.getAnswerContent();
								}
                                Response response = divideResponse(sOut,sFlag);
                                if(response.isXmlAnswer())
								{
                                    response.sAnswer = transXml2Text(response.sAnswer);
                                }
								if(response.isXmlInAnswer())
								{
									response.sAnswer = response.removeXmlChars(response.sAnswer);
								}
                                if(!response.sContext.empty()){
                                        sArg = response.sContext;
                                }
								if(removeWhiteChars(sOut) == removeWhiteChars(sExpectAnswer)){
                                        vTempResult.push_back("[PASSED]\t" + sQuestion);
                                        result.nPassCount++;
                                }else{
                                        vTempResult.push_back("[FAILED]\t" + sQuestion + "\t[期望答案]" + sExpectAnswer + "\t[实际答案]" + removeWhiteChars(sOut));
                                        bGroupPassed = false;
                                }
                                result.nInputCount++;
                        }
                        sArg = "";
                        result.nGroupCount++;
                        if(bGroupPassed == false){
                                outFile.push_back("#" + tset.getLineIndex());
                                for(size_t j=0; j<vTempResult.size(); j++){
                                        outFile.push_back(vTempResult[j]);
                                }
                        }else{
                                result.nGroupPassCount++;
                        }
                }
        }
	int generateKBXml(const acl::String& sRootFile = "",std::string sInFileName = "",std::string sOutFileName = ""){
	    aisms::gOption.bDebugMode = true;
	    aisms::gOption.bSingleAnswer = true;
		
	    acl::File inFile;
	    if(sInFileName.empty()){
	            sInFileName = "in.txt";
	    }
	    if(sOutFileName.empty()){
	            sOutFileName = "in1.txt";
	    }		
	    if(inFile.read(sInFileName) == false) {
	            PRINT("Can't open KB file ["<<sInFileName<<"] !")
	            return 2;
	    }
	    if(checkTestFileFormat(inFile,false) == false){
	            PRINT("KB file meets format error!")
	            return 3;
	    }
		// 记录起始时间
	    acl::Time timer;
	    timer.start("TotalTimeCost");
	    std::vector<acl::Line> vRowGroup;
	    int nLastBegLineNum = 0;
	    std::cerr<<"Total line "<<inFile.size()-1<<", current ";
	    acl::IO::overwrite("");
		outFile.push_back(SEARCH_KB_XML_HEADER_BEGIN);
	    for(size_t i=0; i<inFile.size(); i++){
	            acl::IO::overwrite(i);
	            const acl::Line& sLine = inFile.at(i);
	            if(sLine == "RESET"){ // 对话组更新标志行
	                    continue;
	            }else if(sLine.isBeginWith("#")){// 注释行
	                    continue;
	            }else if(sLine.empty()){// 空行
	                    continue;
	            }else{// 有效行
					std::string expectAnswer,answer;
					std::vector<std::string> vElement = sLine.separate("\t");
					expectAnswer = vElement[vElement.size()-1];
					vElement.pop_back();
					outFile.push_back("	<qa>");
					int ij =0;
					for(;ij<vElement.size();ij++){
						if(!vElement[ij].empty()){
							outFile.push_back("		<question>"+vElement[ij]+"</question>");
						}
					}
					GetExpectAnswer(expectAnswer,answer);
					if(answer.empty() || (ij==0)){
	    				std::cerr<<"line "<<i+1<<", has no answer or question.please check in KB file"<<std::endl;	
						return 4;
					}
					outFile.push_back("		<answer>"+answer+"</answer>");
					outFile.push_back("	</qa>");
	            }
	    }
	    std::cerr<<std::endl;
	    // 写入测试报告
	    //outFile.push_back(result.toStr());
		outFile.push_back(SEARCH_KB_XML_END);
		std::string sKBFileName = "./data/questionKB.xml";


		std::fstream ofs(sKBFileName.c_str(), std::ios::out);
		if(!ofs){
			std::cerr<<"produce  questionKB.xml error:please check the file path:data/questionKB.xml"<<std::endl;	
		    return 0;
		}else{
			ofs.close();		
			outFile.write(sKBFileName);
	    	PRINT("produce ./data/questionKB.xml OK");		
		}

	    return runQuestion(sRootFile,sOutFileName);
	}
   int runQuestion(const acl::String& sRootFile,std::string sInFileName = "",size_t sFlag=0){
                aisms::gOption.bDebugMode = true;
                aisms::gOption.bSingleAnswer = true;
				
				outFile.clear();

                if(ce.init(sRootFile) == false) {
                        PRINT("ChatEngine init error!")
                        return 1;
                }
				
                acl::File inFile;

                if(inFile.read(sInFileName) == false) {
                        PRINT("Can't open input file ["<<sInFileName<<"] !")
                        return 2;
                }
                if(checkTestFileFormat(inFile,false) == false){
                        PRINT("Input file meets format error!")
                        return 3;
                }
				// 记录起始时间
                acl::Time timer;
                timer.start("TotalTimeCost");
                std::vector<acl::Line> vRowGroup;
                int nLastBegLineNum = 0;
                std::cerr<<"Total line "<<inFile.size()-1<<", current ";
                acl::IO::overwrite("");
                for(size_t i=0; i<inFile.size(); i++){
                        acl::IO::overwrite(i);
                        const acl::Line& sLine = inFile.at(i);
                        if(sLine == "RESET"){ // 对话组更新标志行
                                continue; 
                        }else if(sLine.isBeginWith("#")){// 注释行
                                continue;
                        }else if(sLine.empty()){// 空行
                                continue;
                        }else{// 有效行
                                vRowGroup.push_back(sLine);
                                test(vRowGroup,nLastBegLineNum,i+1,sFlag);
                                vRowGroup.clear();
                                nLastBegLineNum = i+2;								
                        }
                }
                std::cerr<<std::endl;
                // 写入测试报告
                outFile.push_back(result.toStr());
             	PRINT(timer.toStr("TotalTimeCost"));
                PRINT(result.toStr())
                outFile.write("out.txt");
                return 0;
        }	
   int run(const acl::String& sRootFile,size_t sFlag,std::string sInFileName = "", std::string sSqlFileName = "", bool bHisotryRight = true){
                aisms::gOption.bDebugMode = true;
                aisms::gOption.bSingleAnswer = true;

				#if 0
                if(ce.init(sRootFile) == false) {
                        PRINT("ChatEngine init error!")
                        return 1;
                }
				#endif
				
                acl::File inFile;
                if(sInFileName.empty() || sInFileName=="-o"){
                        sInFileName = "in.txt";
                }
                if(inFile.read(sInFileName) == false) {
                        PRINT("Can't open input file ["<<sInFileName<<"] !")
                        return 2;
                }
                if(checkTestFileFormat(inFile) == false){
                        PRINT("Input file meets format error!")
                        return 3;
                }
				// 记录起始时间
                acl::Time timer;
                timer.start("TotalTimeCost");
                std::vector<acl::Line> vRowGroup;
                int nLastBegLineNum = 0;
                std::cerr<<"Total line "<<inFile.size()-1<<", current ";
                acl::IO::overwrite("");
                for(size_t i=0; i<inFile.size(); i++){
                        acl::IO::overwrite(i);
                        const acl::Line& sLine = inFile.at(i);
                        if(sLine == "RESET"){ // 对话组更新标志行
                                test(vRowGroup,nLastBegLineNum,i+1,sFlag);
                                vRowGroup.clear();
                                nLastBegLineNum = i+2;
                                continue;
                        }else if(sLine.isBeginWith("#")){// 注释行
                                continue;
                        }else if(sLine.empty()){// 空行
                                continue;
                        }else{// 有效行
                                vRowGroup.push_back(sLine);
                        }
                }
                std::cerr<<std::endl;
                // 写入测试报告
                outFile.push_back(result.toStr());
             	PRINT(timer.toStr("TotalTimeCost"));
                PRINT(result.toStr())
                outFile.write("out.txt");
                return 0;
        }


		std::string removeWhiteChars(const std::string& sIn) const{
			std::string sOut;
			bool bDoFlag = true;
			
			for(size_t i=0; i<sIn.length(); i++){
				if((sIn[i]==' ' || sIn[i]=='\t' || sIn[i]=='\r' || sIn[i]=='\n') && bDoFlag==true){
					continue;
				}else if(sIn[i]=='<'){
					bDoFlag = false;
				}else if(sIn[i]=='>'){
					bDoFlag = true;
				}
				sOut += sIn[i];
			}
			
			return sOut;
		}

private:
        static bool checkTestFileFormat(const acl::File& inFile,bool bInitDate=true) {
                bool bFormatError = false;
                // 检查首行是否为日期设置
                if(bInitDate){
	                if(!inFile.empty() && !inFile[0].isBeginWith("#DATE=")){
	                        PRINT("Can't find date set string in \"in.txt\".")
	                        return false;
	                }
	                // 根据首行日期设定调试用日期变量
	                aisms::gOption.sDate = inFile[0].substr(6);
	                PRINT("Set date to '"<<aisms::gOption.sDate<<"'.")
            	}
				
                // 逐行做基本语法检查
				for(size_t i=1; i<inFile.size(); i++){
                        const acl::Line& sLine = inFile.at(i);
                        if(sLine == "RESET" || sLine.isBeginWith("#") || sLine.empty()){
                                continue;
                        }
                        std::vector<std::string> vElement = sLine.separate("\t");
                        // 有效行的元素数量必须超过两个
                        if(vElement.size() < 2){
                                PRINT("The format of line "<<i+1<<" in \"in.txt\" error.")
                                bFormatError = true;
                                continue;
                        }
                        // 有效行的最后一个元素必须为期望结果
                        if(vElement[vElement.size()-1].find("[EXPECT]") != 0){
                                PRINT("The format of line "<<i+1<<" in \"in.txt\" error.")
                                bFormatError = true;
                                continue;
                        }
                }
              // 要求最后一个有效行必须为RESET
                for(int i=(int)inFile.size()-1; i>-1; i--){
                        const acl::Line& sLine = inFile.at(i);
                        if(sLine.isBeginWith("#") || sLine.empty()){
                                continue;
                        }
                        if(sLine != "RESET"){
                                PRINT("The last valid line of \"in.txt\" should be \"RESET\".")
                                bFormatError = true;
                        }
                        break;
                }
                return !bFormatError;
        }

		bool IsVaildExpectFormat(const std::string& strSrc)const
		{
			return (strSrc.find("[EXPECT]") == 0) || (strSrc.find("[EXPECT_KEYWORD]") == 0);
		}

		bool GetExpectType(const std::string& strSrc, std::string& strExpectType)const
		{
			size_t uPos = strSrc.find(']');
			if (std::string::npos == uPos) { return false; }
			
			strExpectType = strSrc.substr(1,uPos-1);
			return true;
		}

		static bool GetExpectAnswer(const std::string& strSrc, std::string& strExpectType)
		{
			size_t uPos = strSrc.find(']');
			if (std::string::npos == uPos) { return false; }
			
			strExpectType = strSrc.substr(uPos+1);
			return true;
		}

		bool IsExpectTypeMatchActualType(const std::string& strActualType,const std::string& strExpectType)const
		{
			//strActualType value is 
			//DefaultAnswer: std::string Question_info::DefaultAnwser = "DefaultAnswer"; 
			//KeyWordAnwser: std::string Question_info::KeyWordAnwser = "KeyWordAnwser";
			//SemanticAnswer:std::string Question_info::SemanticAnswer = "SemanticAnswer";
			
			//strExpectType  (EXPECT,EXPECT_KEYWORD)
			if (strActualType.empty() || strExpectType.empty()) { return false; }

			//if strActualType is DefaultAnswer, is not match strExpectType
			//if (strActualType == Question_info::DefaultAnwser) { return false; }

			//match scene, return true
			//if strExpectType is EXPECT, strActualType is SemanticAnswer
			if ((strExpectType=="EXPECT") && strActualType==Question_info::SemanticAnswer) { return true; }
				
			if ((strExpectType=="EXPECT_KEYWORD") && strActualType==Question_info::KeyWordAnwser) { return true; }
	
			//no match
			return false;
		}

		void CovertSemanticTypeIfEmpty(std::string& strMatchType)const
		{
			if (!strMatchType.empty()) { return; }

			strMatchType = Question_info::SemanticAnswer;
		}
		
		
        TestSet permutationAndCombination(const std::vector<acl::Line>& vRowGroup){
                TestSet tset;
                std::vector<std::vector<std::string> > vvElement;
                std::vector<size_t> vIdx;
                std::vector<std::string> vExpect;
                for(size_t i=0; i<vRowGroup.size(); i++){
                        std::vector<std::string> vElement = vRowGroup[i].separate("\t");
                        vExpect.push_back(vElement[vElement.size()-1]);
                        vElement.pop_back();
                        vvElement.push_back(vElement);
                }
         for(size_t i=0; i<vvElement.size(); i++){
                        vIdx.push_back(0);
                }
                while(1){
                        std::vector<std::string> vGroup;

                        for(size_t i=0; i<vvElement.size(); i++){
                                vGroup.push_back(vvElement[i][vIdx[i]] + "\t" + vExpect[i]);
                        }
                        tset.push_back(vGroup);
                        bool bChanged = false;
                        for(size_t j=0; j<vIdx.size(); j++){
                                if(vIdx[j] < vvElement[j].size()-1){
                                        vIdx[j]++;
                                        for(size_t k=0; k<j; k++){
                                                vIdx[k] = 0;
                                        }
                                        bChanged = true;
                                        break;
                                }
                        }
                        if(bChanged == false) break;
                }
     return tset;
        }
private:
	 Result result;
     acl::File outFile;
	 robosay::base::CKeyWordScoreRecord keyWordScoreRecord;
};

class CmdArgs {
public:
	CmdArgs() : m_bNaturalInput(false), m_bSemanticInput(false),m_iTestFunction(0),m_bGenerate(false){}
public:
	std::string usage(){
		std::string sRet;

		sRet += "USAGE:\n";
		sRet += "    argument              parameter           intorduction\n";
		sRet += "    -h,--help                                 Print this message.\n";
		sRet += "    -v,--version                              Print version information and then exit.\n";
		sRet += "    -n,--natural                              natural language input";
		sRet += "    -s,--semantic                             semantic input";
		sRet += "    -g,--generate                             generate xml with question.\n";			
		sRet += "    -i,--input            [file-name]         Read input from a txt file.\n";
		sRet += "    -o,--output           [file-name]         Write output to a file.\n";
		sRet += "\n";
		sRet += "EXAMPLES:\n";
		sRet += "    (自然语言自由输入)  ./main -n\n";
		sRet += "    (语义自由输入)      ./main -s\n";
		sRet += "    (自然语言文件输入)  ./main -n -i in.txt -o out.txt\n";
		sRet += "    (语义文件输入)      ./main -s -i in.txt -o out.txt\n";
		sRet += "    (自然语言输入:Test) ./main -n -t -i -o \n";
		sRet += "    (生成question知识库并进行批量用例测试)./main -g -i in.txt -o in1.txt\n";

		return sRet;
	}

	std::string version(){
		std::string sRet;
		sRet = "We don't have release version now!\n";
		return sRet;
	}

	bool parse(int argc, char** argv, std::string& sRet){
		std::vector<std::string> vArg;
		for(int i=1; i<argc; i++){
			vArg.push_back(argv[i]);
		}

		return parse(vArg,sRet);
	}

	bool parse(const std::vector<std::string>& vArg, std::string& sRet){
		if(vArg.empty()){
			sRet += "Error: Don't know what to do without argument!\n\n";
			sRet += usage();
			return false;
		}

		size_t uLast = vArg.size() - 1;
		//read arguments
		for(size_t i=0; i<vArg.size(); i++){
			if(vArg.at(i) == "-h" || vArg.at(i) == "--help"){
				sRet = usage();
				return false;
			}else if(vArg.at(i) == "-v" || vArg.at(i) == "--version"){
				sRet = version();
				return false;
			}else if(vArg.at(i) == "-n" || vArg.at(i) == "--natural"){
				// 自然语言方式输入标志
				m_bNaturalInput = true;
			}else if(vArg.at(i) == "-s" || vArg.at(i) == "--semantic"){
				// 语义方式输入标志
				m_bSemanticInput = true;
			}else if(vArg.at(i) == "-i" || vArg.at(i) == "--input"){
				if(i < uLast){
					m_sInFileName = vArg.at(++i);
				}else{
					return false;
				}
			}else if(vArg.at(i) == "-o" || vArg.at(i) == "--output"){
				if(i < uLast){
					m_sOutFileName = vArg.at(++i);
				}else{
					return false;
				}
			}
			else if(vArg.at(i) == "-t"){
				if(i < uLast){
					m_iTestFunction = 1;
				}else{
					return false;
					}
			}else if(vArg.at(i) == "-g" || vArg.at(i) == "--generate"){
				m_bGenerate = true;
			}else{
				sRet += "Error: Can't parse argument '" + vArg.at(i) + "'!\n\n";
				sRet += usage();
				return false;
			}
		}

		return true;
	}

	bool isNaturalInput() const{
		return m_bNaturalInput;
		std::cout<<"isNaturalInput!"<<std::endl;
	}

	bool isGenerateKBXml() const{
		return m_bGenerate;
	}

	bool isSemanticInput() const{
		return m_bSemanticInput;
	}

	const std::string& getInputFile() const{
		return m_sInFileName;
	}

	const std::string& getOutputFile() const{
		return m_sOutFileName;
	}

	const size_t getTestFunction() const{
		return m_iTestFunction;
	}

private:
	std::string m_sInFileName;
	std::string m_sOutFileName;
	bool        m_bNaturalInput;
	bool        m_bSemanticInput;
	bool        m_bGenerate;
	int	m_iTestFunction;
};

// 自由输入方式
int freeChat(bool bNeedAnalysis)
{
	std::string sPre;
    std::string sType;
	while(1) {
		acl::String sIn = acl::IO::STDIN("YouSay:");
		if(sIn == "q" || sIn == "quit")
			break;
		if(sIn == "RESET"){
			sPre.clear();
			continue;
		}
        Result_info resultInfo;
        bool bRet = ce.process(sIn, true, sPre, resultInfo, "2");
        if(!bRet){
            P("Failed to call m_engine.process!!!\n");
        }else{
            std::cout<<"RoboSay:-----------------------"<<std::endl;
			std::cout<<"Title:"<<resultInfo.m_answer.GetTitle()<<std::endl;
            std::cout<<"Answer:"<<resultInfo.m_answer.get_answer()<<std::endl<<std::endl;
			std::cout<<"相关问题:";
			std::vector<std::string> relateQuestions = resultInfo.m_relate.get_questions();
			for(int tempI=0;tempI<relateQuestions.size();tempI++){
				std::cout<<" "<<tempI+1<<":"<<relateQuestions[tempI];
			}
			robosay::XmlParser xmlParser;
			resultInfo.toXml(xmlParser);
			std::cout<<"  相关问题"<<std::endl;
        }
		std::cout<<"Context:"<<sPre<<std::endl<<std::endl;
	}

	return 0;
}

//测试函数，用来检测自测用例的成功率
int TestFunction(acl::String sRootFile,size_t sFlag = 0,std::string sInFileName = ""){

	ScriptTest scTest;
	scTest.run(sRootFile,sFlag,sInFileName);
    return 0;
}

int main(int argc, char** argv)
{
	acl::String sRootFile = "";
	#if 0
	if (!access("../../data/root.conf", 0)){
		sRootFile = "../../data/root.conf";
	}else 
	#endif
	if (!access("./data/root.conf", 0)){
		sRootFile = "./data/root.conf";
	}else {
		PRINT("../../data/root.conf 和 ./data/root.conf 都不存在!!!");
	}

#ifdef _CHAT_ENGINE_DEBUG
	std::cout<<"*************************"<<std::endl;
	std::cout<<"* This is debug version *"<<std::endl;
	std::cout<<"*************************"<<std::endl;
#endif

    CmdArgs args;
	std::string sMessage;
	bool bNeedAnalysis;
	if(args.parse(argc,argv,sMessage) == false){
		std::cout<<sMessage;
		return 2;
	}

	if(args.isNaturalInput()){
		// Natural language input mode		
		if(ce.init(sRootFile) == false){
			PRINT("ERROR")
			return 1;
		}		
		bNeedAnalysis = true;
		if(args.getInputFile().empty()){    // Free chat mode
			freeChat(bNeedAnalysis);
			return 3;
		}else{                              // File chat mode
			TestFunction(sRootFile,args.getTestFunction(),args.getInputFile());
			return 4;
		}
	}else if(args.isSemanticInput()){
		// Semantic language input mode	
		if(ce.init(sRootFile) == false){
			PRINT("ERROR")
			return 1;
		}	
		bNeedAnalysis = false;
		if(args.getInputFile().empty()){
			freeChat(bNeedAnalysis);
			return 5;
		}else{
			TestFunction(sRootFile,args.getTestFunction());
			return 6;
		}
	}else if(args.isGenerateKBXml()){
		ScriptTest scTest;
		scTest.generateKBXml(sRootFile,args.getInputFile(),args.getOutputFile());
	}
	else{
		return 7;
	}
	return 0;
}
