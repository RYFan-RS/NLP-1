#include "TfIdfRemend.h"
#include <ostream>

namespace robosay{
namespace general{

/*
//static member initialize
CTfIdfRemendCalc::RemendTfIdfFuncType	CTfIdfRemendCalc::m_remendTfIdfFunc[WP_Buff] = 
{
	&CTfIdfRemendCalc::RemendTfIdfForVerb,			//Verb		WP_Verb
	&CTfIdfRemendCalc::RemendTfIdfForQualifier,		//Qualifier	WP_Qualifier
};
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

void CWordPropertyConflict::OutputConflictWordInfos(std::ostream& ofs,const std::map<std::string, WP_TYPE>& wordProperty)const
{
	std::vector<std::string> vecDongci;
	std::vector<std::string> vecXiushici;	
	
	if (!ofs) { return; }

	if (m_conflicts.empty())
	{
		ofs << "恭喜您，分词词性没有冲突哦" << std::endl;
		//return;
	}
	else
	{
		ofs << "冲突分词\t冲突次数\n";
		std::map<std::string, int>::const_iterator iter = m_conflicts.begin();
		
		for (; iter!=m_conflicts.end(); ++iter)
		{
			ofs << iter->first << "\t" << iter->second << "\n";
		}
		
	}	

	ofs << std::endl;


	std::map<std::string, WP_TYPE>::const_iterator iter1 = wordProperty.begin();
	
	for (; iter1!=wordProperty.end(); ++iter1)
	{
		if(iter1->second == WP_Verb)
			vecDongci.push_back(iter1->first);
		else if(iter1->second == WP_Qualifier)
			vecXiushici.push_back(iter1->first);
	}

	int i=0;

	ofs << "所有动词关键字\n";
	std::vector<std::string>::const_iterator iter2 = vecDongci.begin();
	
	for (; iter2!=vecDongci.end(); ++iter2)
	{
		ofs << *iter2 << "\t";
		if(++i == 5){
			ofs << std::endl;	
			i = 0;
		}
	}
	ofs << std::endl;	

	ofs << "所有修饰词关键字\n";
	iter2 = vecXiushici.begin();
	i=0;
	for (; iter2!=vecXiushici.end(); ++iter2)
	{
		ofs << *iter2 << "\t";
		if(++i == 5){
			ofs << std::endl;	
			i = 0;
		}		
	}
	ofs << std::endl;	
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

void CTfIdfRemend::AddWord(const std::string& strWord, WP_TYPE wordProperty)
{
	//(void)m_words.insert(std::make_pair(strWord,wordProperty));

	typedef std::map<std::string, WP_TYPE>::iterator iterator;

	std::pair<iterator, bool> ret = m_words.insert(std::make_pair(strWord,wordProperty));
	
	if (!ret.second)  //存在此key,没有插入
	{
		WP_TYPE existProperty = ret.first->second;
		if(existProperty!=wordProperty)
		{
			if(existProperty==WP_Noun)
				ret.first->second = wordProperty;
			else if(wordProperty==WP_Noun)
				ret.first->second = existProperty;
			else
				m_wordPropertyConflict.AddConflictWord(strWord);
		}
	}
}


void CTfIdfRemend::RemandNeedToMatchTfIdf(std::vector<TfidfValue>& tfIdfQuestion, std::vector<TfidfValue>& tfIdfLibary)const
{
	if (tfIdfQuestion.empty() && tfIdfLibary.empty()) { return; }
	
	CTfIdfRemendCalc tfIdfRemendCalcQue(*this,&tfIdfQuestion);
	tfIdfRemendCalcQue.Remend();
	
	CTfIdfRemendCalc tfIdfRemendCalcLib(*this,&tfIdfLibary);
	//tfIdfRemendCalcLib.Remend();
	tfIdfRemendCalcLib.Remend(false);

	tfIdfRemendCalcQue.MutualRemend(tfIdfRemendCalcLib);	
}

WP_TYPE CTfIdfRemend::GetPartsOfSpeechByWordIndex(int iWordIndex)const
{
	std::map<int, WP_TYPE>::const_iterator iter = m_wordsIndexs.find(iWordIndex);
	
	if (iter == m_wordsIndexs.end())
	{
		return WP_Buff;
	}

	return iter->second;
}

void CTfIdfRemend::OutputWordProperty(std::ostream& ofs)const
{
	if (!ofs) { return; }

	std::map<std::string,WP_TYPE>::const_iterator iter = m_words.begin();
	for (; iter!=m_words.end(); ++iter)
	{
		ofs << iter->first << "\t" << iter->second << "\n";
	}

	ofs << std::endl;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
void CTfIdfRemendCalc::Remend(bool bIsRemedy)
{
	if (!bIsRemedy)
	{
		//当不需要校正时，则认为没有校正的就是已经校正，以便之后联合校正(MutualRemend)
		AddAllToRemedyIndex();
		return;
	}
	
 	if (!Initilize()) { return; }

	if (!IsNeedToRemend()) { return; }

	RemendTfIdf();
}

void CTfIdfRemendCalc::MutualRemend(CTfIdfRemendCalc& rightTfIdfRemendCalc)
{
	if (!IsNeedToRemend() || !rightTfIdfRemendCalc.IsNeedToRemend()) { return; }
	
	int tfIdfValuesIndex = -1;
	std::map<int,int>::iterator iLeftIter = m_remendIndexs.begin();
	for (; iLeftIter != m_remendIndexs.end(); ++iLeftIter)
	{
		if (!rightTfIdfRemendCalc.IsExistWordIndex(iLeftIter->first,tfIdfValuesIndex)) { continue; }

		TfidfValue* pTfidfValueLeft = GetTfidfValueByIndex(iLeftIter->second);
		if (NULL == pTfidfValueLeft) { continue; }
		
		TfidfValue* pTfidfValueRight = rightTfIdfRemendCalc.GetTfidfValueByIndex(tfIdfValuesIndex);
		if (NULL == pTfidfValueRight) { continue; }
		
		float minValue = std::min(pTfidfValueLeft->m_nValue,pTfidfValueRight->m_nValue);
		pTfidfValueLeft->m_nValue = minValue;
		pTfidfValueRight->m_nValue = minValue;
	}
}

bool CTfIdfRemendCalc::Initilize()
{
	if (NULL == m_pTfIdfValues) { return false; }
	if (m_pTfIdfValues->empty()) { return false; }
	
	std::vector<TfidfValue>::iterator iter = m_pTfIdfValues->begin();
	for (size_t i=0; i<m_pTfIdfValues->size(); ++i)
	{
		TfidfValue& elem = (*m_pTfIdfValues)[i];
		if (m_tfIdfRemend.IsExistWordIndex(elem.m_nID))
		{
			m_remendIndexs.insert(std::make_pair(elem.m_nID,i));
		}
		else
		{
			m_argValueOfNoRemendWords += elem.m_nValue;
		}
	}

	int noRemendWordsNum = m_pTfIdfValues->size() - m_remendIndexs.size();
	m_argValueOfNoRemendWords = m_argValueOfNoRemendWords / noRemendWordsNum;
	return true;
}


void CTfIdfRemendCalc::RemendTfIdf()
{
	std::map<int,int>::iterator iter = m_remendIndexs.begin();
	while (iter != m_remendIndexs.end())
	{
		TfidfValue* pTfidfValue = GetTfidfValueByIndex(iter->second);
		if (NULL != pTfidfValue)
		{
			RemendTfIdf(*pTfidfValue);
			//WP_TYPE partsOfSpeech = m_tfIdfRemend.GetPartsOfSpeechByWordIndex(pTfidfValue->m_nID);
			//(this->*m_remendTfIdfFunc[partsOfSpeech])(*pTfidfValue);
		}
		++iter;
	}
}

void CTfIdfRemendCalc::RemendTfIdf(TfidfValue& tfidfValueElem)
{
	WP_TYPE wordProperty = m_tfIdfRemend.GetPartsOfSpeechByWordIndex(tfidfValueElem.m_nID);
	if (WP_Verb == wordProperty)
	{
		RemendTfIdfForVerb(tfidfValueElem);
	}
	else if (WP_Qualifier == wordProperty)
	{
		RemendTfIdfForQualifier(tfidfValueElem);
	}
}

void CTfIdfRemendCalc::RemendTfIdfCommon(TfidfValue& tfidfValueElem)
{
	float fValue = tfidfValueElem.m_nValue;
	
	if (fValue < m_argValueOfNoRemendWords) { return; }

	float fRate = m_argValueOfNoRemendWords/(fValue+m_argValueOfNoRemendWords);
	
	tfidfValueElem.m_nValue = fValue * fRate;
	
}

void CTfIdfRemendCalc::RemendTfIdfForVerb(TfidfValue& tfidfValueElem)
{
	RemendTfIdfCommon(tfidfValueElem);
	tfidfValueElem.m_nValue *= 0.7;
}
void CTfIdfRemendCalc::RemendTfIdfForQualifier(TfidfValue& tfidfValueElem)
{
	RemendTfIdfCommon(tfidfValueElem);
	tfidfValueElem.m_nValue *= 0.5;
}

TfidfValue* CTfIdfRemendCalc::GetTfidfValueByIndex(int tfIdfValuesIndex)
{
	if (NULL == m_pTfIdfValues) { return NULL; }

	if ((tfIdfValuesIndex<0) || (tfIdfValuesIndex>=m_pTfIdfValues->size()))
	{
		return NULL;
	}

	return &(*m_pTfIdfValues)[tfIdfValuesIndex];
}

bool CTfIdfRemendCalc::IsExistWordIndex(int wordIndex, int& tfIdfValuesIndex)const
{
	std::map<int,int>::const_iterator iter = m_remendIndexs.find(wordIndex);
	if (iter == m_remendIndexs.end())
	{
		return false;
	}

	tfIdfValuesIndex = iter->second;
	return true;
}

void CTfIdfRemendCalc::AddAllToRemedyIndex()
{
	if (NULL == m_pTfIdfValues) { return; }
	
	std::vector<TfidfValue>::iterator iter = m_pTfIdfValues->begin();
	for (size_t i=0; i<m_pTfIdfValues->size(); ++i)
	{
		TfidfValue& elem = (*m_pTfIdfValues)[i];
		m_remendIndexs.insert(std::make_pair(elem.m_nID,i));
	}
}



}
}


