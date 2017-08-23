#ifndef __KNOWLEDGE_BASE__HPP__
#define __KNOWLEDGE_BASE__HPP__
#include "lib4/String.hpp"
#include "lib4/FileName.hpp"
#include "cppjieba/Jieba.hpp"
#include "Serialization.hpp"
#include "Tfidf.hpp"

static void* cppjiebaParser = NULL;

#define PROCESS_DISPLAY

class KnowledgeBase{
public:
	bool init(const acl::FileName& sBinDataFile){
		//读取序列化Data文件
		if(Serialization::readData(*this,sBinDataFile) == false){
			PE("Read serialize data faild");
			return false;
		}
		
		PE("m_vAnswerIndex:"<<m_vAnswerIndex.size())
		PE("m_Tfidf.m_word_idx:"<<m_Tfidf.m_word_idx.size())
		PE("m_Tfidf.m_word_frequency:"<<m_Tfidf.m_word_frequency.size())
		
		return true;
	}
	
	bool compile(const acl::FileName& sRawDataFile, const acl::FileName& sBinDataFile){
		if(readFile(sRawDataFile) == false){
			PE("Read raw data file ["<<sRawDataFile<<"] failed");
			return false;
		}
		
		std::map<std::string,int>::const_iterator iter;
		std::vector<TfidfValue> vTfidfValue;
		
		//分词并初始化TFIDF类
		for(size_t i=0,size=m_vCacheQuestion.size(); i<size; i++){
			std::vector<std::string> vWord;
			std::map<std::string,std::vector<std::string> > subSentence;
			
			(reinterpret_cast<cppjieba::Jieba const*>(cppjiebaParser))->CutForSearchEx(m_vCacheQuestion[i],vWord,subSentence,true,true);
			m_vvCacheWord.push_back(vWord);
			
			m_Tfidf.m_questionNum += 1;
			//int appeared = i;
			std::set<int> appeared;
			for(size_t k=0; k<vWord.size(); k++){
				iter = m_Tfidf.m_word_idx.find(vWord[k]);
				if(iter == m_Tfidf.m_word_idx.end()){
					m_Tfidf.m_word_idx.insert(std::make_pair(vWord[k],m_Tfidf.m_word_idx.size()));
					m_Tfidf.m_word_frequency.insert(std::make_pair(m_Tfidf.m_word_idx.size(),1));
					//appeared += 1;
					appeared.insert(m_Tfidf.m_word_idx.size());
				}else{
					if(appeared.find(iter->second) == appeared.end()){
						m_Tfidf.m_word_frequency[iter->second]++;
						appeared.insert(iter->second);
					}
				}
			}
			//P("P:"<<i<<"/"<<size)
		}
		
		//计算所有录入问题的 TFIDF 形式
		for(size_t i=0,size=m_vvCacheWord.size(); i<size; i++){
			const std::vector<std::string>& vWord = m_vvCacheWord[i];
			std::vector<std::string> vOutWord;
			const std::vector<int> vWordIdx = m_Tfidf.transWords(vWord,vOutWord);
			vTfidfValue = m_Tfidf.caculate(vWordIdx);
			vvTfidfValue.push_back(vTfidfValue);
			//P("T:"<<i<<"/"<<size)
		}
		
		//删除内存冗余项
		m_vCacheQuestion.clear();
		m_vCacheQuestion.resize(0);
		for(size_t i=0; i<m_vvCacheWord.size(); i++){
			m_vvCacheWord[i].clear();
			m_vvCacheWord[i].resize(0);
		}
		m_vvCacheWord.clear();
		m_vvCacheWord.resize(0);
		
		//写入序列化Data文件
		if(Serialization::writeData(*this,sBinDataFile) == false){
			PE("Write serialize data faild");
			return false;
		}
		
		return true;
	}
	
	std::string search(const std::vector<std::string>& vWord){
		// 转换字符串单词到数字单词
		std::vector<std::string> vOutWord;
		std::vector<int> vWordIdx = m_Tfidf.transWords(vWord,vOutWord);
		std::vector<TfidfValue> vInTfidf = m_Tfidf.caculate(vWordIdx);
		
		// 匹配
		size_t nIdx = std::string::npos;
		float tmpScore = 0.0;
		for(size_t i=0; i<vvTfidfValue.size(); i++){
			float fScore = matchByTfidf(vInTfidf,vvTfidfValue[i]);
			if(fScore > 0.618){
				if(fScore > tmpScore){
					tmpScore = fScore;
					nIdx = i;
				}
			}
		}
		
		//获取答案
		if(nIdx != std::string::npos){
			return getAnswer(nIdx);
		}else{
			return "";
		}
	}
	
