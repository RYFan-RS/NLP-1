#ifndef __ROBOSAY__GENERAL_CHAT__QA__H__
#define __ROBOSAY__GENERAL_CHAT__QA__H__

#include "CustomerServiceChat/QA.h"
#include "CustomerServiceChat/GeneralChat/Path.h"
#include "CustomerServiceChat/Result.h"
#include "Sentence.h"

namespace robosay{
namespace general{

class QA : public robosay::base::QA
{
public:
	QA(const int nIdx)
		: robosay::base::QA(nIdx){
	}

	virtual ~QA(){

	}

	virtual robosay::base::Result* createResult() const{
		robosay::general::Path* pPath = new robosay::general::Path;
		if(!pPath){
			return NULL;
		}
		return new robosay::base::Result("general",pPath);
	}

	const std::vector<Sentence>& getQuestions() const{
		return m_vQuestion;
	}

	Sentence getQuestion(const int nIdx)const{
        if(nIdx < 0 || (nIdx >= (m_vQuestion.size()+m_vKeyWord.size()))){
            return Sentence();
        }

		if(nIdx < m_vQuestion.size()){
    		return m_vQuestion[nIdx];
		}
		else
			return m_vKeyWord[nIdx-m_vQuestion.size()];
	}

	std::vector<Sentence>& getQuestionsRefer(){return m_vQuestion;}
	virtual bool init(const robosay::XmlParser& xml, xmlNodePtr qaPtr);
	//float matchByTfidf(const std::vector<int>& vWordIdx, TFIDF* pTFIDF, int& nIdx);
	float matchByLD(const std::string& sIn, int& nIdx);
	float matchByPinyin(const std::vector<std::string>& vWordPinyin, int& nIdx);

    std::vector<Sentence>& getKeyWordsRefer() {
		return m_vKeyWord;
	}

	const std::vector< std::vector<std::string> >& getDicKeyWords() const{
		return m_dicKeyWord;
	}

	float matchByTfidf(const std::vector<std::vector<TfidfValue> >& vInTfidf, int& nIdx, const CTfIdfRemend& tfIdfRemend);

	const std::string getCurSemantic()const
	{
		return m_mSemantics.empty() ? "" : m_mSemantics.begin()->first;
	}

	void SetKeyWord(const std::string& strKeyWord);

	const std::map<WP_TYPE,std::set<std::string> >&  getWordProperty() const{
		return m_wordProperty;
	}

	bool init(const std::string& questionValue, const std::string& anwserValue)	;
	Answer_info getAnswerInfo(const StringStringMap& mSmtcWords, const StringStringMap* mPre, const std::string& sWay);

	void insertDefineTag(std::string defineTag){if(!defineTag.empty()) m_defineTags.insert(defineTag);}

	static  std::set<std::string> getTagsInSentence(const std::string& sentence);

	float matchByTags(const std::set<std::string>& tags,std::set<std::string>& setMatchTag) const;

	void setTitleSearchWords(const std::set<int>& src){m_titleSearchWords=src;}
	void setDetailSearchWords(const std::set<int>& src){m_detailSearchWords=src;}	

	void getTitleSameWords (const std::set<int>& src, std::set<int>& result);
	void getDetailSameWords(const std::set<int>& src, std::set<int>& result);	
private:
	std::vector<Sentence> m_vQuestion;
	std::vector<Sentence> m_vKeyWord;
	std::vector< std::vector<std::string> > m_dicKeyWord;
	std::map<WP_TYPE,std::set<std::string> >m_wordProperty;  //ДЪад
	std::set<std::string> m_defineTags;
	std::set<int>         m_titleSearchWords;
	std::set<int>         m_detailSearchWords;
};

}//namespace general
}//namespace robosay

#endif //__ROBOSAY__GENERAL_CHAT__QA__H__
