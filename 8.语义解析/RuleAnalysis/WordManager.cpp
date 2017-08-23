#include "WordManager.h"

namespace sae
{

WordManager::WordManager()
{
    setDatabaseType(WORD_DATABASE);
}

void WordManager::recordIndexOfLastSerializedWord()
{
    indexOfLastSerializedWord = BidirectionalMap::size();
}

bool WordManager::isSerializedWord(const int& i) const
{
    return i > indexOfLastSerializedWord ? false : true;
}

std::vector<std::string> WordManager::getWords() const
{
    return BidirectionalMap::getVStrValue();
}

}//namespace sae


