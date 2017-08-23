#ifndef __SAE__POSITION_H__
#define __SAE__POSITION_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "ErrorData.h"
#include "KeyRange.h"

namespace sae
{

class PositionElement
{
public:
    bool isMark;
    bool isKey;
    KeyNum keyNum;
    int num;
    bool isPlusMark;

public:
    void dump(std::ostream& ofs = std::cout) const {
        ofs<<"isM:"<<isMark<<", isK:"<<isKey<<", keyNum:";
        keyNum.dump(ofs);
        ofs<<", num:"<<num<<", isPlus:"<<isPlusMark<<std::endl;
    }
};

class PosCell
{

public:
    bool m_isIntersection;
    bool m_isKeyType;
    DIRECTION m_hDirection;//head direction : the direction before number
    int m_num;
    int m_whichPosOfKey;//-1:no use; 0:head; TAIL:tail
    DIRECTION m_tDirection;//tail direction : the direction after number
    int m_offset;//the number after tDirection

public:
    bool getReferKeyNum(KeyNum& keyNum) const;
    PosCell();
    int instantiateTail(const int& tail) const;
    bool isBackwardSearch() const;

    void dump(std::ostream& ofs = std::cout) const {
        ofs<<"isI:"<<m_isIntersection<<", isK:"<<m_isKeyType<<", hDir:"<<m_hDirection<<", mNum:"<<m_num<<", whichPos:"<<m_whichPosOfKey<<", tDir:"<<m_tDirection<<", off:"<<m_offset;
    }

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        stream.simple(m_isIntersection);
        stream.simple(m_isKeyType);
        int iEnum;
        stream.simple(iEnum);
        m_hDirection = (DIRECTION)iEnum;
        stream.simple(m_num);
        stream.simple(m_whichPosOfKey);
        stream.simple(iEnum);
        m_tDirection = (DIRECTION)iEnum;
        stream.simple(m_offset);
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        stream.simple(m_isIntersection);
        stream.simple(m_isKeyType);
        int iEnum = m_hDirection;
        stream.simple(iEnum);
        stream.simple(m_num);
        stream.simple(m_whichPosOfKey);
        iEnum = m_tDirection;
        stream.simple(iEnum);
        stream.simple(m_offset);
    }
};

class Position
{

private:
    bool m_isSearchAnywhere;
    std::vector<PosCell> m_searchPos;

private:
    bool isLegalFormula(const FormulaString& sFormula) const;
    bool checkImpossibleCombination(const std::string& s, ErrorData& errData) const;
    std::vector<std::string> divideFormulaElement(const std::string& s, ErrorData& errData) const;
    std::vector<PositionElement> parseElement(std::vector<std::string>& v, const int& keyIndex, ErrorData& errData) const;
    bool parseFormulaElement(const std::string& s, std::vector<PositionElement>& vElements, const int& keyIndex, ErrorData& errData) const;
    bool readFormula(const FormulaString& formulaString, PosCell& cell, const int& keyIndex, ErrorData& errData);
    int readFormulas(std::vector<FormulaString>& vFormulaString, const int& keyIndex, ErrorData& errData);
    bool isImpossiblePosition() const;

public:
    Position() {
        m_isSearchAnywhere=false;
    }
    void dump(std::ostream& ofs = std::cout) const;
    const std::vector<PosCell>& getSearchPoses() const {
        return m_searchPos;
    }
    const bool isSearchAnywhere() const {
        return m_isSearchAnywhere;
    }
    void setSearchModeAnywhere() {
        m_isSearchAnywhere=true;
    }
    int readPositionString(const std::string& sPosition, const int& keyIndex, ErrorData& errData);
    bool isAllPositionInSet(std::set<int>& setKey) const;
	bool allIntersection() const;

//unused:
    bool checkKeyTypePosition(std::vector<PosCell>& vPosCell, const int& keyIndex, ErrorData& errData);
    int readFormulas(std::vector<FormulaString>& vFormulaString,std::vector<PosCell>& vPosCell,const int& keyIndex,ErrorData& errData);


    //------------------------------------------------------------------------------------
    bool isSinglePosition() const;

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        stream.simple(m_isSearchAnywhere);
        //m_searchPos
        int i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            PosCell cell;
            stream.content(cell);
            m_searchPos.push_back(cell);
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        stream.simple(m_isSearchAnywhere);
        //m_searchPos
        int i,size = m_searchPos.size();
        Format &format = stream.get_format();
        format.output_start_array(size);
        for(i = 0; i < size; i++) {
            stream.content(m_searchPos[i]);
        }
        format.output_end_array();
    }
};

}//namespace sae

#endif //__SAE__POSITION_H__
