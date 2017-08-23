#ifndef __SAE__VARIABLE_DEFINATION__HPP__
#define __SAE__VARIABLE_DEFINATION__HPP__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <iostream>
#include <vector>
#include "Utility.h"

namespace sae
{

class VariableDefination
{

public:
	VariableDefination(){
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
			
			size_t nPos = file.m_file[i].m_line.find("#");
			if(nPos == 0){ //注释行
				continue;
			}else if(nPos == std::string::npos){
				vLines.push_back(file.m_file[i]);
			}else{
				std::string sWord = file.m_file[i].m_line.substr(0,nPos);
				file.m_file[i].m_line = sWord;
				vLines.push_back(file.m_file[i]);
			}
		}
		
		file.m_file.swap(vLines);
		m_mName2Index.insert(std::make_pair(Utility::getStringBetween(file.m_name,"/","."),nIdx));
		
		return true;
	}
	
	bool hasVariable(std::string sName){
		std::map<std::string,size_t>::const_iterator iter = m_mName2Index.find(sName);
		return iter == m_mName2Index.end() ? false : true;
	}
	
	std::string getWordsString(std::string sName){
		std::string sRet;
		
		std::map<std::string,size_t>::const_iterator iter = m_mName2Index.find(sName);
		if(iter == m_mName2Index.end()){
			return sRet;
		}
		
		const std::vector<LINE>& vLines = (m_pFiles->at(iter->second)).m_file;
		for(size_t i=0,size=vLines.size(); i<size; i++){
			if(i != 0) sRet += '/';
			sRet += vLines[i].m_line;
		}
		
		return sRet;
	}
	
	void dump(std::ostream& ofs = std::cout) const{
		std::map<std::string,size_t>::const_iterator iter;
		for(iter=m_mName2Index.begin(); iter!=m_mName2Index.end(); ++iter){
			ofs << iter->first << ":" << iter->second << std::endl;
		}
	}
private:
	std::map<std::string,size_t> m_mName2Index;
	LineIndexManager* m_pLineIndexManager;
	std::vector<FILE>* m_pFiles;
};

}//namespace sae

#endif //__SAE__VARIABLE_DEFINATION__HPP__
