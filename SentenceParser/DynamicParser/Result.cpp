#include "Result.h"

namespace aisms
{
namespace dypsr
{

void Result::add(const Range& range)
{
	std::map<size_t,Range>::iterator iter;
	if(this->at(range.first) != " "){
		iter = m_mRange.find(range.first);
		// 完整包含以前的结果
		if(iter != m_mRange.end()){
			std::map<size_t,Range>::iterator it;
			bool bAbandon = false;
			for(int i=range.first; i<=range.second; i++){
				it = m_mRange.find(i);
				if(it != m_mRange.end() && (it == m_mRange.end() || it->second.second != i) ){
					bAbandon = true;
					break;
				}
			}
			if(!bAbandon){
				for(size_t i=range.first; i<=range.second; i++){
					if(iter != m_mRange.end()){
						if(m_mRange.find(i) == m_mRange.end()) continue;
						m_mRange.erase(iter++);
					}
				}
				m_mRange.insert(std::make_pair(range.first,range));
			}
		}else{
			// 完整的被以前的结果包含或者和以前的结果冲突
			bool bAbandon = false;
			for(iter=m_mRange.begin(); iter!=m_mRange.end(); ++iter){
				if(iter->second.second < range.first) continue;
				if(iter->second.first <= range.first && iter->second.second >= range.second){
					bAbandon = true;
					break;
				}else if(iter->second.first <= range.first && iter->second.second < range.second){
					bAbandon = true;
					break;
				}else if(iter->second.first > range.first && iter->second.second <= range.second){
					bAbandon = true;
					break;
				}
			}
			if(!bAbandon){
				m_mRange.insert(std::make_pair(range.first,range));
			}
		}
	}
}

std::string Result::getInput() const
{
    std::string sRet;

    for(size_t i=0; i<this->size(); i++) {
        sRet += this->at(i);
    }

    return sRet;
}

std::string Result::toStr() const
{
    std::map<size_t,Range>::const_iterator iter;
	
    std::stringstream ss;
	ss << "{";
    for(iter=m_mRange.begin(); iter!=m_mRange.end(); ++iter) {
		ss << "["<<iter->second.first<<","<<iter->second.second<<"]";
    }
	ss << "}";

    return ss.str();
}

std::vector<std::string> Result::makeCharFromStr(const acl::String& s) const
{
    std::vector<std::string> vRet, v = s.toCharactor();
    std::string sWord;

    for(size_t i=0; i<v.size(); i++) {
        //if(v.at(i).length() > 1 || !acl::Utility::isAlphaNumber(v.at(i).at(0))){
		if(v.at(i).length() > 1 || (!acl::Utility::isAlphaNumber(v.at(i).at(0)) && v.at(i)!="-")){
            if(!sWord.empty()){
                vRet.push_back(sWord);
                sWord.erase(sWord.begin(),sWord.end());
            }
            vRet.push_back(v.at(i));
        }else{
            sWord += v.at(i);
        }
    }

    if(!sWord.empty()){
        vRet.push_back(sWord);
    }

    return vRet;
}

void Result::readWords(const std::vector<std::string>& vWord){
	int nBeginPos = 0, nEndPos = 0;
	bool bLastIsAplhaNumber = false;

	for(size_t i=0; i<vWord.size(); i++){
		std::vector<std::string> vChar = makeCharFromStr(vWord.at(i));
		for(size_t j=0; j<vChar.size(); j++){
			this->push_back(vChar.at(j));
		}
		bool bIsAplhaNumber = acl::Utility::isAlphaNumber(vWord.at(i));
		if(bIsAplhaNumber && bLastIsAplhaNumber){
			this->pop_back();
			this->at(this->size()-1) += vWord.at(i);
		}else{
			if(bIsAplhaNumber) bLastIsAplhaNumber = true;
			else bLastIsAplhaNumber = false;
			nEndPos = nBeginPos + vChar.size() - 1;
			Range range(nBeginPos,nEndPos);
			m_mRange.insert(std::make_pair(nBeginPos,range));
			nBeginPos = nEndPos + 1;
		}
	}
}

std::vector<std::string> Result::getWords() const
{
	if(m_mRange.empty()){
		return *this;
	}else{
		std::vector<std::string> vWord;
		std::map<size_t,Range>::const_iterator iter;

		for(iter=m_mRange.begin(); iter!=m_mRange.end(); ++iter) {
			std::string sWord;
			for(int i=iter->second.first; i<=iter->second.second; i++) {
				sWord += this->at(i);
			}
			vWord.push_back(sWord);
		}

		return vWord;
	}
}

std::map<size_t,Range> Result::getRanges() const
{
	return m_mRange;
}

void Result::setRanges(const std::map<size_t,Range>& mRange){
	m_mRange = mRange;
}

/*
size_t Result::getNextUnparsedPos(size_t uPos) const{
	std::map<size_t,Range>::const_iterator iter = m_mRange.find(uPos);
	if(iter == m_mRange.end()) return uPos;
	
	for(iter=m_mRange.begin(); iter!=m_mRange.end(); ++iter){
		if(iter->first == uPos && 
	}
}
*/
}//dypsr
}//namespace aisms
