#ifndef __SUMMARY_INFO__HPP__
#define __SUMMARY_INFO__HPP__
#include "DynamicParser/Parser.h"
#include "SemanticAnalysis/mean.h"
#include "Serialization.hpp"

class SummaryResult{
public:
	std::string sWords;
	std::string sKeyword;
	std::string sAnswer;
};

class SummaryInfo{
public:
	bool init(const acl::FileName& sSemanticDataFile, const acl::FileName& sBinDataFile){
		//初始化语义词典
		if(m_sae.init(sSemanticDataFile) != 0){
			PE("Init semantic file ["<<sSemanticDataFile<<"] error!");
			return false;
		}
		
		//初始化动态分词引擎
		if(m_dyParser.init("com",m_sae.getData().getConstWordManager().getWords()) == false){
			PE("Init dynamic parser error!");
			return false;
		}
		
		//读取编译过的百度百科概要信息数据
		if(Serialization::readData(*this,sBinDataFile) == false){
			PE("Read data file ["<<sBinDataFile<<"] error!");
			return false;
		}
		
		return true;
	}
	
	SummaryResult analyze(const std::string& sInput){
		aisms::dypsr::Result psrResult = m_dyParser.parse(sInput,"com");
		std::vector<std::string> vWord = psrResult.getWords();
		sae::MeanWords words(vWord,m_sae.getExcutableData().getDataManagerPointer());
		
		VMeanResult vResult = m_sae.analyze(sInput,words,m_sae.getExecuteMethod("NORMAL_ANALYSIS"));
		
		SummaryResult result;
		for(size_t i=0; i<words.size(); i++){
			std::string sWordTag = words[i].dump2Str(m_sae.getExcutableData().getConstDataManager());
			if(sWordTag.find("KEY") != std::string::npos){
				result.sKeyword += words[i].getStrWord(m_sae.getExcutableData().getConstDataManager());
			}
		}
		
		result.sWords = words.dump2Str();
		
		std::string sSentenceTags = words.getStrTags();
		if(sSentenceTags.find("询问定义") != std::string::npos){
			std::map<std::string, std::string>::const_iterator iter = m_mSummary.find(result.sKeyword);
			if(iter != m_mSummary.end()){
				result.sAnswer = iter->second;
			}else{
				result.sAnswer = "";
			}
		}else{
			result.sAnswer = "";
		}
		
		return result;
	}
	
	bool compile(const acl::FileName& sRawDataFile, const acl::FileName& sBinDataFile){
		acl::File file;
		
		if(file.read(sRawDataFile) == false){
			PE("Read raw data file ["<<sRawDataFile<<"] failed");
			return false;
		}
		
		for(size_t i=0,size=file.size(); i<size; i++){
			const acl::Line& line = file[i];
			if(line.empty()) continue;
			
			std::vector<std::string> v = line.separate("\t");
			m_mSummary.insert(std::make_pair(v[0],v[1]));
		}
		
		file.clear();
		file.resize(0);
		
		//写入序列化Data文件
		if(Serialization::writeData(*this,sBinDataFile) == false){
			PE("Write serialize data faild");
			return false;
		}
		
		return true;
	}
	
public:
	template <class Format, class Refs>
	inline void composite(obj_input<Format, Refs>& stream) {
		int i,size = 0;
		Format &format = stream.get_format();
		format.input_start_array(size);
		for(i = 0; i < size; i++) {
			std::string first,second;
			stream.simple(first);
			stream.simple(second);
			m_mSummary.insert(std::make_pair(first,second));
		}
	}
	
	template <class Format, class Refs>
	inline void composite(obj_output<Format, Refs>& stream) {
		int size = m_mSummary.size();
		std::map<std::string,std::string>::iterator i;
		Format &format = stream.get_format();
		format.output_start_array(size);
		for(i = m_mSummary.begin(); i != m_mSummary.end(); ++i) {
			stream.simple(i->first);
			stream.simple(i->second);
		}
		format.output_end_array();
	}
private:
	ContextDictionary m_sae;
	std::map<std::string,std::string> m_mSummary;
	aisms::dypsr::Parser m_dyParser;
};

#endif //__SUMMARY_INFO__HPP__
