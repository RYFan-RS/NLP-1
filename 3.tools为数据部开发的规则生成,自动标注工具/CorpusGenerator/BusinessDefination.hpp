#ifndef __SAE__BUSINESS_DEFINATION__HPP__
#define __SAE__BUSINESS_DEFINATION__HPP__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <iostream>
#include <vector>
#include "Utility.h"
#include "lib4/Utility.hpp"
#include "lib4/Definations.hpp"

namespace sae
{

class BusinessDefination
{
public:
	bool parse(const FILE& file, size_t nIdx, const std::vector<FILE>& vFiles, ErrorData& errData){
		std::vector<std::string> vBusiness;
		
		for(int i=0,size = file.m_file.size(); i<size; i++){
			if(file.m_file[i].m_line.empty()) continue;//空行
			
			size_t nPos = file.m_file[i].m_line.find("#");
			
			if(nPos == 0){ //领域声明行
				std::vector<std::string> vKey = acl::Utility::separate(file.m_file[i].m_line,"#");
				vBusiness.clear();
				for(size_t j=0,size=vKey.size(); j<size; j++){
					vBusiness.push_back(vKey[j]);
				}
			}else{
				for(size_t j=0,size=vBusiness.size(); j<size; j++){
					std::stringstream ss;
					ss<<nIdx<<vBusiness[j];
					std::map<std::string,std::vector<std::string> >::iterator iter = m_mIdx2BizWords.find(ss.str());
					if(iter == m_mIdx2BizWords.end()){
						std::vector<std::string> vWord;
						vWord.push_back(file.m_file[i].m_line);
						m_mIdx2BizWords.insert(std::make_pair(ss.str(),vWord));
					}else{
						iter->second.push_back(file.m_file[i].m_line);
					}
					m_mWord2Index.insert(std::make_pair(file.m_file[i].m_line,nIdx));
				}
			}
		}
		
		return true;
	}
	
	std::string getWordsString(const std::string& sWord, const std::string& sBussiness){
		std::string sRet;
		
		std::map<std::string,size_t>::const_iterator iter = m_mWord2Index.find(sWord);
		if(iter == m_mWord2Index.end()){
			return sRet;
		}
		
		size_t nIdx = iter->second;
		std::stringstream ss;
		ss<<nIdx<<sBussiness;
		std::map<std::string,std::vector<std::string> >::const_iterator iter1 = m_mIdx2BizWords.find(ss.str());
		if(iter1 == m_mIdx2BizWords.end()){
			return sRet;
		}else{
			const std::vector<std::string>& vWord = iter1->second;
			for(size_t i=0,size=vWord.size(); i<size; i++){
				if(i!=0) sRet += "/";
				sRet += vWord[i];
			}
		}
		
		return sRet;
	}
	
	void dump(){
		std::map<std::string,size_t>::const_iterator iter;
		for(iter=m_mWord2Index.begin(); iter!=m_mWord2Index.end(); ++iter){
			P(iter->first<<"--"<<iter->second)
		}
	}
private:
	std::map<std::string,size_t> m_mWord2Index;
	std::map<std::string,std::vector<std::string> > m_mIdx2BizWords;
};

}//namespace sae

#endif //__SAE__BUSINESS_DEFINATION__HPP__
