#ifndef __ROBOSAY__COMMON__KB_ATTRIBUTE__HPP__
#define __ROBOSAY__COMMON__KB_ATTRIBUTE__HPP__
#include "Common/libxml2/XmlParser.hpp"

namespace robosay{

class KBAttribute
{
public:
	KBAttribute(){
		m_nKBIdx = -1;
	}
	
	bool init(const acl::FileName& sXmlFile, XmlParser& xmlKB){
		// 获取知识库文件名
		m_sFileName = sXmlFile;
		
		// 获取知识库名称
		m_sKBName = xmlKB.getAttribute(xmlKB.getRootNode("scml"),"name");
		if(m_sKBName.empty()){
			return false;
		}
		// 获取知识库名称
		m_sKBType = xmlKB.getAttribute(xmlKB.getRootNode("scml"),"type");
		if(m_sKBType.empty()){
			return false;
		}

        // 获取知识库名称
		m_sKBIndustry = xmlKB.getAttribute(xmlKB.getRootNode("scml"),"industry");
		if(m_sKBIndustry.empty()){
            PRINT("Warning: File["<<sXmlFile<<"] has no attribute of industry");
            m_sKBIndustry = "common";
		}

		return true;
	}

    void getKBInfo(std::string& sXmlName, std::string& sScmlName){
        sXmlName = m_sFileName.getNameWithoutPathExtension();
        sScmlName = m_sKBName;
    }

	void setKBIdx(int nIdx){
		m_nKBIdx = nIdx;
	}
	int getKBIdx() const{
		return m_nKBIdx;
	}
	
	const std::string getFileName() const{
		return m_sFileName.getNameWithoutPath();
	}

	const std::string getFilePath() const{
		return m_sFileName.getPath();
	}
	
	const std::string& getKBName() const{
		return m_sKBName;
	}
	
	const std::string& getKBType() const{
		return m_sKBType;
	}

    const std::string& getKBIndustry() const{
        return m_sKBIndustry;
    }

public:
	int m_nKBIdx;
	acl::FileName m_sFileName;
	std::string m_sKBName;
	std::string m_sKBType;
    std::string m_sKBIndustry;
};

}//namespace robosay

#endif //__ROBOSAY__COMMON__KB_ATTRIBUTE__HPP__
