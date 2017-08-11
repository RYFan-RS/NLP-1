#ifndef __ROBOSAY__INVERT__INDEX_H__
#define __ROBOSAY__INVERT__INDEX_H__
#include <map>
#include <math.h>
#include <fstream>
#include <set>
#include "Common/SortManager.h"

typedef std::map<int, std::map<int,float> >::const_iterator CONST_POINTER_INVERT_INDEX;

namespace robosay{
namespace general{




class InvertIndex
{
	public:
		void handleDocument(const std::vector<std::string> & words,const int & docId){
			
			for(int i=0;i<words.size();i++){
				std::map<std::string,std::map<int,float> >::iterator itr = m_index.find(words[i]);
				if(itr==m_index.end()){
					std::map<int,float> mapValue;
					mapValue[docId] = 1.0;
					//单词,文档，文档中出现的次数
					m_index.insert(make_pair(words[i],mapValue));
				}else{
					std::map<int,float>&  refMapValue = itr->second;
					std::map<int,float>::iterator it = refMapValue.find(docId);
					if(it==refMapValue.end()){
						refMapValue.insert(std::pair<int,float>(docId,1.0f));
					}else{
						it->second=it->second+1;
					}
				}
			}

			std::map<int,int>::iterator docItr = m_doc.find(docId);
			if(docItr==m_doc.end()){
				//文档号:本文章出现多少词汇
				m_doc.insert(std::pair<int,int>(docId,words.size()));
			}else{
				docItr->second = docItr->second+words.size();
			}
		}
		

		void initTfIdf(){
			std::map<std::string,std::map<int,float> >::iterator itr = m_index.begin();		
			int  wordId=0;
			
			int totalDocNumber = m_doc.size();
			
			for(;itr != m_index.end();itr++){
				//这个单词的IDF
				float idf = log10(float(totalDocNumber) / (itr->second.size()+1));
				std::map<int,float>::iterator it = itr->second.begin();
				/*
				for(;it!=itr->second.end();it++){
					//m_doc是文章ID:单词总数
					std::map<int,int>::iterator docItr = m_doc.find(it->first); 
					int docWords = 100;
					if(docItr!=m_doc.end()){
						docWords = docItr->second+1;
					}
					//it->second = it->second/docWords *idf;
					//it->second = it->second * idf;
				}
				*/
				itr->second.insert(std::pair<int,float>(-1,++wordId));
				itr->second.insert(std::pair<int,float>(-2,idf));
			}
		}

		std::vector<std::pair<int,std::string> > match(const std::vector<std::string>& words,const std::set<int>& setMatched,
															std::map<int,std::string>& highWordMatchQA){
			std::set<std::string> setWords;
			std::map<int,std::pair<std::string,float> > wordsMap;
			int wordId;
			float wordIdf;
			for(int i=0;i<words.size();i++){
				setWords.insert(words[i]);
			}

			std::map<int, std::map<int,float> > result;/*int docid  int float  wordId  idf*/
			std::set<std::string>::const_iterator itr = setWords.begin();
			for(;itr!=setWords.end();itr++){
				if(result.size()>5000)
					break;
				const std::map<int,float>* pMap = search(*itr);
				if(pMap!=NULL){
					std::map<int,float>::const_iterator itWordId = pMap->find(-1);
					if(itWordId==pMap->end())
						continue;
					wordId = itWordId->second;

					itWordId = pMap->find(-2);
					if(itWordId==pMap->end())
						continue;

					std::pair<std::string,float> keyValue(*itr,itWordId->second);
					wordsMap[wordId]=keyValue;

					merge(result,pMap,wordId);
					
				}
			}


			robosay::base::CSortManager<CONST_POINTER_INVERT_INDEX,float>  sortManager;

			CONST_POINTER_INVERT_INDEX pInvertIndex = result.begin();
			for(;pInvertIndex!=result.end();pInvertIndex++){

				if(setMatched.find(pInvertIndex->first)!=setMatched.end()){
					std::string highLight;

					std::map<int,float>::const_iterator it = pInvertIndex->second.begin();
					for(;it!=pInvertIndex->second.end();it++){
						highLight+=wordsMap[it->first].first;
						highLight+=";";
					}					
					highWordMatchQA[pInvertIndex->first] = highLight;
					continue;
				}	
				
				float score = matchByTfidf(pInvertIndex, wordsMap);
				if(score>0.01){
					sortManager.Add(score,pInvertIndex);	
				}
			}
			
			sortManager.Sort();
			const std::vector< robosay::base::CSortManager<CONST_POINTER_INVERT_INDEX,float>::NodeElem >& elems = sortManager.GetAllElems();	
			
			std::vector<robosay::base::CSortManager<CONST_POINTER_INVERT_INDEX,float>::NodeElem>::const_iterator iter = elems.begin();

			std::vector<std::pair<int,std::string> > finalResult;

			for(int i=0;iter!=elems.end();iter++,i++){
				const CONST_POINTER_INVERT_INDEX& itr1 = iter->GetElem();
				std::string highLight;


				std::map<int,float>::const_iterator it = itr1->second.begin();
				for(;it!=itr1->second.end();it++){
					highLight+=wordsMap[it->first].first;
					highLight+=";";
				}
				std::pair<int,std::string> scorePair(itr1->first,highLight);
				finalResult.push_back(scorePair);
			}
			return finalResult;
		}

		float matchByTfidf(CONST_POINTER_INVERT_INDEX& source, std::map<int, std::pair<std::string,float> >& sentence){
			float fScore = 0.f, distance = 0.000001;
			int number=0;
			int sameWordNumber=0;
			if(source->second.size()==0)
				return fScore;

			std::map<int,float>::const_iterator it = source->second.begin();
			for(;it!=source->second.end();it++){
				
				fScore+= sentence[it->first].second;
				number++;
				sameWordNumber+=it->second;
				sameWordNumber-=1;
				assert(sameWordNumber>=0);
			}

			fScore = fScore*number*10+sameWordNumber*distance;
			return fScore;
		}
		
		void merge(std::map<int, std::map<int,float> >& result,const std::map<int,float>* psrc, const int& wordId){
			if(psrc == NULL)
				return ;
		
			std::map<int,float>::const_iterator it = psrc->begin();
			for(;it!=psrc->end();it++){
				if(it->first == -1 || it->first == -2){
					continue;
				}
				
				std::map<int, std::map<int,float> >::iterator itr = result.find(it->first);
				if(itr==result.end()){
					std::map<int,float> wordMap;
					wordMap[wordId] = it->second;
					result[it->first] = wordMap;
				}else{
					itr->second[wordId] = it->second;
				}
			}
		}
		
		const std::map<int,float>* search(const std::string& word)
		{
			std::map<std::string,std::map<int,float> >::iterator found;
			found = m_index.find(word);
			if(found!=m_index.end())
				return &(m_index[word]);
			else
				return NULL;
		}

	private:
	    /*std::string 单词
	     int,double  int>=0  文档id为int值中该单词出现的次数
	     int = -2   表示该单词的idf值
	     int = -1   表示该单词的id值，具有唯一性*/
		std::map<std::string,std::map<int,float> > m_index;
		std::map<int,int>                          m_doc;
		/*int int  文档id, 文档单词总数*/
};

}//namespace general
}//namespace robosay

#endif //__ROBOSAY__INVERT__INDEX_H__
