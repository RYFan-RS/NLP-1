#ifndef __TFIDF_HPP__
#define __TFIDF_HPP__
#include <map>
#include <math.h>
#include <fstream>
#include "lib4/Definations.hpp"
#include "Serialization.hpp"

class TfidfValue{
public:
	int m_nID;
	float m_nValue;
	
public:
	template <class Format, class Refs>
	inline void composite(obj_input<Format, Refs>& stream) {
		stream.simple(m_nID);
		stream.simple(m_nValue);
	}
	
	template <class Format, class Refs>
	inline void composite(obj_output<Format, Refs>& stream) {
		stream.simple(m_nID);
		stream.simple(m_nValue);
	}
};

class Tfidf{
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
		
#ifdef FEATURE_TF_ONE_TIME
		length = word_frequency.size();
#endif
		it = word_frequency.begin();
		for(; it!=word_frequency.end(); it++){
			float tf = float(it->second) / length;
			it1 = m_word_frequency.find(it->first);
			float idf = log10(float(m_questionNum) / (it1->second+1));
			float tfidf = tf * idf;
			TfidfValue value;
			value.m_nID = it->first;
			value.m_nValue = tfidf;
			vTfidfValue.push_back(value);
		}
		
		return vTfidfValue;
	}
	
	std::vector<int> transWords(const std::vector<std::string>& vWord) const{
		std::vector<int> vWordIdx;
		std::map<std::string,int>::const_iterator iter;
		
		for(size_t i=0; i<vWord.size(); i++){
			iter = m_word_idx.find(vWord[i]);
			if(iter != m_word_idx.end()){
				vWordIdx.push_back(iter->second);
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
	
public:
	template <class Format, class Refs>
	inline void composite(obj_input<Format, Refs>& stream) {
		stream.simple(m_questionNum);
		
		int size = 0;
		Format &format = stream.get_format();
		format.input_start_array(size);
		for(size_t i = 0; i < size; i++) {
			std::string first;
			int second;
			stream.simple(first);
			stream.simple(second);
			m_word_idx.insert(std::make_pair(first,second));
		}
		
		size = 0;
		Format &format1 = stream.get_format();
		format1.input_start_array(size);
		for(size_t i = 0; i < size; i++) {
			int first,second;
			stream.simple(first);
			stream.simple(second);
			m_word_frequency.insert(std::make_pair(first,second));
		}
	}
	
	template <class Format, class Refs>
	inline void composite(obj_output<Format, Refs>& stream) {
		stream.simple(m_questionNum);
		
		int size = m_word_idx.size();
		std::map<std::string,int>::iterator i;
		Format &format = stream.get_format();
		format.output_start_array(size);
		for(i = m_word_idx.begin(); i != m_word_idx.end(); ++i) {
			stream.simple(i->first);
			stream.simple(i->second);
		}
		format.output_end_array();
		
		size = m_word_frequency.size();
		std::map<int,int>::iterator j;
		Format &format1 = stream.get_format();
		format1.output_start_array(size);
		for(j = m_word_frequency.begin(); j != m_word_frequency.end(); ++j) {
			stream.simple(j->first);
			stream.simple(j->second);
		}
		format1.output_end_array();
	}
	
public:
	std::map<std::string,int> m_word_idx; // 单词和单词序号对应的词典
	std::map<int,int> m_word_frequency;   // 单词序号和所出现在文档的数目的词典
	int m_questionNum;                    // 问题库中question的数目
};


#endif //__TFIDF_HPP__
