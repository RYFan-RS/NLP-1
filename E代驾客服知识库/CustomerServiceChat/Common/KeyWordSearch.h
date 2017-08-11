#ifndef __ROBOSAY__COMMON__KEYWORDSEARCH__HPP__
#define __ROBOSAY__COMMON__KEYWORDSEARCH__HPP__

#include "Common/libxml2/XmlParser.hpp"
#include "CustomerServiceChat/Common/KBAttribute.h"
#include "SentenceParser/MixParser/Parser.h"
#include "SemanticAnalysis/RuleAnalysis/mean.h"
#include "Common/lib4/CommonDef.h"
#include "CustomerServiceChat/GeneralChat/TFIDF.h"
#include "CustomerServiceChat/GeneralChat/WordSubdivisionDic.h"
#include "CustomerServiceChat/GeneralChat/WordCombination.h"
#include "CustomerServiceChat/ErrorCorrectionHandle.h"
#include "CustomerServiceChat/GeneralChat/WordNearMeaning.h"

#include "CustomerServiceChat/Common/InvertIndex.h"


namespace robosay{
using namespace aisms;
using namespace general;
class KeyWordSearch 
{
public:
	KeyWordSearch():m_bInit(false)
	{
		m_TFIDF.m_questionNum = 0;
	}

/*初始化一次*/
bool init(const acl::FileName& sXmlFile, std::string sCommonDicDir = ""){

	if(m_bInit) return true;
	
    if(sCommonDicDir.empty()){
        return false;
    }

	#if 0
	if(m_mixParser.init(sXmlFile,true, sCommonDicDir+ "/",false) == false){
		PRINT("KeyWordSearch Init mix parser error!")
		return false;
	}
	#endif

	aisms::mxpsr::Parser::initStaticParser(sCommonDicDir + "/" +"NewWord.dic");

	acl::File stopWordFile;

	acl::FileName sDicPath = sCommonDicDir + "/" + "StopWord.dic";

//#ifdef USE_ENCODE_DECODE
#if 0
	StringVector vWordsDic;
	acl::FileEncoder encoder;
	std::string sFileFullPath = sDicPath.getBaseName()+".dat";
	encoder.decodeFile(sFileFullPath, vWordsDic);
	for(size_t j=0; j<vWordsDic.size(); j++){
		std::string line = vWordsDic[j];
#else
	stopWordFile.setCommentMarks("#");

	stopWordFile.read(sDicPath);
	for(size_t j=0; j<stopWordFile.size(); j++){
		stopWordFile[j].toUpper();
		stopWordFile[j].strip();
#endif
		m_stopWord.insert(stopWordFile[j]);
	}

	
	/*待细分词功能不需要
	if (!m_wordSubdivisionDic.Initialize(sCommonDicDir + "/" + "WordSubdivisionDic.dic"))
	{
		PRINT("KeyWordSearch Init wordSubdivisionDic error!");
	}
	*/
	m_WordNearMeaningHandle.initNormalFile(sCommonDicDir + "/" + "SameWord.dic");
	
	if (!m_errorCorrectionHandle.Initialize(sCommonDicDir + "/" + "ErrorCorrectionWords.dic"))
	{
		PRINT("KeyWordSearch Init errorCorrectionHandle error!");
	}

	m_bInit = true;
	
	return true;
}

#if 0
void addWords(const std::vector<std::string> &wordVec){
	std::vector<std::string>::const_iterator it = wordVec.begin();
	for(;it!=wordVec.end();it++){
		m_wordDic.insert(*it);
	}
}

void addWords(const std::set<std::string> &wordVec){
	std::set<std::string>::const_iterator it = wordVec.begin();
	for(;it!=wordVec.end();it++){
		m_wordDic.insert(*it);
	}
}

void generateDictionary(){	
	std::vector<std::string> vecWordDic;
	std::ofstream ofs("ContainSpaceKeyWordInfo.txt");
	CWordCombination wordCom(' ');
	

	std::set<std::string>::const_iterator it = m_wordDic.begin();
	for(;it!=m_wordDic.end();it++)
	{
		if (std::string::npos == it->find(' '))
		{			
			vecWordDic.push_back(*it);
		}
		else
		{
			//output 含有空格的分词
			if (ofs) { ofs << *it << '\n'; }
			//获取含有空格的分词组合，添加到分词词典中
			wordCom.Combination(*it);
			wordCom.CopyResult(std::back_inserter(vecWordDic));
		}
	}
	
	m_mixParser.getDynamicParser().init("UserDic",vecWordDic);
}

std::vector<std::string> parse(const std::string &input){

	
	//#define IS_USER_DYNAMIC_PARSER
#ifdef IS_USER_DYNAMIC_PARSER	
	std::vector<std::string> vGetWord = m_mixParser.getDynamicParser().parse(input,"UserDic").getWords();
#else
	std::vector<std::string> vGetWord = m_mixParser.parse(input).getWords();
#endif
	
	//std::vector<std::string> vGetWord = m_mixParser.getDynamicParser().parse(input,"UserDic").getWords();

#if 0
	StringVector::iterator it = vGetWord.begin();
	for(; it!=vGetWord.end();){
		((acl::String&)*it).removeChar(' ');
		if(0!= m_stopWord.count(*it)){
			it = vGetWord.erase(it);
			continue;
		}
		it++;
	}
#endif
	//m_wordSubdivisionDic.SubdivideWord(vGetWord);

	return vGetWord;
}
void TestOutputWordNumber()const
{
	m_TFIDF.TestOutputWordNumber();
}


std::vector<TfidfValue> getQuestionTfIdf(std::string input){
	std::vector<TfidfValue> vInTfidf;
	std::vector<std::string> sOutput;

	const std::vector<std::string>& vecWord = parse(input);

	std::cout << "\n---------------------------------------------" << std::endl;
	for (std::vector<std::string>::const_iterator iter=vecWord.begin(); iter!=vecWord.end(); ++iter)
	{
		std::cout<< *iter << "/";
	}	
	std::vector<int> vWordIdx = m_TFIDF.transWords(vecWord,sOutput);

	std::cout << "\n---------------------------------------------" << std::endl;
	for (std::vector<std::string>::const_iterator iter=sOutput.begin(); iter!=sOutput.end(); ++iter)
	{
		std::cout<< *iter << "/";
	}
	std::cout << std::endl;	
	
	vInTfidf = m_TFIDF.caculate(vWordIdx); //关键词中每个分词的Tfidf值

	return vInTfidf;
}
#endif
	void filterStopWord(std::vector<std::string> & words){
		std::vector<std::string> temp;
		std::vector<std::string>::const_iterator itr = words.begin();
		for(;itr!=words.end();itr++){
			if(m_stopWord.find(*itr)==m_stopWord.end()){
				temp.push_back(*itr);
			}
		}
		words.swap(temp);
	}

	std::vector<std::vector<std::string> >getNearWordsSegmentMethod(const std::vector<std::string>& srcWords,
			const std::map<std::string,std::vector<std::string> >& subSentence,int maxNumber){
		std::vector<std::string> words(srcWords);

		std::vector<std::vector<std::string> >  result;
		std::vector<std::string>::const_iterator itr ;
		std::map<std::string,std::vector<std::string> >::const_iterator mapItr;
		
		if(!m_WordNearMeaningHandle.process(words)){
			return result;
		}

		int number = 0;
		
		for(itr=words.begin();itr!=words.end();itr++){
			mapItr = subSentence.find(*itr);
			if(mapItr != subSentence.end()){
				if(number< mapItr->second.size()){
					number = mapItr->second.size();
				}
			}
		}
		
		for(int i=1;i<=number;i++){
			std::vector<std::string> wordsId;
			for(itr=words.begin();itr!=words.end();itr++){
				mapItr = subSentence.find(*itr);
				if(mapItr != subSentence.end()){
					int index = mapItr->second.size()-1;
					if((i-1) <mapItr->second.size()){
						index = i-1;
					}
					acl::String realText(mapItr->second.at(index)); 
					std::vector<std::string> strTexts = realText.separate("\t");
					wordsId.insert(wordsId.end(), strTexts.begin(), strTexts.end());
				}
				else
				{
					wordsId.push_back(*itr);
				}
			}
			if(wordsId.size()!=0){
				result.push_back(wordsId);
			}
		}


		result.push_back(words);
		if(result.size()>maxNumber){
			result.resize(maxNumber);
		}

		return result;
	}
	
    std::vector<std::vector<std::string> > getAllSegmentMethod(const std::vector<std::string>& words,
			const std::map<std::string,std::vector<std::string> >& subSentence,int maxNumber = KeyWordSearch::maxNumberSegmentMethods,
			int maxNearWordsNumber = KeyWordSearch::maxNumberNearWordsSegmentMethods){
		std::vector<std::string>::const_iterator itr ;
		std::map<std::string,std::vector<std::string> >::const_iterator mapItr;
		
		std::vector<std::vector<std::string> >  result;
			
		int number = 0;
		
		for(itr=words.begin();itr!=words.end();itr++){
			mapItr = subSentence.find(*itr);
			if(mapItr != subSentence.end()){
				if(number< mapItr->second.size()){
					number = mapItr->second.size();
				}
			}
		}

		result.push_back(words);
		
		std::vector<std::vector<std::string> > vNearWordsSegmentMethods = getNearWordsSegmentMethod(words,subSentence,maxNearWordsNumber);
		result.insert(result.end(),vNearWordsSegmentMethods.begin(),vNearWordsSegmentMethods.end());

		if(maxNumber>=maxNearWordsNumber){
			maxNumber=maxNumber-maxNearWordsNumber+vNearWordsSegmentMethods.size();
		}
		for(int i=1;i<=number;i++){
			std::vector<std::string> wordsId;
			for(itr=words.begin();itr!=words.end();itr++){
				mapItr = subSentence.find(*itr);
				if(mapItr != subSentence.end()){
					int index = mapItr->second.size()-1;
					if((i-1) <mapItr->second.size()){
						index = i-1;
					}
					acl::String realText(mapItr->second.at(index)); 
					std::vector<std::string> strTexts = realText.separate("\t");
					wordsId.insert(wordsId.end(), strTexts.begin(), strTexts.end());
				}
				else
				{
					wordsId.push_back(*itr);
				}
			}
			if(wordsId.size()!=0){
				result.push_back(wordsId);
			}
		}

		if(result.size()>maxNumber){
			result.resize(maxNumber);
		}
		
		#if 0
		printStringVector(result);
		#endif
		
		return result;
	}

	std::map<int,std::string> getQuestionTfIdfValue(std::vector<std::vector<TfidfValue> >& tfIdfVec,const std::vector<std::string>& words,
					const std::map<std::string,std::vector<std::string> >& subSentence){
		std::vector<std::vector<std::string> >  result = getAllSegmentMethod(words,subSentence,maxNumberQuestionSegmentMethods,1);
		//std::vector<std::string> sOutput;
		std::vector<TfidfValue> vTfidfValue;
		std::map<int,std::string>  mapIdToWord;

		tfIdfVec.clear();
		
		for(size_t j=0; j<result.size(); j++){
			
				
			StringVector& vWord = result[j];
			filterStopWord(vWord);

			if(vWord.size()==0){
				continue;
			}
			std::vector<int> vWordIdx = m_TFIDF.transWords(vWord,mapIdToWord);
			vTfidfValue = m_TFIDF.caculate(vWordIdx);
			tfIdfVec.push_back(vTfidfValue);
		}

		printStringVector(result);

		return  mapIdToWord;
	}

	static void printStringVector(const std::vector<std::vector<std::string> >& vecVecString){
		std::vector<std::vector<std::string> >::const_iterator itr = vecVecString.begin();
		for(int j=1;itr!=vecVecString.end();itr++,j++){
			std::cout<<"第"<<j<<"个元素  ";
			for(int i=0;i<itr->size();i++){
				std::cout<<itr->at(i)<<";";
			}
			std::cout<<std::endl;
		}
	}

	std::vector<std::string> getKeyWords(const std::vector<std::string>& sentence) {
		robosay::base::CSortManager<std::string>  sortManager;
		
		std::vector<std::string> result;

		std::vector<std::string>::const_iterator itr = sentence.begin();

		for(;itr!=sentence.end();itr++){
			acl::String temp = *itr;
			std::vector<std::string> words = temp.toCharactor();
			if(words.size()==0){
				continue;
			}
			sortManager.Add(words.size(),*itr);
		}
		sortManager.Sort();

		const std::vector< robosay::base::CSortManager<std::string>::NodeElem >& elems = sortManager.GetAllElems();
		
		std::vector<robosay::base::CSortManager<std::string>::NodeElem>::const_iterator iter = elems.begin();
		
		while (iter != elems.end())
		{
			result.push_back(iter->GetElem());
			++iter;
		}

		return result;
	}
public:
	mxpsr::Parser m_mixParser;
	//std::set<std::string>  m_wordDic;
	general::TFIDF         m_TFIDF;
	bool 	m_bInit;
	std::set<std::string>  m_stopWord;
	general::InvertIndex   m_InvertIndex;
	//CWordSubdivisionDic	   m_wordSubdivisionDic;
	base::CErrorCorrectionHandle m_errorCorrectionHandle;
	general::WordNearMeaning	m_WordNearMeaningHandle;
	static const int maxNumberSegmentMethods = 6;
	static const int maxNumberNearWordsSegmentMethods = 2;	
	static const int maxNumberQuestionSegmentMethods = 3;	
};

}//namespace robosay

#endif //__ROBOSAY__COMMON__KEYWORDSEARCH__HPP__
