#include "WordNearMeaning.h"
#include "Common/lib4/Definations.hpp"

namespace robosay
{
namespace general
{

bool WordNearRule::read(const acl::String& line){
	std::vector<std::string> vWords = line.separate(g_WordMeanRule__Internal_Separator);
	if(vWords.size()==0){
		return false;
	}
	
	vFrom = vWords;
	
	if(filter() == false){
		PRINT("Empty word is not allowed!")
		return false;
	}
	
	return true;
}

std::string WordNearRule::handleWordNearRule(const acl::Line& line, std::vector<WordNearRule>& vecWordNearRule){
	std::string ruleId;
	std::vector<acl::String> vWords = line.separate_(g_WordMeanRule_Id_Separator);
	
	if(vWords.size() != 2 || vWords[0].empty() || vWords[1].empty()){
		return ruleId;
	}
	
	vWords[0].toUpper();
	vWords[0].strip();
	
	vWords[1].strip();
	
	ruleId = vWords[0];

	std::vector<acl::String> rules = vWords[1].separate_(g_WordMeanRule_Extenrnal_Separator);

	for(int i=0;i<rules.size();i++){
		WordNearRule rule;
		rules[i].strip();
		if(rule.read(rules[i])){
			vecWordNearRule.push_back(rule);
		}
	}

	return ruleId;
}

std::string WordNearRule::getFirstChar() const{
	return vFrom[0];
}

bool WordNearRule::filter() {
	std::vector<std::string> tempVec;
	bool result = false;
	for(size_t i=0; i<vFrom.size(); i++){
		if(!vFrom[i].empty()) {
			acl::String temp(vFrom[i]);
			temp.toUpper();
			temp.strip();
			if(temp.empty()){
				tempVec.push_back(vFrom[i]);
			}else
			{
				tempVec.push_back(temp);
			}
		}
	}

	vFrom.swap(tempVec);
	if(vFrom.size()==0)
		return false;

	for(size_t i=0; i<vFrom.size(); i++){
		acl::String temp(vFrom[i]);
		temp.strip();
		if((!temp.empty())&&(temp!=" ")){
			result = true;
			break;
		}
	}
	
	return result;
}

bool WordNearRule::process(std::vector<std::string>& vWord, const size_t& nPos, const std::map<int,std::string>& wordIdtoStrMap) const{
	size_t nTargetLen = vWord.size() - nPos;
	size_t nSourceLen = vFrom.size();

	std::string wordIdStr;
	std::map<int,std::string>::const_iterator itr=wordIdtoStrMap.find(m_wordId);
	if(itr!=wordIdtoStrMap.end()){
		wordIdStr = itr->second;
	}
	else{
		return false;
	}
	
	if(wordIdStr.empty()) return false;
	if(nSourceLen > nTargetLen) return false;
	
	for(size_t i=0; i<nSourceLen; i++){
		if(vFrom[i] != vWord[nPos+i]) return false;
	}

	if(nSourceLen==1 &&(wordIdStr==vFrom[0])){
		return  false;
	}
	
	std::vector<std::string> vNewWord;
	for(size_t i=0; i<nPos; i++){
		vNewWord.push_back(vWord[i]);
	}
	
	vNewWord.push_back(wordIdStr);
	
	for(size_t i=nPos+nSourceLen; i<vWord.size(); i++){
		vNewWord.push_back(vWord[i]);
	}
	
	vWord.swap(vNewWord);
	
	return true;
}

bool WordNearMeaning::initNormalFile(const std::string& filename){
	acl::File file;
	file.setCommentMarks("#");
	if(file.read(filename) == false){
		PRINT("Can't open WordNearMeaning dictonary ["<<filename<<"]")
		return false;
	}
	
	return readFile(file);
}

bool WordNearMeaning::readFile(const acl::File& file){
	for(size_t i=0; i<file.size(); i++){
		std::vector<WordNearRule> vecWordNearRule;
		std::string wordId;
		const acl::Line& line = file[i];
		
		//if(line.empty() || line.isBeginWith("#")) continue;
		if(line.empty()) continue;

		wordId = WordNearRule::handleWordNearRule(line, vecWordNearRule);
		if(wordId.empty() || vecWordNearRule.size()==0){
			continue;
		}
		/*add code here*/
		std::pair<std::map<int,std::string>::iterator,bool> ret;
		ret = m_m_words.insert(std::make_pair(m_m_words.size(),wordId));
		if(ret.second == false){
			continue;
		}

		for(int j=0;j<vecWordNearRule.size();j++){
			vecWordNearRule[j].setWordId(ret.first->first);
			m_m_FirstChar_Rule.insert(std::make_pair(vecWordNearRule[j].getFirstChar(),vecWordNearRule[j]));
		}
	}
	
	return true;
}

bool WordNearMeaning::process(std::vector<std::string>& vWord) const{
	bool result = false;
	
	typedef std::multimap<std::string, WordNearRule>::const_iterator MyIter;
	std::pair<MyIter,MyIter> pIter;
	
	for(size_t i=0; i<vWord.size(); i++){
		pIter = m_m_FirstChar_Rule.equal_range(vWord[i]);
		if(pIter.first != pIter.second){
			for(MyIter iter=pIter.first; iter!=pIter.second; ++iter){
				if(iter->second.process(vWord,i,m_m_words) == true){
					result = true;
					break;
				}
			}
		}
	}

	return result;
}

}
}
