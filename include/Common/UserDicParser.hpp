#ifndef __ROBOSAY___USER_DIC_PARSER_H__
#define __ROBOSAY___USER_DIC_PARSER_H__
#include <vector>
#include "Common/lib4/Definations.hpp"
#include "Common/lib4/File.hpp"
#include "Common/lib4/CommonDef.h"
#include "Common/lib4/StringUtility.hpp"
#include "Common/lib4/FileEncoder.hpp"
#include "Common/lib4/FileName.hpp"

namespace robosay{

class UserDicParser{
public:
	UserDicParser()
        : m_mapAll(){}

    ~UserDicParser(){
        m_mapAll.clear();
    }

    bool init(const acl::FileName& sPath = "", const StringStringSetMap* pWords = NULL){
        m_mapAll.clear();
        StringVector vLines;
		if(sPath.getExtension() != "dat"){
			acl::File file;
			bool bRet = file.read(sPath);
			if(!bRet){
				PRINT("Can't open normal file ["<<sPath<<"]")
				return false;
			}
			vLines = file.toVector();
		}else{
			acl::FileEncoder encoder;
			std::string sFileFullPath = sPath.getBaseName()+".dat";
			bool bRet = encoder.decodeFile(sFileFullPath, vLines);
			if(!bRet){
				PRINT("Can't open encode file ["<<sFileFullPath<<"]")
				return false;
			}
		}
		const size_t uLineCnt = vLines.size();
		std::string sKey;
		acl::Line sName;
		for(size_t uIdx = 0u; uLineCnt > uIdx; ++uIdx){
			acl::Line line = vLines[uIdx];
			line.strip();
			if(line.isBeginWith("[")){
				size_t nPos = line.find("]");
				if(nPos == std::string::npos){
					PRINT("The format of line ["<<line<<"] error!")
					return false;
				}
				sKey = line.substr(1, nPos - 1);
				if(sKey.empty()){
					PRINT("The format of line ["<<line<<"] error!")
					return false;
				}
			}else if(line.isBeginWith("(")){
				acl::String sLine = acl::StringUtility::getStringBetween(line, "=", ")");
				StringVector vStrs = sLine.separate(",");
				sName = acl::StringUtility::getStringBetween(line, "#DEFINE", "=");
				sName.strip();
				bool bRet = insertAll(sKey, sName, vStrs);
				if(!bRet){
					PRINT("Insert [key:"<<sKey<<" name:"<<sName<<"] failed!")
					return false;
				}
			}else{
				;
			}
		}
		
        AddMap(pWords);
		return true;
    }

    bool insertAll(const std::string& sKey, const std::string& sName, const StringVector& vStrs){
        if(sKey.empty() || sName.empty() || vStrs.empty()){
            return false;
        }
        acl::String sK(sKey);
        sK.toUpper();
        acl::String sN(sName);
        sN.toUpper();
        StringSet& seNames = m_mapName[sKey];
        seNames.insert(sN);
        const StringVector::const_iterator last = vStrs.end();
        StringVector::const_iterator itr = vStrs.begin();
        StringSet& seWords = m_mapAll[sK];
		StringStringMap& mWords = m_mAlias2Name2[sK];
		StringStringSetMap& mWords3 = m_mAlias2Name3[sK];
        for(; last != itr; ++itr){
            acl::String str(*itr);
            str.strip();
            str.toUpper();
            m_mAlias2Name.insert(std::make_pair(str, sN));
            seWords.insert(str);
			mWords.insert(std::make_pair(str, sN));
			
			StringSet& seWords3 = mWords3[sN];
			seWords3.insert(str);
        }
        return true;
    }

    bool AddMap(const StringStringSetMap* sssm){
        if(!sssm){
			PRINT("Map is empty!")
            return false;
        }
        const StringStringSetMap::const_iterator last = sssm->end();
        StringStringSetMap::const_iterator itr = sssm->begin();
        for(; last != itr; ++itr){
            const StringSet::const_iterator l = itr->second.end();
            StringSet::const_iterator i = itr->second.begin();
            acl::String sKey(itr->first);
            sKey.toUpper();
            StringSet& seWords = m_mapAll[sKey];
            for(; l != i; ++i){
                acl::String str(*i);
                str.strip();
                str.toUpper();
                m_mAlias2Name.insert(std::make_pair(str, str));
                seWords.insert(str);
            }
        }

        return true;
    }

    const StringStringSetMap* getMap() const{
        return m_mapAll.empty() ? NULL : &m_mapAll;
    }

    std::string getNameByAlias(const std::string& sAlias){
        const StringStringMap::const_iterator itrFind = m_mAlias2Name.find(sAlias);
        if(itrFind == m_mAlias2Name.end()){
            return "";
        }

        return itrFind->second;
    }
	
	bool getNameByAlias(const std::string& sKey, std::string& sVal) const{
		const std::map<std::string, StringStringMap>::const_iterator itrFind = m_mAlias2Name2.find(sKey);
        if(itrFind == m_mAlias2Name2.end()){
            return false;
        }
        acl::String sStr(sVal);
        sStr.toUpper();
		const StringStringMap::const_iterator itr= itrFind->second.find(sStr);
		if(itr == itrFind->second.end()){
			return false;
		}
        sVal = itr->second;
		return true;
	}

	std::string toStr() const{
        std::stringstream ss;
        const StringStringSetMap::const_iterator last = m_mapAll.end();
        StringStringSetMap::const_iterator itr = m_mapAll.begin();
        for(; last != itr; ++itr){
            const StringSet::const_iterator l = itr->second.end();
            StringSet::const_iterator i = itr->second.begin();
            for(; l != i; ++i){
                ss<<"MAP:\t["<<itr->first<<"]\t["<<*i<<"]\n";
            }
        }
        const StringStringMap::const_iterator l = m_mAlias2Name.end();
        StringStringMap::const_iterator i = m_mAlias2Name.begin();
        for(; l != i; ++i){
            ss<<"ALIAS:\t["<<i->first<<"]\t["<<i->second<<"]\n";
        }

        return ss.str();
    }

    /*StringVector getAttributes() const{
        return getVectorByKey("属性");
    }*/

    StringVector getVectorByKey(const std::string& sKey)const{
        StringStringSetMap::const_iterator itr = m_mapName.find(sKey);
        if(itr == m_mapName.end()){
            return StringVector();
        }
        StringVector vRet;
        vRet.reserve(itr->second.size());
        const StringSet::const_iterator l = itr->second.end();
        StringSet::const_iterator i = itr->second.begin();
        for(;l != i; ++i){
            vRet.push_back(*i);
        }

        return vRet;
    }

	bool getInfoByGroupName(const std::string& sName, StringStringSetMap& mWords){
		const std::map<std::string, StringStringSetMap>::const_iterator itr = m_mAlias2Name3.find(sName);
		if(itr == m_mAlias2Name3.end()){
			return false;
		}
		mWords = itr->second;
		return true;
	}

private:
    StringStringSetMap m_mapAll;
    StringStringSetMap m_mapName;
	std::map<std::string, StringStringMap> m_mAlias2Name2;
	std::map<std::string, StringStringSetMap> m_mAlias2Name3;
    StringStringMap m_mAlias2Name;
};
}//namespace robosay

#endif //__ROBOSAY___USER_DIC_PARSER_H__
