#ifndef __SAE__VCELL_H__
#define __SAE__VCELL_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <iostream>
#include <fstream>
#include <vector>
#include "Cell.hpp"
#include "Range.h"
#include "CellCache.h"

namespace sae
{

class VCell;

extern  const VCell g_EmptyVCell;

class VCell : public std::vector<Cell*>
{
private:
    char m_flag;
    SVector<int> mc_CellIndex;
    SVector<int> m_vNameIndexs;

    std::vector<int> getFirstMatchedList(MatchingMatrix& matrix,const VCell& dynamicVCell) const;

    bool parseNegative(std::string& s, bool& negativeTag, ErrorData& errData);
    bool checkNegativeTagsAndContentLength(ErrorData& errData) const;
    bool isSimpleCellString(const std::string& s) const;

public:
    VCell();

    /*void setCellIndex(const SVector<int>& cellIndex) {
        mc_CellIndex = cellIndex;
    }*/
    const SVector<int>& getCellIndex() const {
        return mc_CellIndex;
    }
    void setFlag(const char& flag) {
        m_flag = flag;
    }
    const char& getFlag() const {
        return m_flag;
    }

    bool isAllNegative() const;
    bool hasContinuousCell() const;

    bool allMatch(MatchingMatrix& matrix,const VCell& dynamicVCell = g_EmptyVCell) const;
    bool forwardMatch(MatchingMatrix& matrix,const VCell& dynamicVCell = g_EmptyVCell) const;
    bool phraseMatch(MatchingMatrix& matrix,const VCell& dynamicVCell = g_EmptyVCell) const;
    bool phraseShuffleMatch(MatchingMatrix& matrix,const VCell& dynamicVCell = g_EmptyVCell) const;

    bool isReferKeyCellLegal(const int& currentKeyIndex) const;
    void freeCellMemories();

    bool addUsrDic(const std::map<int, std::vector<int> >& mUsrWords);

private:
    template <class DefinePointer, class P>
    bool batchParse(std::vector<std::string>& v, const bool& negativeTag, DefinePointer& rDefine, P& p, ErrorData& errData) {
        for(size_t i=0; i<v.size(); i++) {
            if(negativeTag) v[i] = "!" + v[i];

            Cell* cell = new Cell;

            ERROR_TRACE( cell->parse(v[i],rDefine,p,errData) )

            gp_CellCache->push_back(cell);
            mc_CellIndex.push_back(gp_CellCache->size() - 1);
            push_back(cell);
        }

        return true;
    }

    template <class DefineRuleRef,class DefinePointer,class P>
    bool batchParseDefine(std::vector<std::string>& v,const bool& negativeTag,DefineRuleRef& dRule,DefinePointer& rDefine,P& p,const std::string& basicWordMark,ErrorData& errData) {
        std::vector<Cell*> extendedCells;
        const size_t startIndexPos = mc_CellIndex.size();
        size_t i;

        for(i=0; i<v.size(); i++) {
            if(negativeTag) v[i]="!"+v[i];
            v[i] = basicWordMark + v[i];
            Cell* cell = new Cell;

            ERROR_TRACE( cell->parse(v[i],rDefine,p,errData) )

            gp_CellCache->push_back(cell);
            mc_CellIndex.push_back(gp_CellCache->size() - 1);
            push_back(cell);
            extendedCells.push_back(cell);
        }

        //only use cache when this is not a basic type set
        if(basicWordMark.empty()) {
            if(negativeTag) {
                dRule.setNegativeCache(extendedCells);
                for(i=startIndexPos; i<mc_CellIndex.size(); i++) {
                    dRule.addNegativeCacheIndex(mc_CellIndex[i]);
                }
            } else {
                dRule.setAffirmCache(extendedCells);
                for(i=startIndexPos; i<mc_CellIndex.size(); i++) {
                    dRule.addAffirmCacheIndex(mc_CellIndex[i]);
                }
            }
        }

        return true;
    }

