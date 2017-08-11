#ifndef __ROBOSAY_GENERAL__SENTENCE_H__
#define __ROBOSAY_GENERAL__SENTENCE_H__
#include <set>
#include <algorithm>
#include <iterator>
#include "DataModel/WordNetwork/WordNetwork.h"
#include "Common/lib4/Debug.hpp"
#include "DataModel/defines.h"
#include "SentenceParser/MixParser/Parser.h"
#include "DataModel/defines.h"
#include "Common/GlobalOption.h"
#include "TfidfValue.h"
#include "TFIDF.h"
#include "PINYIN.h"

namespace robosay{
namespace general{
using namespace aisms::datamodel;

#ifdef _GENERAL_SEARCH_DEBUG
const std::string DSTREAM = "GENERAL_SEARCH_DEBUG_LOG.log";
static std::stringstream ssDebug;
#endif

class Sentence
{
	public:
		void setWords(const std::set<std::string>& seWord);
		void setWords(const std::vector<std::string>& vWord);
		//void setTfidfValue(const std::vector<TfidfValue>& vTfidfValue);
		void setPinyin(const std::vector<std::string>& vWordPinyin);
		const std::set<std::string>& getWords() const;
		const std::vector<std::string>& getVWord() const {return m_vWord;}
		void setSentence(const std::string& sSentence, const std::string& sID);
		const std::string& getSentence() const;
		std::string getID()const;
		//int match(const Sentence& sentence, const std::set<std::string>& seExtWord, const wn::WordNetwork& wordNet, const aisms::mxpsr::Parser &mixParse) const;
		//void dump(std::ostream& ofs = std::cout) const;
		//void calcWeight(const aisms::mxpsr::Parser &mixParse);
		//int getWeight() const;
		//float matchByTfidf(const std::vector<int>& vWordIdx, TFIDF* pTFIDF);
		
		float matchByLD(const std::string& sIn);
		float matchByPinyin(const std::vector<std::string>& vWordPinyin);
		float matchByFuzzyPinyin(const std::vector<std::string>& vWordPinyin);
		//float matchByTfidf(const std::vector<TfidfValue> &vInTfidf, const CTfIdfRemend& tfIdfRemend);	
		float matchByTfidf(const std::vector<std::vector<TfidfValue> >&vInTfidf, const CTfIdfRemend& tfIdfRemend);		
		void  addOneSegment(std::vector<int>& segmentIds);
		const std::vector<std::vector<int> >& getAllSegmentMethods() const {return m_allSegmentMethods;}
		void  addTfidfValue(const std::vector<TfidfValue>& vTfidfValue);
	private:
		float matchByTfidf(const std::vector<TfidfValue>& tfidfValues1, const std::vector<TfidfValue>& tfidfValues2);
	private:
		std::string m_sSentence;
		std::string	m_sID;
		std::vector<std::vector<TfidfValue> > m_vTfidfValue;
		std::vector<std::vector<int> > m_allSegmentMethods;
		std::vector<std::string> m_vPinyin;
		std::set<std::string> m_seWord;
		std::vector<std::string> m_vWord;
		int m_nWeight;
		int caculateScore(const std::set<std::string> &stdMatched, int nInputWeight, const std::set<std::string> &stdInput, const aisms::mxpsr::Parser &mixParse) const;
		inline int calcWeight(const std::set<std::string> &lineWord, const aisms::mxpsr::Parser &mixParse) const;
		inline int getWeight(const Property& pro) const;
};

}//namespace general
}//namespace robosay

#endif //__ROBOSAY_GENERAL__SENTENCE_H__
