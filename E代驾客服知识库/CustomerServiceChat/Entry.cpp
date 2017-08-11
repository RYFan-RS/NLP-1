#include "Entry.h"
namespace robosay{
namespace base{

Entry::Entry(){
}

Entry::~Entry(){
	m_mWordIdxs.clear();
}

IntSet Entry::getIdx(const StringVector& vWords){
	IntSet vRet;
	const StringVector::const_iterator last = vWords.end();
	StringVector::const_iterator itr = vWords.begin();
	for(; last != itr; ++itr){
		StringIntSetMap::const_iterator itrFind = m_mWordIdxs.find(*itr);
		if(itrFind != m_mWordIdxs.end()){
			set_union(vRet,itrFind->second);
		}
	}
	return vRet;
}

IntSet Entry::getIdx(const StringStringMap& mWords){
	IntSet vRet;
	const StringStringMap::const_iterator last = mWords.end();
	StringStringMap::const_iterator itr = mWords.begin();
	for(; last != itr; ++itr){
		StringIntSetMap::const_iterator itrFind = m_mWordIdxs.find(itr->first);
		if(itrFind != m_mWordIdxs.end()){
			set_union(vRet,itrFind->second);
		}
	}
	return vRet;
}
}//namespace base
}//namespace robosay