    template <class DefineRuleRef,class DefinePointer,class P>
    bool useCacheOrExtend(std::string& s,const bool& negativeTag,std::string& keyWord,DefineRuleRef& dRule,DefinePointer& rDefine,P& p,const std::string& basicWordMark,ErrorData& errData) {
        if(!(dRule.getType() & BASIC_DEFINITION)) {
            ERROR_MESSAGE( Message::msg060(s) )
        }

        if(negativeTag && dRule.hasNegativeCache() && basicWordMark.empty() && !dRule.isNull()) {
            const std::vector<Cell*>& extendedCells = dRule.getNegativeCache();
            for(size_t i=0; i<extendedCells.size(); i++) {
                push_back(extendedCells[i]);
            }
            mc_CellIndex += dRule.getNegativeCacheIndex();
        } else if(!negativeTag && dRule.hasAffirmCache() && basicWordMark.empty() && !dRule.isNull()) {
            const std::vector<Cell*>& extendedCells = dRule.getAffirmCache();
            for(size_t j=0; j<extendedCells.size(); j++) {
                push_back(extendedCells[j]);
            }
            mc_CellIndex += dRule.getAffirmCacheIndex();
        } else {
            std::vector<std::string> v=dRule.getVal();
            if(v.empty()) {
                ERROR_MESSAGE( Message::msg061(s) )
            }

            //judge if the word definition has upper case alphabet
            if(p.getDatabaseType() == WORD_DATABASE) {
                int ret = Utility::isVectorHasLowerCaseAphabet(v);
                if( ret > -1 ) {
                    ERROR_MESSAGE( Message::msg062(s) )
                }
            }

            if(batchParseDefine(v,negativeTag,dRule,rDefine,p,basicWordMark,errData)==false) {
                ERROR_MESSAGE_TRACE( Message::msg063(s), ADD_TO_FRONT)
            }
        }
        return true;
    }

    template <class DefinePointer, class P>
    bool parseSetExtend(std::string& s, const bool& negativeTag, DefinePointer& rDefine, P& p, ErrorData& errData) {
        std::string keyWord,functionMark,basicWordMark;
        if(CommonParser::getSynonymExtendKeyword(s,functionMark,keyWord,basicWordMark) == false) {
            ERROR_MESSAGE( Message::msg062(s) )
        }

        if(functionMark == "|") {
            m_flag |= SHUFFLE_RELATION;
        }

        m_vNameIndexs.push_back(p.insertFind(keyWord));

        ERROR_TRACE( useCacheOrExtend(s,negativeTag,keyWord,rDefine.find(keyWord),rDefine,p,basicWordMark,errData) )

        return true;
    }

    template <class DefinePointer>
    bool parseConditionalDefinitions(std::string& s, DefinePointer& rDefine, ErrorData& errData) const {
        std::string sDefineCall,keyWord,value,functionMark,basicWordMark;
        size_t pos=0,pos1;

        while(1) {
            pos = s.find("$(",pos);
            if(pos == std::string::npos) break;

            pos1 = s.find(")",pos);
            if(pos1 == std::string::npos) break;

            sDefineCall = s.substr(pos,pos1-pos+1);
            if(CommonParser::getSynonymExtendKeyword(sDefineCall,functionMark,keyWord,basicWordMark) == false) {
                ERROR_MESSAGE( Message::msg064(sDefineCall) )
            }
            if(rDefine.find(keyWord).isNull()) {
                ERROR_MESSAGE( Message::msg065(keyWord) )
            }

            if( rDefine.find(keyWord).getType() & BASIC_DEFINITION ) {
                pos = pos1;
                continue;
            }

            if( rDefine.find(keyWord).getType() & KEY_DEFINITION ) {
                ERROR_MESSAGE( Message::msg060(keyWord) )
            }

            const std::string& val = rDefine.find(keyWord).getVal()[0];
            if(pos > 0 && s[pos-1] == '!') {
                value = "";
                value += val[0];
                for(size_t i=1; i<val.length()-2; i++) {
                    if(val[i] == '!' && val[i+1] == '$' && val[i+2] == '(') {
                        value += "$(";
                        i += 2;
                    } else if(val[i] == '$' && val[i+1] == '(') {
                        value += "!$(";
                        i ++;
                    } else {
                        value += val[i];
                    }
                }
                value += val[val.length()-2];
                value += val[val.length()-1];
            } else {
                value = val;
            }
            s = s.substr(0,pos) + value + s.substr(pos + keyWord.length() + 3);
            pos += value.length();
        }

        return true;
    }

