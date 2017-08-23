#ifndef __INDEX_READER__HPP__
#define __INDEX_READER__HPP__
#include "lib4/FileName.hpp"
#include "lib4/String.hpp"
#include "lib4/File.hpp"
#include "lib4/Definations.hpp"

class IndexElement{
public:
	size_t nLine;
	std::string sTitle;
	acl::FileName sFileName;
};

class IndexReader{
public:
	IndexReader(const char* sFileName){
		m_nCurIdx = 0;
		m_sFileName = sFileName;
	}
	
	bool read(){
		acl::File file;
		
		if(file.read(m_sFileName) == false){
			PE("Read index file ["<<m_sFileName<<"] failed")
			return false;
		}
		
		for(size_t i=0,size=file.size(); i<size; i++){
			const acl::Line& line = file[i];
			if(line.empty()) continue;
			
			std::vector<std::string> vElement = line.separate("\t");
			if(vElement.size() != 4 && vElement.size() != 3){
				PE("vElement.size()="<<vElement.size())
				PE("Index file line ["<<(i+1)<<"] format error")
				return false;
			}
			
			if(vElement.size()==3){
				IndexElement element;
				element.nLine = i+1;
				element.sTitle = vElement[0];
				element.sFileName = m_sFileName.getPath()+"/"+vElement[2];
				m_vElements.push_back(element);
			}
		}
		
		return true;
	}
	
	void setCurIdx(size_t nLine){
		m_nCurIdx = nLine;
	}
	
	acl::FileName getFilePath(){
		size_t size = m_vElements.size();
		if(m_nCurIdx < size){
			PE("Processing "<<m_nCurIdx<<"/"<<size)
			return m_vElements[m_nCurIdx++].sFileName;
		}else{
			return "";
		}
	}
private:
	size_t m_nCurIdx;
	std::vector<IndexElement> m_vElements;
	acl::FileName m_sFileName;
};

#endif //__INDEX_READER__HPP__
