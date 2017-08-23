#ifndef __WORDS_H__
#define __WORDS_H__
#include <vector>
#include "Word.h"

namespace aisms{

class Words : public std::vector<Word>
{
public:
	Words(){}
	Words(const std::vector<std::string>& vWord){
		for(size_t i=0; i<vWord.size(); i++){
			Word word;
			word.setWord(vWord[i]);
			this->push_back(word);
		}
	}
	
	std::string toStr() const{
		std::string s;
		
		s = "{";
		for(size_t i=0; i<this->size(); i++){
			if(i != 0) s += ",";
			s += this->at(i).toStr();
		}
		s += "}";
		
		return s;
	}
	
	bool hasTag(const std::string& sTag) const{
		return m_seTag.find(sTag) == m_seTag.end() ? false : true;
	}
	
	void addTag(const std::string& sTag){
		m_seTag.insert(sTag);
	}
	
	const std::set<std::string>& getTags() const{
		return m_seTag;
	}
	
	std::set<std::string> getWordTags() const{
		std::set<std::string> seTag;
		
		for(size_t i=0; i<this->size(); i++){
			const std::set<std::string> seWordTag = this->at(i).getTags();
			std::copy(seWordTag.begin(),seWordTag.end(),std::inserter(seTag,seTag.begin()));
		}
		
		return seTag;
	}
	
	std::vector<std::string> getWords() const{
		std::vector<std::string> vWord;
		
		for(size_t i=0; i<this->size(); i++){
			vWord.push_back(this->at(i).getWord());
		}
		
		return vWord;
	}
	
	void addWord(const std::string& s){
		Word word;
		word.setWord(s);
		this->push_back(word);
	}
private:
	std::set<std::string> m_seTag;
};

}//namespace aisms

#endif//__WORD_H__
