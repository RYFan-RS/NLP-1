#ifndef __SAE__CELL_H__
#define __SAE__CELL_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <iostream>
#include <sstream>
#include "SerializableVector.hpp"
#include "MatchingMatrix.h"
#include "ErrorData.h"
#include "Range.h"
#include "KeyRange.h"
#include "CommonParser.h"
#if defined(MEAN_DEBUG)
#include <sstream>
#include "Common/lib4/Debug.hpp"
#endif //MEAN_DEBUG

namespace sae
{

class Cell
{

private:
    char m_flags; //find the definations in TypeDefine.mae.h
    SVector<bool> negativeTags;
    SVector<int> content;
    SVector<SVector<int> > vNegativeGroupContent;
    KeyNum keyNum;

    void copyCell(const Cell* cell);
    bool matchAndTypeContent(MatchingMatrix& matrix) const;
    bool matchContinuousTypeContent(MatchingMatrix& matrix) const;
    bool matchForwardContinuousTypeContent(MatchingMatrix& matrix) const;
    bool matchBackwardContinuousTypeContent(MatchingMatrix& matrix) const;
    std::vector<std::string> divideCell(const std::string& s, ErrorData& errData);
    bool parseNegative(std::string& s, bool& negativeTag, ErrorData& errData);
    bool parseReferKey(std::string& s, const bool& negativeTag, ErrorData& errData);

public:
    Cell();

    const char& getFlags() const {
        return m_flags;
    }
    const SVector<SVector<int> >& getNegativeGroupContent() const {
        return vNegativeGroupContent;
    }
    bool isReferKeyCell() const {
        return m_flags & REFER_KEY_CELL ? true : false;
    }
    bool isAndRelation() const {
        return m_flags & AND_RELATION ? true : false;
    }
    bool isNegativeSet() const {
        return m_flags & NEGATIVE_SET ? true : false;
    }
    size_t length() const {
        return m_flags & AND_RELATION ? 1 : content.size();
    }
    bool isAllNegative() const;
    Cell* makeNewCellByDynamicContent(const SVector<int>& content) const;
    bool matchAndTypeContentAtSinglePosition(const MatchingMatrix& matrix, const int& pos) const;
    const SVector<bool>& getNegativeTags() const {
        return negativeTags;
    }
    void setNegativeTags(const SVector<bool>& negativeTags) {
        this->negativeTags = negativeTags;
    }
    const SVector<int>& getContent() const {
        return content;
    }
    void setContent(const SVector<int>& content) {
        this->content = content;
    }
    void setKeyNum(const KeyNum& keyNum) {
        this->keyNum = keyNum;
    }
    void setFlags(const char& flags) {
        this->m_flags = flags;
    }
    void setNegativeGroupContent(const SVector<SVector<int> >& negativeGroupContent) {
        this->vNegativeGroupContent = negativeGroupContent;
    }
    bool match(MatchingMatrix& matrix) const {
        return m_flags & AND_RELATION ? matchAndTypeContent(matrix) : matchContinuousTypeContent(matrix);
    }
    SVector<int> getEntryCodes() const {
        return m_flags & AND_RELATION ? content : SVector<int>(content[0]);
    }
    const int& getCode() const {
        return content[0];
    }
    bool findCode(const int& code) const {
        return content.find(code);
    }
    const KeyNum& getKeyNum() const {
        return keyNum;
    }
    bool hasNegativeTag() const;
    bool isBasicWordCell() const;

    void dump(std::ostream& ofs = std::cout) const;

    template<class P>
    void dump(const P& p, std::ostream& ofs = std::cout) const {
        if(m_flags & NORMAL_CELL) {
            for(size_t i=0; i<content.size(); i++) {
                if(i != 0) ofs<<(m_flags & AND_RELATION ? "&" : ";");
                if(negativeTags[i]) ofs<<"!";
                ofs<<p.find(content[i]);
            }
            if(!vNegativeGroupContent.empty()) {
                if(!content.empty()) ofs<<"&";
                for(size_t j=0; j<vNegativeGroupContent.size(); j++) {
                    if(j != 0) ofs<<"&";
                    ofs<<"!(";
                    for(size_t k=0; k<vNegativeGroupContent[j].size(); k++) {
                        if(k != 0) ofs<<"&";
                        ofs<<p.find(vNegativeGroupContent[j][k]);
                    }
                    ofs<<")";
                }
            }
        } else {
            if(negativeTags[0]) ofs<<"!";
            keyNum.dump(ofs);
        }
    }

    bool addUsrDic(const int nWordID){
        content.push_back(nWordID);
        negativeTags.push_back(false);
    }

private:
    template <class P>
    bool parseNormalString(std::string& s, const bool& negativeTag, P& p, ErrorData& errData) {
        s = Utility::convertTranslateChar(s);
        if(p.getDatabaseType() == WORD_DATABASE) {
            content.push_back( p.insertFind(s) );
            if(s.find("<b>") == 0) {
                m_flags |= BASIC_WORD_CELL;
            }
        } else {
            int code = p.find(s);
            if(code == p.m_failCode) {
                ERROR_MESSAGE( Message::msg061(s) )
            }
            content.push_back( code );
        }

        negativeTags.push_back(negativeTag);

        return true;
    }

