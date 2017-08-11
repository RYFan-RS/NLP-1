#ifndef __TFIDF__REMEND__FZ__H__
#define __TFIDF__REMEND__FZ__H__

#include "TfidfValue.h"
#include "WordPropertyDef.h"
#include <map>
#include <set>
#include <string>
#include <vector>
#include <iostream>

namespace robosay{
namespace general{

/////////////////////////////////////////////////////////////////////////

class CWordPropertyConflict
{
public:
	void AddConflictWord(const std::string& strWord)
	{
		m_conflicts[strWord]++;
	}

	void OutputConflictWordInfos(std::ostream& ofs, const std::map<std::string, WP_TYPE>& wordProperty)const;
	
private:
	//first conflict word, second : conflict number
	std::map<std::string, int> m_conflicts;
};

class CTfIdfRemend
{
public:
	//init
	void AddWord(const std::string& strWord, WP_TYPE wordProperty);

	void AddWordIndex(int iWordIndex, WP_TYPE wordProperty)
	{
		//名词不进行校正
		if ((WP_Noun==wordProperty) || (WP_Buff==wordProperty)) { return; }
		
		(void)m_wordsIndexs.insert(std::make_pair(iWordIndex, wordProperty));
	}

	//callBackFunc : function prototype support -> void (*)(const string&, WP_TYPE)const
	template<class CallBackFunc>
	void ForEachWord(CallBackFunc callBackFunc)const
	{
		std::map<std::string,WP_TYPE>::const_iterator iter = m_words.begin();
		for (; iter!=m_words.end(); ++iter)
		{
			callBackFunc(iter->first, iter->second);
		}
	}

public:
	bool IsExistWordIndex(int iWordIndex)const
	{
		return m_wordsIndexs.find(iWordIndex) != m_wordsIndexs.end();
	}

	WP_TYPE GetPartsOfSpeechByWordIndex(int iWordIndex)const;

public:
	//calc
	void RemandNeedToMatchTfIdf(std::vector<TfidfValue>& tfIdfQuestion, std::vector<TfidfValue>& tfIdfLibary)const;

	void OutputWordProperty(std::ostream& ofs)const;
	void OutputConflictWordInfos(std::ostream& ofs)const
	{
		m_wordPropertyConflict.OutputConflictWordInfos(ofs,m_words);
	}
	
private:
	//first: word; second: Word Property, 
	std::map<std::string, WP_TYPE> m_words;//所有分词的词性
	
	//first: wordIndex; second: Word Property,
	std::map<int, WP_TYPE> m_wordsIndexs;//需要校正的分词索引及词性

	CWordPropertyConflict m_wordPropertyConflict;//所有分词的词性冲突统计
};


/////////////////////////////////////////////////////////////////////////
class CTfIdfRemendCalc
{
	//typedef void (CTfIdfRemendCalc::* RemendTfIdfFuncType)(TfidfValue& tfidfValueElem);
	
public:
	CTfIdfRemendCalc(const CTfIdfRemend& tfIdfRemend,std::vector<TfidfValue>* pTfIdfValues)
		:m_tfIdfRemend(tfIdfRemend),m_pTfIdfValues(pTfIdfValues),m_argValueOfNoRemendWords(0.0f)
	{
	}
	~CTfIdfRemendCalc(){ m_pTfIdfValues = NULL; }

public:
	void Remend(bool bIsRemedy=true);
	void MutualRemend(CTfIdfRemendCalc& tfIdfRemendCalc);

private:
	bool Initilize();	

	TfidfValue* GetTfidfValueByIndex(int tfIdfValuesIndex);
	bool IsExistWordIndex(int wordIndex, int& tfIdfValuesIndex)const;
	bool IsNeedToRemend()const
	{
		return !m_remendIndexs.empty();
	}

private:
	void RemendTfIdf();
	void RemendTfIdf(TfidfValue& tfidfValueElem);
	void RemendTfIdfCommon(TfidfValue& tfidfValueElem);
	void RemendTfIdfForVerb(TfidfValue& tfidfValueElem);
	void RemendTfIdfForQualifier(TfidfValue& tfidfValueElem);
	//do nothing
	//void RemendTfIdfForVoid(TfidfValue& tfidfValueElem){ }

	void AddAllToRemedyIndex();
	
private:
	const CTfIdfRemend& 		m_tfIdfRemend;
	std::vector<TfidfValue>* 	m_pTfIdfValues;

	//first : word index, second : m_pTfIdfValues elem index
	std::map<int,int>			m_remendIndexs;
	float						m_argValueOfNoRemendWords;

	//static RemendTfIdfFuncType	m_remendTfIdfFunc[POS_Full];
};


}
}

#endif

