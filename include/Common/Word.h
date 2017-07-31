#ifndef __WORD_H__
#define __WORD_H__
#include <string>
#include <set>
#include <iostream>

namespace aisms{

class Word
{
public:
	std::string toStr() const{
		std::string s;
		
		s += m_sWord;
		if(!m_seTag.empty()){
			s += "[";
			for(std::set<std::string>::const_iterator i=m_seTag.begin(); i!=m_seTag.end(); ++i){
				if(i != m_seTag.begin()) s += "/";
				s += *i;
			}
			s += "]";
		}
		
		return s;
	}
	
	bool hasTag(const std::string& sTag) const{
		return m_seTag.find(sTag) == m_seTag.end() ? false : true;
	}
	
	void addTag(const std::string& sTag){
		if(!sTag.empty()) m_seTag.insert(sTag);
	}
	
	const std::set<std::string>& getTags() const{
		return m_seTag;
	}
	
	void setTags(const std::set<std::string>& seTag){
		m_seTag = seTag;
	}
	
	const std::string& getWord() const{
		return m_sWord;
	}
	
	std::string& getWordRefer(){
		return m_sWord;
	}
	
	void setWord(const std::string& sWord){
		m_sWord = sWord;
	}
	
	void clearTags(){
		m_seTag.clear();
	}

	void setOriginalWord(const std::string& sOriginalWord){
		m_sOriginalWord = sOriginalWord;
	}

	const std::string& getOriginalWord()const{
		return m_sOriginalWord;
	}
	
private:
	std::set<std::string> m_seTag;
	std::string m_sWord;
	std::string m_sOriginalWord;
};

}//namespace aisms

#endif//__WORD_H__