	float matchByTfidf(const std::vector<TfidfValue>& tfidfValues1, const std::vector<TfidfValue>& tfidfValues2){
		float fScore;
		std::map<int,float> mTfidf;
		std::map<int,float>::const_iterator iter;
		std::set<TfidfValue> setInTfidf;
		float numerator = 0.f;                    // consine 相似度分子

		for(size_t i=0; i< tfidfValues1.size(); i++){
			mTfidf.insert(std::make_pair(tfidfValues1[i].m_nID,tfidfValues1[i].m_nValue));
		}

		for(size_t i=0; i< tfidfValues2.size(); i++){   //问题中每个分词的Tfidf值
			iter = mTfidf.find(tfidfValues2[i].m_nID);
			if(iter != mTfidf.end()){
				numerator += tfidfValues2[i].m_nValue * iter->second; //关键词中每个分词的Tfidf值和问题中的相乘
			}
		}

		float squareA = 0.f, sqrtA;
		float squareB = 0.f, sqrtB;
		for(size_t i=0; i<tfidfValues2.size(); i++){
			squareA += tfidfValues2[i].m_nValue * tfidfValues2[i].m_nValue;
		}
		for(size_t i=0; i<tfidfValues1.size(); i++){
			squareB += tfidfValues1[i].m_nValue * tfidfValues1[i].m_nValue;
		}
		sqrtA = sqrt(squareA);
		sqrtB = sqrt(squareB);
		float denominator = sqrtA * sqrtB;  // consine 相似度计算的分母
		
		if (fabsf(denominator - 0) < 0.000001){
			return 0.f;
		}

		float consineSim = numerator / denominator;    // 欧氏距离的 consine 相似度
		fScore = consineSim;

		return fScore;
	}
	
	std::string getAnswer(size_t nIdx){
		FILE *fp = fopen(m_sFileName.c_str(),"r");
		if(fp == NULL){
			PRINT("Can't open file '"<<m_sFileName<<"'.")
			return "";
		}
		fseek(fp,m_vAnswerIndex[nIdx],SEEK_SET);
		
		std::string sLine;
		while(1){
			char ch = fgetc(fp);
			if(ch == '\n') break;
			sLine += ch;
		}
		fclose(fp);
		
		return sLine;
	}
public:
	template <class Format, class Refs>
	inline void composite(obj_input<Format, Refs>& stream) {
		stream.simple(m_sFileName);
		stream.content(m_Tfidf);
		
		int size = 0;
		Format &format = stream.get_format();
		format.input_start_array(size);
		for(size_t i = 0; i <size; i++) {
			unsigned long e;
			stream.simple(e);
			m_vAnswerIndex.push_back(e);
		}
		
		format.input_start_array(size);
		for(size_t i = 0; i <size; i++) {
			size_t size1 = 0;
			format.input_start_array(size1);
			std::vector<TfidfValue> vTfidfValue;
			for(size_t j = 0; j < size1; j++) {
				TfidfValue val;
				stream.content(val);
				vTfidfValue.push_back(val);
			}
			vvTfidfValue.push_back(vTfidfValue);
		}
	}
	
	template <class Format, class Refs>
	inline void composite(obj_output<Format, Refs>& stream) {
		stream.simple(m_sFileName);
		stream.content(m_Tfidf);
		
		int size = m_vAnswerIndex.size();
		Format &format = stream.get_format();
		format.output_start_array(size);
		for(size_t i = 0; i < size; i++) {
			stream.simple(m_vAnswerIndex[i]);
		}
		format.output_end_array();
		
		size = vvTfidfValue.size();
		Format &format1 = stream.get_format();
		format.output_start_array(size);
		for(size_t i = 0; i < size; i++) {
			size_t size1 = vvTfidfValue[i].size();
			Format &format2 = stream.get_format();
			format2.output_start_array(size1);
			for(size_t j = 0; j < size1; j++) {
				stream.content(vvTfidfValue[i][j]);
			}
			format2.output_end_array();
		}
		format1.output_end_array();
	}
	
private:
	bool readFile(const std::string& sRawDataFile){
		std::ifstream ifs(sRawDataFile.c_str());
		if(!ifs){
			PE("Can't open file ["<<sRawDataFile<<"] !")
			return false;
		}
		
		acl::Line line;
		unsigned long nOffset=0;
		while(!ifs.eof()){
			std::getline(ifs,line);
			if(line.empty()){
				nOffset++;
				continue;
			}
			
			std::vector<std::string> v = line.separate("\t");
			if(v.size() != 2){
				nOffset += line.length() + 1 ;
				continue;
			}
			
			m_vCacheQuestion.push_back(v[0]);
			m_vAnswerIndex.push_back(nOffset+v[0].length()+1);
			
			nOffset += line.length() + 1;
		}
		
		ifs.close();
		
		m_sFileName = sRawDataFile;
		
		return true;
	}
	
private:
	std::string m_sFileName;
	Tfidf m_Tfidf;
	std::vector<std::string> m_vCacheQuestion;
	std::vector<std::vector<std::string> > m_vvCacheWord;
	std::vector<std::vector<TfidfValue> > vvTfidfValue;
	std::vector<unsigned long> m_vAnswerIndex;
};

#endif //__KNOWLEDGE_BASE__HPP__
