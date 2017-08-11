#ifndef __ROBOSAY__GENERAL__TFIDF_H__
#define __ROBOSAY__GENERAL__TFIDF_H__
#include <map>
#include "TfidfValue.h"
#include <math.h>
#include <fstream>
#include "Common/SortManager.h"
#include <boost/bind/bind.hpp>
#include "TfIdfRemend.h"

namespace robosay{
namespace general{

class TFIDF{
public:
	
	std::vector<TfidfValue> caculate(const std::vector<int>& vWord) const{
		std::map<std::string,int>::const_iterator iter;
		std::map<int, int>::const_iterator it,it1;
		std::vector<TfidfValue> vTfidfValue;
		std::map<int, int> word_frequency;
		
		// 构建句子的稀疏表示矩阵
		int length = vWord.size();         // 句子长度
		for(size_t i=0; i<vWord.size(); i++){
			if(word_frequency.find(vWord[i]) == word_frequency.end()){
				word_frequency.insert(std::make_pair(vWord[i], 1));
			}
			#ifndef FEATURE_TF_ONE_TIME
			else{
				word_frequency[vWord[i]]++;
			}
			#endif
		}
		
		/* 
		for(size_t i=0; i<vWord.size(); i++){
			iter = m_word_idx.find(vWord[i]);
			if(iter != m_word_idx.end()){   // 词典中包含该词
				if(word_frequency.find(iter->second) != word_frequency.end()){    // 添加词频
					word_frequency.insert(std::make_pair(iter->second, 1));
				}else{                                                            // 词频加1
					word_frequency[iter->second]++;
				}
			}
		}
		*/
		#ifdef FEATURE_TF_ONE_TIME
		length = word_frequency.size();
		#endif
		it = word_frequency.begin();
		for(; it!=word_frequency.end(); it++){
			float tf = float(it->second) / length;
			it1 = m_word_frequency.find(it->first);
			float idf = log10(float(m_questionNum) / (it1->second+1));
			float tfidf = tf * idf;
			robosay::general::TfidfValue value;
			value.m_nID = it->first;
			value.m_nValue = tfidf;
			vTfidfValue.push_back(value);
		}
		
		return vTfidfValue;
	}
	
std::vector<int> transWords(const std::vector<std::string>& vWord, std::map<int,std::string> & sOutput) const{
		std::vector<int> vWordIdx;
		std::map<std::string,int>::const_iterator iter;
		
		for(size_t i=0; i<vWord.size(); i++){
			iter = m_word_idx.find(vWord[i]);
			if(iter != m_word_idx.end()){
				vWordIdx.push_back(iter->second);
				
				sOutput.insert(std::make_pair(iter->second,vWord[i]));
			}
		}
		
		return vWordIdx;
	}

	//test, output word number
	void HandleOneElem(std::ofstream* pOfs, int lenght, const std::pair<std::string,int>& elem, int& iNumCount)const
	{
		if (NULL == pOfs) { return; }

		iNumCount++;
		
		std::map<int,int>::const_iterator retIter = m_word_frequency.find(elem.second);
		if (retIter == m_word_frequency.end()) { return; }
		
		*pOfs << elem.first << "\t" << retIter->second << "; ";
		if (5 == iNumCount)
		{
			*pOfs << "\n"; 
			iNumCount = 0;
		}
	}
	
	void TestOutputWordNumber()const
	{
		std::ofstream ofs("TestWordNumber.txt");
		if (!ofs) { return; }

		robosay::base::CSortManager<std::pair<std::string,int> > sortManager;
		std::map<std::string,int>::const_iterator iter = m_word_idx.begin();
		while (iter != m_word_idx.end())
		{
			sortManager.Add(iter->first.length(),*iter);
			++iter;
		}

		sortManager.Sort(false);
		int iNumCount = 0;
		sortManager.ForEach(bind(&TFIDF::HandleOneElem,this,&ofs,_1,_2,iNumCount));
	}

	///////////////////////////////////////////////////////////////////////////////
	void InitTfIdfRemend()
	{
		m_tfIdfRemend.ForEachWord(bind(&TFIDF::SearchWordIndex, this, _1,_2));
	}

	void SearchWordIndex(const std::string& strWord, WP_TYPE wordProperty)
	{
		std::map<std::string,int>::iterator iter = m_word_idx.find(strWord);
		
		if (iter == m_word_idx.end()) { return; }

		m_tfIdfRemend.AddWordIndex(iter->second, wordProperty);
	}
	
public:
	std::map<std::string,int> m_word_idx; // 单词和单词序号对应的词典
	std::map<int,int> m_word_frequency;   // 单词序号和所出现在文档的数目的词典
	int m_questionNum;                      // 问题库中question的数目

	CTfIdfRemend m_tfIdfRemend;
};

}//namespace general
}//namespace robosay

#endif //__ROBOSAY_GENERAL__TFIDF_H__
