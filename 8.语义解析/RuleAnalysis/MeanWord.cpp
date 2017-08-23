#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "MeanWord.h"
#include <sstream>

using namespace std;

namespace sae
{

#ifndef WIN32
pthread_mutex_t MeanWord::m_mutex_mae_meanword_locker = PTHREAD_MUTEX_INITIALIZER;
#endif //WIN32

MeanWord::MeanWord()
{
    m_isDigitalWord = false;
    m_word = -1;
    m_basicWord = -1;
}

void MeanWord::setWord(const int& word)
{
    m_word = word;
}

void MeanWord::setWord(const std::string& word, const DataManager& dataManager)
{
#ifndef WIN32
    pthread_mutex_lock(&m_mutex_mae_meanword_locker);
#endif //WIN32
    m_word = const_cast<DataManager&>(dataManager).getWordManager().insertFind(word);
#ifndef WIN32
    pthread_mutex_unlock(&m_mutex_mae_meanword_locker);
#endif //WIN32	

    if(dataManager.getConstWordManager().isSerializedWord(m_word)) {
        m_isDigitalWord = true;
    } else {
        m_isDigitalWord = false;
        m_sWord = word;
    }
}

void MeanWord::setBasicWord(const std::string& basicWord, const DataManager& dataManager)
{
    m_basicWord = dataManager.getConstWordManager().find("<b>"+basicWord);

    if( m_basicWord == -1 ) {
        m_sBasicWord = basicWord;
    }
}

const std::string& MeanWord::getStrWord(const DataManager& dataManager) const
{
    return (m_isDigitalWord == true) ? dataManager.getConstWordManager().find(m_word) : m_sWord;
}

std::string MeanWord::getStrBasicWord(const DataManager& dataManager) const
{
    return (m_basicWord > -1) ? dataManager.getConstWordManager().find(m_basicWord).substr(3) : m_sBasicWord;
}

std::string MeanWord::getStrTags(const DataManager& dataManager) const
{
    std::string s;

    for(size_t i=0; i<m_tags.size(); i++) {
        if( i != 0 ) {
            s += ",";
        }
        s += dataManager.getConstTagManager().find(m_tags[i]);
    }

    return s;
}

void MeanWord::dump(const DataManager& dataManager,std::ostream& ofs) const
{
    //dump word
    if(m_isDigitalWord == true) {
        ofs<<getStrWord(dataManager);
    } else {
        ofs<<m_sWord;
    }
    //dump tag
    if(!m_tags.empty()) {
        ofs<<"|";
        ofs<<getStrTags(dataManager);
    }
}

std::string MeanWord::dump2Str(const DataManager& dataManager) const
{
    stringstream s;
    dump(dataManager,s);
    return s.str();
}

bool MeanWord::operator ==(const MeanWord& otherWord) const
{
    return this->m_isDigitalWord != otherWord.m_isDigitalWord ||
           this->m_word != otherWord.m_word ||
           this->m_sWord != otherWord.m_sWord ||
           this->m_tags != otherWord.m_tags ||
           this->m_cacheBasicOrg != otherWord.m_cacheBasicOrg
           ? false : true;
}

bool MeanWord::operator !=(const MeanWord& otherWord) const
{
    return !( *this == otherWord );
}

void MeanWord::setTags(const SVector<int>& tags)
{
    std::set<int> filter;
    size_t i;

    for(i=0; i<tags.size(); i++) {
        filter.insert(tags[i]);
    }
    m_tags.clear();
    for(std::set<int>::const_iterator ite=filter.begin(); ite!=filter.end(); ++ite) {
        m_tags.push_back(*ite);
    }
}

void MeanWords::dump(std::ostream& ofs) const
{
    // dump sentence tag
    if(!m_tags.empty()) {
        ofs<<"[";
        ofs<<getStrTags();
        ofs<<"] : ";
    }    

    // dump words
    for(size_t i=0; i<(*this).size(); i++) {
        if(i != 0) {
            ofs<<"  ";
        }
        (*this)[i].dump(*mp_DataManager,ofs);
    }
}

std::string MeanWords::dump2Str() const
{
    stringstream s;
    dump(s);
    return s.str();
}

std::string MeanWords::dumpHoldWords() const
{
    std::string s;

    for(size_t i=0; i<m_holdWords.size(); i++) {
        s += " " + m_holdWords[i].dump2Str(*mp_DataManager);
    }

    return s;
}

MeanWords::MeanWords(const std::vector<std::string>& vWord, DataManager* pData)
{
    if(vWord.empty()) return;

    mp_DataManager = pData;
    for(size_t i=0; i<vWord.size(); i++) {
        MeanWord meanWord;

        meanWord.setWord(vWord[i],*mp_DataManager);
        meanWord.setBasicWord(vWord[i],*mp_DataManager);
        meanWord.m_cacheBasicOrg = vWord[i];
        (*this).push_back(meanWord);
    }

    this->m_orignalString = "";
}

MeanWords::MeanWords(const aisms::Words& words, DataManager* pData)
{
    if(words.empty()) return;
	
	std::set<std::string>::const_iterator iter;
    mp_DataManager = pData;
	
    for(size_t i=0; i<words.size(); i++) {
        MeanWord meanWord;
		
        meanWord.setWord(words.at(i).getWord(),*mp_DataManager);
        meanWord.setBasicWord(words.at(i).getWord(),*mp_DataManager);
        meanWord.m_cacheBasicOrg = words.at(i).getWord();
		const std::set<std::string>& seTags = words.at(i).getTags();
		for(iter=seTags.begin(); iter!=seTags.end(); ++iter){
			int nTag = mp_DataManager->getTagManager().find(*iter);
			if(nTag != -1){
				meanWord.addTag(mp_DataManager->getTagManager().find(*iter));
			}
		}
        (*this).push_back(meanWord);
    }
	
	const std::set<std::string>& seTags = words.getWordTags();
	for(iter=seTags.begin(); iter!=seTags.end(); ++iter){
		this->m_tags.push_back(mp_DataManager->getTagManager().find(*iter));
	}
	
    this->m_orignalString = "";
}

MeanWords::MeanWords(MeanWordsStringInterface& stringInterface, DataManager* pData)
{
    if(stringInterface.getOriginalString().empty()) return;
    if(stringInterface.getWordStrings().size()<1) return;

    mp_DataManager = pData;
    for(size_t i=0; i<stringInterface.getWordStrings().size(); i++) {
        MeanWord meanWord;

        if(stringInterface.getWordHinshi(i)<0) continue;
        if(stringInterface.getWordInput(i).empty()) continue;
        if(stringInterface.getWordBasic(i).empty()) continue;
        if(stringInterface.getWordBasicOrg(i).empty()) continue;

        meanWord.setWord(stringInterface.getWordInput(i),*mp_DataManager);
        meanWord.setBasicWord(stringInterface.getWordBasic(i),*mp_DataManager);
        meanWord.m_cacheBasicOrg = stringInterface.getWordBasicOrg(i);
        meanWord.m_tags = stringInterface.getWordTags(i);
        (*this).push_back(meanWord);
    }

    (*this).m_tags = stringInterface.getTags(mp_DataManager);

    (*this).m_orignalString = stringInterface.getOriginalString();
    (*this).m_orignalString = stringInterface.getOriginalString();
}

MatchingMatrix MeanWords::generateMatchingMatrixFromWord(const RANGE& range, const bool& isBasicWord) const
{
    MatchingMatrix matrix;
    SVector<int> v;

    v.push_back(0);
    if(isBasicWord == true) {
        for(int i=range.start; i<=range.end; i++) {
            v[0] = (*this)[i].getBasicWord();
            matrix.push_back(v);
        }
    } else {
        for(int i=range.start; i<=range.end; i++) {
            v[0] = (*this)[i].getWord();
            matrix.push_back(v);
        }
    }

    if( range.isBackwardSearch() ) matrix.setToBackWardSearch();
    if( range.isContinuousSearch() ) matrix.setFirstNeedToMatchPosition();

    return matrix;
}

MatchingMatrix MeanWords::generateMatchingMatrixFromTag(const RANGE& range) const
{
    MatchingMatrix matrix;
    SVector<int> v;

    for(int i=range.start; i<=range.end; i++) {
        v = (*this)[i].getTags();
        matrix.push_back(v);
    }

    if( range.isBackwardSearch() ) matrix.setToBackWardSearch();
    if( range.isContinuousSearch() ) matrix.setFirstNeedToMatchPosition();

    return matrix;
}

MatchingMatrix MeanWords::generateMatchingMatrixFromSentenceTag() const
{
    MatchingMatrix matrix;
    SVector<int> v;

    for(size_t i=0; i<m_tags.size(); i++) {
        v.push_back(m_tags[i]);
    }
    matrix.push_back(v);

    return matrix;
}

SVector<int> MeanWords::generateDynamicContentFromWord(const RANGE& range, const int& offset,
        const DataManager& dataManager) const
{
    SVector<int> v;

    for(int i=range.start; i<=range.end; i++) {
        if(offset == -1) {
            v.push_back( (*this)[i].getWord() );
        } else {
            const std::string& word = dataManager.getConstWordManager().find((*this)[i].getWord());
            std::string newWord;
            if(offset >= (int)word.length()) {
                newWord = Utility::getIndexCharInString(word,word.length()-1);
            } else {
                newWord = Utility::getIndexCharInString(word,offset);
            }
            v.push_back( dataManager.getConstWordManager().find(newWord) );
        }
    }

    return v;
}

std::string MeanWords::getStrTags() const
{
    std::string s;

    for(size_t i=0; i<m_tags.size(); i++) {
        if( i != 0 ) {
            s += ",";
        }
        s += mp_DataManager->getTagManager().find(m_tags[i]);
    }

    return s;
}

void MeanWords::setHoldWords(const std::vector<MeanWord>& words)
{
    m_holdWords = words;
}

const std::vector<MeanWord>& MeanWords::getHoldWords() const
{
    return m_holdWords;
}

bool MeanWords::operator ==(const MeanWords& otherWords) const
{
    size_t i;

    if(this->size() != otherWords.size()) return false;
    if(this->m_tags.size() != otherWords.m_tags.size()) return false;
    if(this->m_holdWords.size() != otherWords.m_holdWords.size()) return false;

    for(i=0; i<this->size(); i++) {
        if( (*this)[i] != otherWords[i] ) return false;
    }

    if(this->m_tags != otherWords.m_tags) return false;

    for(i=0; i<this->m_holdWords.size(); i++) {
        if( this->m_holdWords[i] != otherWords.m_holdWords[i] ) return false;
    }

    if(this->m_orignalString != otherWords.m_orignalString) return false;

    return true;
}

bool MeanWords::operator !=(const MeanWords& otherWords) const
{
    return !( *this == otherWords );
}

void MeanWords::setTags(const SVector<int>& tags)
{
    std::set<int> filter;
    size_t i;

    for(i=0; i<tags.size(); i++) {
        filter.insert(tags[i]);
    }
    m_tags.clear();
    for(std::set<int>::const_iterator ite=filter.begin(); ite!=filter.end(); ++ite) {
        m_tags.push_back(*ite);
    }
}

std::map<std::string,std::string> MeanWords::getTagContentMap() const
{
	std::map<std::string,std::string> mTagContent;
	std::map<std::string,std::string>::iterator iter;
	
	for(size_t i=0; i<this->size(); i++){
		const SVector<int>& vTags = this->at(i).getTags();
		for(size_t j=0; j<vTags.size(); j++){
			const std::string sTag = mp_DataManager->getTagManager().find(vTags[j]);
			iter = mTagContent.find(sTag);
			if(iter == mTagContent.end()){
				mTagContent.insert(std::make_pair(sTag,this->at(i).getStrWord(*mp_DataManager)));
			}
		}
	}
	
	return mTagContent;
}

std::multimap<std::string,std::string> MeanWords::getTagContentMultiMap() const
{
	std::multimap<std::string,std::string> mmTagContent;
	std::multimap<std::string,std::string>::iterator iter;
	
	for(size_t i=0; i<this->size(); i++){
		const SVector<int>& vTags = this->at(i).getTags();
		for(size_t j=0; j<vTags.size(); j++){
			const std::string sTag = mp_DataManager->getTagManager().find(vTags[j]);
			mmTagContent.insert(std::make_pair(sTag,this->at(i).getStrWord(*mp_DataManager)));
		}
	}
	
	return mmTagContent;
}

}//namespace sae