    void parseBasicWordMark();

public:
    template <class DefinePointer, class P>
    bool parse(const std::string& input, DefinePointer& rDefine, P& p, ErrorData& errData) {
        std::vector<std::string> vContent;
        std::string s(input);

        ERROR_TRACE( parseConditionalDefinitions(s,rDefine,errData) )

        ERROR_TRACE( CommonParser::checkType(s,m_flag,vContent,errData) )

        if( Utility::hasEmptyCell(vContent) > -1 ) {
            ERROR_MESSAGE( Message::msg066() )
        }

        for(size_t i=0; i<vContent.size(); i++) {
            if( isSimpleCellString(vContent[i]) ) {
                bool negativeTag = false;
                std::string keyWord,functionMark,basicWordMark;
                ERROR_TRACE( parseNegative(vContent[i],negativeTag,errData) )

                if(CommonParser::isSynonymExtend(vContent[i]) == true) {
                    if(CommonParser::getSynonymExtendKeyword(vContent[i],functionMark,keyWord,basicWordMark) == false) {
                        ERROR_MESSAGE( Message::msg064(vContent[i]) )
                    }

                    if(functionMark == "/" && (m_flag & SHUFFLE_RELATION) ) {
                        ERROR_MESSAGE( Message::msg067() )
                    } else if(functionMark == "|" && (m_flag & OR_RELATION) ) {
                        ERROR_MESSAGE( Message::msg067() )
                    }

                    if(negativeTag == true && functionMark.empty()) {
                        Cell* cell = new Cell;

                        vContent[i] = "!" + vContent[i];
                        ERROR_TRACE( cell->parse(vContent[i],rDefine,p,errData) )

                        gp_CellCache->push_back(cell);
                        mc_CellIndex.push_back(gp_CellCache->size() - 1);
                        push_back(cell);
                    } else {
                        ERROR_TRACE( parseSetExtend(vContent[i],negativeTag,rDefine,p,errData) )
                    }
                } else { // not synonym extend
                    if( negativeTag == true) {
                        Cell* cell = new Cell;

                        vContent[i] = "!" + vContent[i];
                        ERROR_TRACE( cell->parse(vContent[i],rDefine,p,errData) )

                        gp_CellCache->push_back(cell);
                        mc_CellIndex.push_back(gp_CellCache->size() - 1);
                        push_back(cell);
                    } else { // not negative tag
                        Cell* cell = new Cell;

                        ERROR_TRACE( cell->parse(vContent[i],rDefine,p,errData) )

                        gp_CellCache->push_back(cell);
                        mc_CellIndex.push_back(gp_CellCache->size() - 1);
                        push_back(cell);
                    }
                }
            } else { // not simple cell string
                Cell* cell = new Cell;

                ERROR_TRACE( cell->parse(vContent[i],rDefine,p,errData) )

                gp_CellCache->push_back(cell);
                mc_CellIndex.push_back(gp_CellCache->size() - 1);
                push_back(cell);
            }
        }

        if(m_flag == 0) {
            m_flag |= OR_RELATION;
        }

        ERROR_TRACE( checkNegativeTagsAndContentLength(errData) )

        parseBasicWordMark();

        return true;
    }

public:
    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        stream.simple(m_flag);
        stream.content(m_vNameIndexs);
        size_t i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            Cell* cell;
            stream.pointer(cell);
            this->push_back(cell);
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        stream.simple(m_flag);
        stream.content(m_vNameIndexs);
        size_t i,size = this->size();
        Format &format = stream.get_format();
        format.output_start_array(size);
        for(i = 0; i < size; i++) {
            stream.pointer(this->at(i));
        }
        format.output_end_array();
    }
};

}//namespace sae

#endif //__SAE__VCELL_H__
