#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "Utility.h"
#include "KeyPart.h"

namespace sae
{

bool KeyCell::checkKeyElementsNumber(const std::string& sKey, const int& keyIndex, ErrorData& errData) const
{
    int count = 1;
    for(size_t i=0; i<sKey.length(); i++) {
        if(sKey[i] == '\t') {
            count++;
        }
    }
    if(count != KeyElementsNumber) {
        errData.m_errMsg = Message::msg025(keyIndex,sKey,KeyElementsNumber);
		errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    } else {
        return true;
    }
}

bool KeyCell::getSearchModeFromString(std::string& sKey, std::string& sSearchMode, const int& keyIndex, ErrorData& errData) const
{
    sSearchMode = Utility::trimLeft(sKey,"\t");
    if(sSearchMode.empty()) {
        errData.m_errMsg = Message::msg026(keyIndex);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    } else return true;
}

bool KeyCell::getPositionFromString(std::string& sKey, std::string& sPosition, const int& keyIndex, ErrorData& errData) const
{
    sPosition = Utility::trimLeft(sKey,"\t");
    if(sPosition.empty()) {
        errData.m_errMsg = Message::msg027(keyIndex);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    } else return true;
}

bool KeyCell::getScriptLimitFromString(std::string& sKey, std::string& sCharLimit,const int& keyIndex, ErrorData& errData) const
{
    sCharLimit = Utility::trimLeft(sKey,"\t");
    if(sCharLimit.empty()) {
        errData.m_errMsg = Message::msg028(keyIndex);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    } else return true;
}

bool KeyCell::getWordLimitFromString(std::string& sKey, std::string& sWordLimit,const int& keyIndex, ErrorData& errData) const
{
    sWordLimit = Utility::trimLeft(sKey,"\t");
    if(sWordLimit.empty()) {
        errData.m_errMsg = Message::msg029(keyIndex);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    } else return true;
}

bool KeyCell::getTagLimitFromString(std::string& sKey, std::string& sTagLimit,const int& keyIndex, ErrorData& errData) const
{
    sTagLimit = Utility::trimLeft(sKey,"\t");
    if(sTagLimit.empty()) {
        errData.m_errMsg = Message::msg030(keyIndex);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    } else return true;
}

bool KeyCell::readKeyString(const std::string& sKeyIn, const int& keyIndex, DataManager* pDataManager, ErrorData& errData)
{
    std::string sKey;

    //check if this KEY is a weak deny KEY
    if(sKeyIn[1] == '[' && sKeyIn[sKeyIn.length()-2]==']') {
        if(keyIndex == 0) {
            ERROR_MESSAGE( Message::msg031(keyIndex) )
        }
        sKey = sKeyIn.substr(2,sKeyIn.length()-4);
        m_isOmissibleKey = true;
    } else {
        sKey = sKeyIn.substr(1,sKeyIn.length()-2);;
    }

    //check elements number
    ERROR_TRACE( checkKeyElementsNumber(sKey,keyIndex,errData) )

    //search mode
    std::string sSearchMode;
    ERROR_TRACE( getSearchModeFromString(sKey,sSearchMode,keyIndex,errData) )
    else {
        ERROR_TRACE( m_searchMode.readSearchModeString(sSearchMode,keyIndex,errData) )
    }

    //search position
    std::string sPosition;
    ERROR_TRACE( getPositionFromString(sKey,sPosition,keyIndex,errData) )
    else if(sPosition == "*") {
        if(keyIndex == 0) {
            m_position.setSearchModeAnywhere();
        } else {
            ERROR_MESSAGE( Message::msg032(keyIndex) )
        }
    } else if(sPosition == "[P]+1" && keyIndex == 0) {
        m_position.setSearchModeAnywhere();
    } else {
		if(keyIndex == 0){
			if(sPosition.find("+") == 0 || sPosition.find("-") == 0){
				ERROR_MESSAGE( Message::msg108() )
			}
		}
        if( m_position.readPositionString(sPosition,keyIndex,errData) != 0 ) {
            errData.errorTrace(__FILE__,__LINE__-1);
            return false;
        }
    }

    //charactor limit
    std::string sScriptLimit;
    ERROR_TRACE( getScriptLimitFromString(sKey,sScriptLimit,keyIndex,errData) )
    else if(sScriptLimit != "*") {
        if( m_scriptLimit.parse(sScriptLimit,pDataManager,errData) == false ) {
            errData.m_errMsg = Message::msg033(keyIndex) + errData.m_errMsg;
            errData.errorTrace(__FILE__,__LINE__-1);
            return false;
        }
    }

    //word limit
    std::string sWordLimit;
    ERROR_TRACE( getWordLimitFromString(sKey,sWordLimit,keyIndex,errData) )
    else {
        if(sWordLimit != "*") {
            if(m_searchMode.hasWordSearchType() == false) {
                ERROR_MESSAGE( Message::msg034(keyIndex) )
            }
            if(m_wordLimit.hasUnisolatedComma(sWordLimit) == true) {
                ERROR_MESSAGE( Message::msg035(keyIndex) )
            }
            ERROR_TRACE( m_wordLimit.parse(sWordLimit,pDataManager,errData) )
        } else {
            if(m_searchMode.hasWordSearchType() == true) {
                ERROR_MESSAGE( Message::msg036(keyIndex) )
            }
        }
    }

    //Tag limit
    std::string sTagLimit;
    ERROR_TRACE( getTagLimitFromString(sKey,sTagLimit,keyIndex,errData) )
    else {
        if(sTagLimit != "*") {
            if(m_searchMode.hasTagSearchType() == false) {
                ERROR_MESSAGE( Message::msg037(keyIndex) )
            }
            if( m_tagLimit.parse(sTagLimit,pDataManager,errData) == false ) {
                ERROR_MESSAGE( Message::msg038(keyIndex) + errData.m_errMsg )
            }
        } else {
            if(m_searchMode.hasTagSearchType() == true) {
                ERROR_MESSAGE( Message::msg039(keyIndex) )
            }
        }
    }

    if(keyIndex == 0) { //check the entry key
        if(checkNegativeInEntryKey() == false) {
            ERROR_MESSAGE( Message::msg040(keyIndex) )
        }
        if(isOmissibleKey() == true) {
            ERROR_MESSAGE( Message::msg041(keyIndex) )
        }
    } else { //check the normal keys
        if( hasMeaninglessCombination(errData) ) {
            errData.m_errMsg += Utility::int2Str(keyIndex)+".";
            errData.errorTrace(__FILE__,__LINE__-1);
            return false;
        }
    }

    //check all the keys
    ERROR_TRACE( isReferKeyCellLegal(keyIndex,errData) )

    return true;
}

bool KeyCell::checkNegativeInEntryKey()
{
    if(m_searchMode.hasWordSearchType()) {
        if(m_wordLimit.getVCellPointer()->isAllNegative() == false) {
            return true;
        }
    }
    if(m_searchMode.hasTagSearchType()) {
        if(m_tagLimit.getVCellPointer()->isAllNegative() == false) {
            return true;
        }
    }

    return false;
}

void KeyCell::dump(const DataManager& dataManager, std::ostream& ofs) const
{
    //search mode
    m_searchMode.dump(ofs);
    ofs<<DUMP_SEPARATOR;
    //search position
    m_position.dump(ofs);
    ofs<<DUMP_SEPARATOR;
    //script limitation
    m_scriptLimit.dump(ofs);
    ofs<<DUMP_SEPARATOR;
    //word limitation
    m_wordLimit.dump(dataManager,ofs);
    ofs<<DUMP_SEPARATOR;
    //tag limitation
    m_tagLimit.dump(dataManager,ofs);
}

void KeyPart::dump(const DataManager& dataManager, std::ostream& ofs) const
{
    for(size_t i=0; i<m_keys.size(); i++) {
        if(m_keys[i].isOmissibleKey() == false) {
            ofs<<"(";
            m_keys[i].dump(dataManager,ofs);
            ofs<<")";
        } else {
            ofs<<"([";
            m_keys[i].dump(dataManager,ofs);
            ofs<<"])";
        }
    }
}

bool KeyPart::checkReferOfOmissibleKeyInKeys(ErrorData& errData) const
{
    std::set<int> setOmissibleKey;

    for(size_t i=0; i<m_keys.size(); i++) {
        if(m_keys[i].isOmissibleKey() == true) {
            setOmissibleKey.insert(i);
        } else {
            if(!setOmissibleKey.empty()) {
                if(m_keys[i].getPosition().isAllPositionInSet(setOmissibleKey) == true) {
                    errData.m_errMsg = Message::msg042(i);
                    errData.errorTrace(__FILE__,__LINE__-1);
                    return false;
                }

                if(m_keys[i].getConstWordLimit().isAllWordInSet(setOmissibleKey) == true) {
                    errData.m_errMsg = Message::msg043(i);
                    errData.errorTrace(__FILE__,__LINE__-1);
                    return false;
                }
            }
        }
    }

    return true;
}

std::set<int> KeyPart::getOmissibleKeySet() const
{
    std::set<int> setOmissibleKey;

    for(size_t i=0; i<m_keys.size(); i++) {
        if(m_keys[i].isOmissibleKey() == true) {
            setOmissibleKey.insert(i);
        }
    }

    return setOmissibleKey;
}

bool KeyPart::checkPositionsOutsideBounds(ErrorData& errData) const
{

    for(size_t i=0; i<m_keys.size(); i++) {

        const std::vector<PosCell>& searchPoses = m_keys[i].getPosition().getSearchPoses();
        for(size_t j=0; j<searchPoses.size(); j++) {

            if(searchPoses[j].m_isKeyType==true && searchPoses[j].m_tDirection==DIRECTION_AFTER) {

                const std::vector<PosCell>& targetSearchPoses = m_keys[searchPoses[j].m_num].getPosition().getSearchPoses();
                for(size_t k=0; k<targetSearchPoses.size(); k++) {
                    if(targetSearchPoses[k].m_num==TAIL && targetSearchPoses[k].m_hDirection==NO_DIRECTION && targetSearchPoses[k].m_tDirection==NO_DIRECTION) {
                        errData.m_errMsg = Message::msg044(i);
                        errData.errorTrace(__FILE__,__LINE__-1);
                        return false;
                    }
                }

            }

            if(searchPoses[j].m_isKeyType==true && searchPoses[j].m_tDirection==DIRECTION_BEFORE) {

                const std::vector<PosCell>& targetSearchPoses = m_keys[searchPoses[j].m_num].getPosition().getSearchPoses();
                for(size_t k=0; k<targetSearchPoses.size(); k++) {
                    if(targetSearchPoses[k].m_num==0 && targetSearchPoses[k].m_hDirection==NO_DIRECTION && targetSearchPoses[k].m_tDirection==NO_DIRECTION) {
                        errData.m_errMsg = Message::msg045(i);
                        errData.errorTrace(__FILE__,__LINE__-1);
                        return false;
                    }
                }

            }

        }
    }

    return true;

}

bool KeyCell::hasMeaninglessCombination(ErrorData& errData) const
{
    if( isPhraseSearchModeAndSingleSearchPositionCombined() ) {
        errData.m_errMsg = Message::msg046();
        errData.errorTrace(__FILE__,__LINE__-1);
        return true;
    }

    if( isContinuousSearchMethodAndSingleSearchPositionCombined() ) {
        errData.m_errMsg = Message::msg047();
        errData.errorTrace(__FILE__,__LINE__-1);
        return true;
    }

    if( isContinuousConditionAndSingleSearchPositionCombined(errData) ) {
        errData.errorTrace(__FILE__,__LINE__-1);
        return true;
    }

    return false;
}

bool KeyCell::isPhraseSearchModeAndSingleSearchPositionCombined() const
{
    return m_searchMode.hasPhraseMode() && m_position.isSinglePosition() ? true : false;
}

bool KeyCell::isContinuousSearchMethodAndSingleSearchPositionCombined() const
{
    return m_searchMode.hasContinuousMode() && m_position.isSinglePosition() ? true : false;
}

bool KeyCell::isReferKeyCellLegal(const int& currentKeyIndex, ErrorData& errData) const
{
    if( m_wordLimit.getVCellPointer() != NULL && !m_wordLimit.getVCellPointer()->isReferKeyCellLegal(currentKeyIndex) ) {
        errData.m_errMsg = Message::msg048(currentKeyIndex);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    if( m_tagLimit.getVCellPointer() != NULL && !m_tagLimit.getVCellPointer()->isReferKeyCellLegal(currentKeyIndex) ) {
        errData.m_errMsg = Message::msg049(currentKeyIndex);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    return true;
}

bool KeyCell::isContinuousConditionAndSingleSearchPositionCombined(ErrorData& errData) const
{
    if( m_wordLimit.getVCellPointer() != NULL && m_wordLimit.getVCellPointer()->hasContinuousCell() && m_position.isSinglePosition()) {
        errData.m_errMsg = Message::msg050();
        errData.errorTrace(__FILE__,__LINE__-1);
        return true;
    }

    if( m_tagLimit.getVCellPointer() != NULL && m_tagLimit.getVCellPointer()->hasContinuousCell() && m_position.isSinglePosition()) {
        errData.m_errMsg = Message::msg051();
        errData.errorTrace(__FILE__,__LINE__-1);
        return true;
    }

    return false;
}

}//namespace sae
