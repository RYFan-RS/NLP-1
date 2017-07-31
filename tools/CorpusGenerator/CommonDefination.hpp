#ifndef __SAE__COMMON_DEFINATION__HPP__
#define __SAE__COMMON_DEFINATION__HPP__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <iostream>
#include <vector>
#include <map>
#include <iterator>
#include "ErrorData.h"
#include "Utility.h"

namespace sae
{

class CommonDefination
{
public:
	CommonDefination(){
		m_pLineIndexManager = NULL;
		m_pFiles = NULL;
	}
	
	void setLineIndexManagerPointer(LineIndexManager* pLineIndexManager){
		m_pLineIndexManager = pLineIndexManager;
	}
	
	void setFilesPointer(std::vector<FILE>* pFiles){
		m_pFiles = pFiles;
	}
	
	bool parse(FILE& file, size_t nIdx, const std::vector<FILE>& vFiles, ErrorData& errData){
		std::map<std::string,size_t>::const_iterator iter; 
		std::vector<LINE> vLines;
		
		for(int i=0,size = file.m_file.size(); i<size; i++){
			if(file.m_file[i].m_line.empty()) continue;//空行
			
			size_t nPos = file.m_file[i].m_line.find("!");
			if(nPos == 0){ //注释行
				continue;
			}else if(nPos == std::string::npos){
				iter = m_mWord2Index.find(file.m_file[i].m_line);
				if(iter != m_mWord2Index.end()){
					errData.m_errMsg = "通用集合["+file.m_name+"]中的单词["+file.m_file[i].m_line
										+"]与["+vFiles[iter->second].m_name+"]中的单词定义冲突";
					errData.errorTrace(__FILE__,__LINE__-1);
					return false;
				}else{
					vLines.push_back(file.m_file[i]);
					m_mWord2Index.insert(std::make_pair(file.m_file[i].m_line,nIdx));
				}
			}else{
				std::string sWord = file.m_file[i].m_line.substr(0,nPos);
				std::string sException = file.m_file[i].m_line.substr(nPos=1);
				iter = m_mWord2Index.find(sWord);
				if(iter != m_mWord2Index.end()){
					errData.m_errMsg = "通用集合["+file.m_name+"]中的单词["+sWord
										+"]与["+vFiles[iter->second].m_name+"]中的单词定义冲突";
					errData.errorTrace(__FILE__,__LINE__-1);
					return false;
				}else{
					file.m_file[i].m_line = sWord;
					file.m_file[i].m_defination_excepiton = sException;
					vLines.push_back(file.m_file[i]);
					m_mWord2Index.insert(std::make_pair(file.m_file[i].m_line,nIdx));
				}
			}
		}
		
		file.m_file.swap(vLines);
		
		return true;
	}
	
	std::string getWordsString(const std::string& sWord, const std::string& sBussiness){
		std::string sRet;
		
		std::map<std::string,size_t>::const_iterator iter = m_mWord2Index.find(sWord);
		if(iter == m_mWord2Index.end()){
			return sRet;
		}
		
		const std::vector<LINE>& vLines = (m_pFiles->at(iter->second)).m_file;
		for(size_t i=0,size=vLines.size(); i<size; i++){
			if(vLines[i].m_defination_excepiton == sBussiness) continue;
			if(!sRet.empty()) sRet += '/';
			sRet += vLines[i].m_line;
		}
		
		return sRet;
	}
private:
	std::map<std::string,size_t> m_mWord2Index;
	LineIndexManager* m_pLineIndexManager;
	std::vector<FILE>* m_pFiles;
};

}//namespace sae

#endif //__SAE__COMMON_DEFINATION__HPP__