    template <class DefineRuleRef, class DefinePointer, class P>
    bool useCacheOrExtend(std::string& s, const bool& negativeTag, std::string& keyWord, DefineRuleRef& dRule, DefinePointer& rDefine, P& p, const std::string& basicWordMark, ErrorData& errData) {
        if (!(dRule.getType() & BASIC_DEFINITION)) {
            ERROR_MESSAGE( Message::msg069(keyWord) )
        }

        Cell *cell;
        if (negativeTag && dRule.hasNegativeCache()) {
            cell = dRule.getNegativeCache()[0];
        } else if (!negativeTag && dRule.hasAffirmCache()) {
            cell = dRule.getAffirmCache()[0];
        } else {
            cell = new Cell;

            std::vector<std::string> v = dRule.getVal();
            if (v.empty()) {
                ERROR_MESSAGE( Message::msg061(keyWord) )
            }
            //judge if the word definition has upper case alphabet
            if(p.getDatabaseType() == WORD_DATABASE) {
                int ret = Utility::isVectorHasLowerCaseAphabet(v);
                if( ret > -1 ) {
                    ERROR_MESSAGE( Message::msg062(keyWord) )
                }
            }

            for (size_t i = 0; i < v.size(); i++) {
                v[i] = basicWordMark + v[i];
                if (cell->parseNormalString(v[i], negativeTag, p, errData) == false) {
                    ERROR_MESSAGE_TRACE( Message::msg063(s), ADD_TO_FRONT )
                }
            }

            //only use cache when this is not a basic type set
            if(basicWordMark.empty()) {
                if (negativeTag) {
                    dRule.pushNegativeCacheCell(cell);
                } else {
                    dRule.pushAffirmCacheCell(cell);
                }
            }
        }
        copyCell(cell);

        return true;
    }

    template <class DefinePointer, class P>
    bool parseSetExtend(std::string& s, const bool& negativeTag, DefinePointer& rDefine, P& p, ErrorData& errData) {
        std::string keyWord,functionMark,basicWordMark;
        if(CommonParser::getSynonymExtendKeyword(s,functionMark,keyWord,basicWordMark) == false) {
            ERROR_MESSAGE( Message::msg064(s) )
        }

        if(functionMark == ";") {
            if( m_flags & AND_RELATION ) {
                ERROR_MESSAGE( Message::msg067() )
            }
            m_flags |= CONTINUOUS_RELATION;
        } else if(functionMark == "&") {
            if( m_flags & CONTINUOUS_RELATION ) {
                ERROR_MESSAGE( Message::msg067() )
            }
            m_flags |= AND_RELATION;
        }

        ERROR_TRACE( useCacheOrExtend(s,negativeTag,keyWord,rDefine.find(keyWord),rDefine,p, basicWordMark,errData) )

        if (!(m_flags & (AND_RELATION | CONTINUOUS_RELATION))) m_flags |= AND_RELATION;

        return true;
    }


public:
    template <class DefinePointer, class P>
    bool parse(std::string& s, DefinePointer& rDefine, P& p, ErrorData& errData) {
        std::vector<std::string> v = divideCell(s,errData);

        for(size_t i=0; i<v.size(); i++) {
            bool negativeTag = false;
            ERROR_TRACE( parseNegative(v[i],negativeTag,errData) )

            if(CommonParser::isSynonymExtend(v[i]) == true) {
                ERROR_TRACE( parseSetExtend(v[i],negativeTag,rDefine,p,errData) )
            } else if(CommonParser::isReferKey(v[i]) == true) {
                ERROR_TRACE( parseReferKey(v[i],negativeTag,errData) )
            } else if(v[i][0]=='(' && v[i][v[i].length()-1]==')' && v[i].length() > 2) {
                v[i] = v[i].substr(1,v[i].length()-2);
                Cell cell;
                ERROR_TRACE( cell.parse(v[i],rDefine,p,errData) )
                if(v[i].find(";")!=std::string::npos) {
                    (*this) = cell;
                    m_flags |= NEGATIVE_SET;
                } else {
                    vNegativeGroupContent.push_back(cell.getContent());
                }
            } else {
                //judge if the word definition has upper case alphabet
                if(p.getDatabaseType() == WORD_DATABASE) {
                    if( Utility::isStringHasLowerCaseAphabet(v[i]) ) {
                        ERROR_MESSAGE( Message::msg062(v[i]) )
                    }
                }

                ERROR_TRACE( parseNormalString(v[i],negativeTag,p,errData) )
            }
        }

        //set AND_RELATION as default search type
        if( !(m_flags & (AND_RELATION | CONTINUOUS_RELATION) ) ) m_flags |= AND_RELATION;

        return true;
    }

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        stream.simple(m_flags);
        stream.content(negativeTags);
        stream.content(content);
        stream.content(vNegativeGroupContent);
        stream.content(keyNum);
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        stream.simple(m_flags);
        stream.content(negativeTags);
        stream.content(content);
        stream.content(vNegativeGroupContent);
        stream.content(keyNum);
    }
};

}//namespace sae

#endif //__SAE__CELL_H__
