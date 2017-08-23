#include "CommonParser.h"
#include "KeyWordManager.h"

namespace sae
{

typedef enum KeyWordField_Enum 
{
	KWF_KeyWordMark = 0,
	KWF_TagIndexNum,
	KWF_KeyWordSet,
	KWF_Buffer
}KWF_ENUM;

////////////////////////////////////////////////////////////////////////////////////////////////////////
//CKeyWordManager begin

bool CKeyWordManager::Parse(const LINE& line, ErrorData& errData)
{
	std::vector<std::string> keyWordElem = CommonParser::divideElements(line.m_line,KWF_Buffer,errData);
	
	if (keyWordElem.empty()) { 
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
	}

	 if(!Utility::isPureNumber(keyWordElem[KWF_TagIndexNum])){
        errData.m_errMsg = Message::msg012();
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    int iTagIndexNum = atoi(keyWordElem[KWF_TagIndexNum].c_str());
	std::string& strRef = keyWordElem[KWF_KeyWordSet];
	strRef = strRef.substr(0,strRef.length()-1);
	m_keyWords[iTagIndexNum].AddKeyWord(strRef);

	return true;
}

//bool : true successful, false : fail
bool CKeyWordManager::FindKeyWordSetByTagIndexNum(int iTagIndexNum, const CKeyWordSet*& pKeyWordSet)const
{
	std::map<int, CKeyWordSet>::const_iterator iter = m_keyWords.find(iTagIndexNum);
	if (iter == m_keyWords.end()){
		return false;
	}

	pKeyWordSet = &iter->second;
	
	return true;
}


}
