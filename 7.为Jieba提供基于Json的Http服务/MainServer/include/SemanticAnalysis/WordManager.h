#ifndef __SAE__WORD_MANAGER_H__
#define __SAE__WORD_MANAGER_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <iostream>
#include <map>
#include "BidirectionalMap.h"
#include "ErrorData.h"

namespace sae
{

class WordManager : public BidirectionalMap
{
private:
    int indexOfLastSerializedWord;
public:
    WordManager();
    void recordIndexOfLastSerializedWord();
    bool isSerializedWord(const int& i) const;
    std::vector<std::string> getWords() const;

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        BidirectionalMap::composite(stream);
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        BidirectionalMap::composite(stream);
    }
};

}//namespace sae

#endif //__SAE__WORD_MANAGER_H__
