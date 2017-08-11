#ifndef __ROBOSAY__COMMON__KEYWORDANALYSE__HPP__
#define __ROBOSAY__COMMON__KEYWORDANALYSE__HPP__

#include "Common/lib4/String.hpp"
#include "CustomerServiceChat/GeneralChat/WordPropertyDef.h"


namespace robosay{
	using namespace general;

class KeyWordAnalyse
{
public:

const std::vector< std::vector<std::string> >& analyse(const std::string& keyWord, std::vector<std::string>& vWords, std::map<WP_TYPE,std::set<std::string> >&mWordProperty){

	std::vector<std::string> result;
	
	m_keyWord.clear();
	m_result.clear();

	acl::String strTemp(keyWord);
	strTemp.toUpper();
	
	if(false == parse(strTemp,vWords,mWordProperty)){
		return m_result;
	}
	
	doRecursion(m_keyWord.begin(),result,m_keyWord.end());

	return m_result;
}

bool parse(const std::string& keyWord, std::vector<std::string>& vWords, std::map<WP_TYPE,std::set<std::string> >&mWordProperty){
	std::string::size_type strSize = 0, indexBegin = 0;
	bool currentAdded = false;
	acl::String temp;
	
	
	for(;strSize<keyWord.size();strSize++){
		acl::String StringWord(keyWord.substr(indexBegin,strSize-indexBegin));
		StringWord.strip();/*去掉 开头和结果的空格，tab，换行符*/
		temp = StringWord;
		if(keyWord[strSize] == ','){
			indexBegin = strSize+1;
			if(temp.empty()) {
				currentAdded = false;
				continue;
			}

			if(!checkKeyWord(temp,mWordProperty))
				return false;
			vWords.push_back(temp);
			temp.removeChar(' ');
			if(currentAdded){
				m_keyWord[m_keyWord.size()-1].push_back(temp);
			}
			else{
				std::vector<std::string> vec;
				vec.push_back(temp);
				m_keyWord.push_back(vec);
			}
			
			currentAdded = false;
		}
		else if(keyWord[strSize] == '/'){
			indexBegin = strSize+1;
			if(temp.empty()) {
				continue;
			}

			if(!checkKeyWord(temp,mWordProperty))
				return false;
			
			vWords.push_back(temp);
			temp.removeChar(' ');
			if(currentAdded){
				m_keyWord[m_keyWord.size()-1].push_back(temp);
			}
			else{
				std::vector<std::string> vec;
				vec.push_back(temp);
				m_keyWord.push_back(vec);
			}
			
			currentAdded = true;
		}
	}
	
	if(indexBegin<keyWord.size()){
		temp = keyWord.substr(indexBegin);
		if(!checkKeyWord(temp,mWordProperty))
			return false;		
		vWords.push_back(temp);
		temp.removeChar(' ');
		if(currentAdded){
			m_keyWord[m_keyWord.size()-1].push_back(temp);
		}
		else{
			std::vector<std::string> vec;
			vec.push_back(temp);
			m_keyWord.push_back(vec);
		}
	}

	return true;
}

private:
	void doRecursion(std::vector< std::vector<std::string> >::const_iterator it, std::vector<std::string> &result, std::vector< std::vector<std::string> >::const_iterator end){
		if(it== end)
		{
			if(!result.empty()){
				m_result.push_back(result);
				
				#if 0
				for(size_t i=0;i<result.size();i++)
					std::cout<<result[i]<<":";
				std::cout<<std::endl;
				#endif
			}
			return ;
		}
		

		const std::vector<std::string> &vec = *it;
		std::vector<std::string>::const_iterator itVector = vec.begin();
		for(;itVector!=vec.end();itVector++){
			std::vector< std::vector<std::string> >::const_iterator itTemp = it+1;
			result.push_back(*itVector);
			doRecursion(itTemp,result,end);
			result.pop_back();	
			
		}
	}

	bool checkKeyWord(std::string& keyword,std::map<WP_TYPE,std::set<std::string> >&mWordProperty){
		size_t loc = 0;
		std::string errorString1("，"), errorString2("、");
		loc = keyword.find(errorString1) ;
		if(loc !=  std::string::npos){
			std::cout<<"keyword happan undefine sysbol 1."<<std::endl;	
			return false;
		}

		loc = keyword.find(errorString2) ;
		if(loc !=  std::string::npos){
			std::cout<<"keyword happan undefine sysbol 2."<<std::endl;	
			return false;
		}
		
		
		WP_TYPE result = getKeyWordProperty(keyword); 
		if(result != WP_Buff){
			mWordProperty[result].insert(keyword);
		}	
		else
		{
			std::cout<<"keyword happan error.the no property string is "<<keyword<<std::endl;				
		}
		return true;
	}

	WP_TYPE getKeyWordProperty(std::string& keyword){
		const char cbegin = '(',  cEnd=')';
		int result = -1;
		size_t iEnd,iBegin=0;
		iBegin = keyword.find(cbegin);

		if(iBegin == std::string::npos)
			return 	WP_Noun;

		iEnd = keyword.find(cEnd,iBegin+1);

		if(iEnd == std::string::npos || (iEnd != keyword.size()-1))
			return 	WP_Buff;		

		std::string wordProperty = keyword.substr(iBegin+1,iEnd-iBegin-1);
		
		keyword = keyword.substr(0,iBegin);

		//if(wordProperty == "n" || wordProperty == "N"){
		//	return scWordType;
		//}
		((acl::String&)wordProperty).strip();
		return WordProperty(wordProperty);
	}

	WP_TYPE WordProperty(const std::string& strWordProperty)
	{
		if((strWordProperty=="d") || (strWordProperty=="D")){
			return WP_Verb;
		}

		if((strWordProperty=="x") || (strWordProperty=="X")){
			return WP_Qualifier;
		}

		return WP_Buff;
	}

public:
	std::vector< std::vector<std::string> > m_keyWord;
	std::vector< std::vector<std::string> > m_result;
	//static const int scWordType = 0;  //0代表该词是修饰词，比如高低等等。这类词语可能修饰名词或者动词
};

}//namespace robosay

#endif //__ROBOSAY__COMMON__KEYWORDANALYSE__HPP__
