#ifndef __SAE__SEARCH_MODE_H__
#define __SAE__SEARCH_MODE_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "ErrorData.h"

namespace sae
{

enum SEARCH_TYPE {
    WORD_TYPE,
    TAG_TYPE,
};

class SearchModeCell
{
public:
    SEARCH_TYPE m_searchType;
    bool m_limitMode;
    bool m_phraseMode;
    bool continuousTag;

public:
    SearchModeCell() {
        m_limitMode = false;
        m_phraseMode = false;
        continuousTag = false;
    }
    void dump(std::ostream& ofs = std::cout) const;
    friend std::ostream& operator << (std::ostream& ofs, const SearchModeCell& cell) {
        cell.dump(ofs);
        return ofs;
    }

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        //m_searchType
        int iEnum;
        stream.simple(iEnum);
        m_searchType = (SEARCH_TYPE)iEnum;
        //m_limitMode
        stream.simple(m_limitMode);
        //m_phraseMode
        stream.simple(m_phraseMode);
        //continuousTag
        stream.simple(continuousTag);
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        //m_searchType
        int iEnum = m_searchType;
        stream.simple(iEnum);
        //m_limitMode
        stream.simple(m_limitMode);
        //m_phraseMode
        stream.simple(m_phraseMode);
        //continuousTag
        stream.simple(continuousTag);
    }
};

class SearchMode
{
private:
    std::vector<SearchModeCell> m_cells;
public:
    bool readSearchModeString(const std::string& sSearchMode, const int& keyIndex, ErrorData& errData);
    const std::vector<SearchModeCell>& getSearchModeCells() const {
        return m_cells;
    }
    bool hasWordSearchType() const;
    bool hasTagSearchType() const;
    bool hasNormalMode() const;
    bool hasPhraseMode() const;
    bool hasLimitMode() const;
    bool hasContinuousMode() const;

    void dump(std::ostream& ofs = std::cout) const;
    friend std::ostream& operator << (std::ostream& ofs, const SearchMode& searchMode) {
        searchMode.dump(ofs);
        return ofs;
    }
    bool hasMeaninglessCombination() const;

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        //m_cells
        int i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            SearchModeCell cell;
            stream.content(cell);
            m_cells.push_back(cell);
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        //m_cells
        int i,size = m_cells.size();
        Format &format = stream.get_format();
        format.output_start_array(size);
        for(i = 0; i < size; i++) {
            stream.content(m_cells[i]);
        }
        format.output_end_array();
    }
};

}//namespace sae

#endif //__SAE__SEARCH_MODE_H__
