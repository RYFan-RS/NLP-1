#include "Sentence.h"
#include "Common/lib4/Definations.hpp"
#include <math.h>

namespace robosay{
namespace general{

#define min(x,y) (((x)<(y))?(x):(y))

void Sentence::setWords(const std::set<std::string>& seWord){
	m_seWord = seWord;
}

void Sentence::setWords(const std::vector<std::string>& vWord){
	for(size_t i=0; i<vWord.size(); i++){
		if(vWord.at(i).empty()) continue;
		m_vWord.push_back(vWord[i]);
		m_seWord.insert(vWord.at(i));
	}
}
/*
void Sentence::setTfidfValue(const std::vector<TfidfValue>& vTfidfValue){
	m_vTfidfValue = vTfidfValue;
}
*/
void  Sentence::addOneSegment(std::vector<int>& segmentIds){
	m_allSegmentMethods.push_back(segmentIds);
}
void  Sentence::addTfidfValue(const std::vector<TfidfValue>& vTfidfValue){
	m_vTfidfValue.push_back(vTfidfValue);
}

void Sentence::setPinyin(const std::vector<std::string>& vWordPinyin){
	m_vPinyin = vWordPinyin;
}

const std::set<std::string>& Sentence::getWords() const{
	return m_seWord;
}

void Sentence::setSentence(const std::string& sSentence, const std::string& sID){
	m_sSentence = sSentence;
	m_sID = sID;
}

const std::string& Sentence::getSentence() const{
	return m_sSentence;
}

std::string Sentence::getID() const{
	return m_sID;
}

/*
int Sentence::match(const Sentence& sentence, const std::set<std::string>& seExtWord, const wn::WordNetwork& wordNet, const aisms::mxpsr::Parser &mixParse) const{
	std::set<std::string> seMatched;
	std::set<std::string>::const_iterator iter;
	int nScore = 0;

	set_intersection(m_seWord.begin(),m_seWord.end(),seExtWord.begin(),seExtWord.end(),std::inserter(seMatched,seMatched.begin()));
	if(seMatched.empty()){
#ifdef _BASIC_SEARCH_DEBUG
//		ssDebug<<"Score="<<nScore<<" "<<acl::debug::toStr(m_seWord)<<"^"<<acl::debug::toStr(seExtWord)<<"="<<acl::debug::toStr(seMatched);
//		acl::debug::MESSAGE(DSTREAM,ssDebug,acl::debug::LEVEL0);
#endif
       return nScore;
	}

	nScore = caculateScore(seMatched, sentence.getWeight(), seExtWord, mixParse);
#ifdef _BASIC_SEARCH_DEBUG
	ssDebug<<"Score="<<nScore<<" "<<acl::debug::toStr(m_seWord)<<"^"<<acl::debug::toStr(seExtWord)<<"="<<acl::debug::toStr(seMatched);
	acl::debug::MESSAGE(DSTREAM,ssDebug,acl::debug::LEVEL0);
#endif

	return nScore;
}

int Sentence::caculateScore(const std::set<std::string> &stdMatched, int nInputWeight, const std::set<std::string> &stdInput, const aisms::mxpsr::Parser &mixParse) const
{
	//PRINT("GetWeightTimes:"<<stdMatched.size()<<"+"<<stdInput.size()<<"+"<<m_seWord.size()<<"=" <<stdMatched.size()+stdInput.size()+m_seWord.size())
    int nScore = calcWeight(stdMatched, mixParse) * 200 / (m_nWeight + nInputWeight);
	//PRINT(nScore<<" "<<acl::debug::toStr(m_seWord)<<"^"<<acl::debug::toStr(stdInput)<<"="<<acl::debug::toStr(stdMatched))

    return nScore;
}

inline int Sentence::calcWeight(const std::set<std::string> &vWords, const aisms::mxpsr::Parser &mixParse) const
{
	int result = 0;
	for(std::set<std::string>::iterator iter = vWords.begin(); iter != vWords.end(); iter++)
    {
        result += getWeight(mixParse.getWordProperty(*iter));
	}
	return result;
}

void Sentence::calcWeight(const aisms::mxpsr::Parser &mixParse){
	m_nWeight = calcWeight(m_seWord,mixParse);
}


int Sentence::caculateScore(const std::set<std::string> &stdMatched, const std::set<std::string> &stdInput, const aisms::mxpsr::Parser &mixParse) const
{
	int nResultWeight = 0;
	int nNoFindWeight = 0;
	std::set<std::string>::const_iterator iter;
	std::set<std::string>::iterator itQuestion;

    for(itQuestion=m_seWord.begin(); itQuestion != m_seWord.end(); itQuestion++){
		iter = stdInput.find(*itQuestion);

		if(iter == stdInput.end()){
			//记录没有找到的
			nNoFindWeight -= getWeight(mixParse.getWordProperty(*itQuestion));
		}else{
			//找到了此词，判断此词的权重 循环得到句子权重
			nResultWeight += getWeight(mixParse.getWordProperty(*itQuestion));
		}

	}
	return nResultWeight - abs(nNoFindWeight);

}
*/

float Sentence::matchByTfidf(const std::vector<TfidfValue>& tfidfValues1, const std::vector<TfidfValue>& tfidfValues2)
{
	float fScore;
	std::map<int,float> mTfidf;
	std::map<int,float>::const_iterator iter;
	std::set<TfidfValue> setInTfidf;
	bool flag = false;
	float numerator = 0.f;                    // consine 相似度分子

	for(size_t i=0; i< tfidfValues1.size(); i++){
		mTfidf.insert(std::make_pair(tfidfValues1[i].m_nID,tfidfValues1[i].m_nValue));
	}

	for(size_t i=0; i< tfidfValues2.size(); i++){   //问题中每个分词的Tfidf值
		iter = mTfidf.find(tfidfValues2[i].m_nID);
		if(iter != mTfidf.end()){
			numerator += tfidfValues2[i].m_nValue * iter->second; //关键词中每个分词的Tfidf值和问题中的相乘
			flag = true;
		}
	}

	if(!flag){
		return 0.f;
	}

	float squareA = 0.f, sqrtA;
	float squareB = 0.f, sqrtB;
	for(size_t i=0; i<tfidfValues2.size(); i++){
		squareA += tfidfValues2[i].m_nValue * tfidfValues2[i].m_nValue;
	}
	for(size_t i=0; i<tfidfValues1.size(); i++){
		squareB += tfidfValues1[i].m_nValue * tfidfValues1[i].m_nValue;
	}
	sqrtA = sqrt(squareA);
	sqrtB = sqrt(squareB);
	float denominator = sqrtA * sqrtB;  // consine 相似度计算的分母
	
	if (fabsf(denominator - 0) < 0.000001){
		return 0.f;
	}

	return numerator / denominator;    // 欧氏距离的 consine 相似度
}

float Sentence::matchByTfidf(const std::vector<std::vector<TfidfValue> >&vInTfidf, const CTfIdfRemend& tfIdfRemend){
	float fMaxScore = 0.0;

	for(size_t i=0; i<vInTfidf.size(); i++){
		for(size_t j=0; j<m_vTfidfValue.size(); j++){	
			float fScore = matchByTfidf(vInTfidf[i],m_vTfidfValue[j]);
			if(fScore > fMaxScore){
				fMaxScore = fScore;
			}
		}
	}	

	return fMaxScore;
}
#if 0
float Sentence::matchByTfidf(const std::vector<TfidfValue> &vInTfidf,const CTfIdfRemend& tfIdfRemend){
	#ifndef IS_USE_TFIDF_REMEND
	return matchByTfidf(vInTfidf,m_vTfidfValue);
	#else
	std::vector<TfidfValue> tfidfValues1(vInTfidf);
	std::vector<TfidfValue> tfidfValues2(m_vTfidfValue);
	tfIdfRemend.RemandNeedToMatchTfIdf(tfidfValues1, tfidfValues2);
	return matchByTfidf(tfidfValues1,tfidfValues2);
	#endif
/*
	float fScore;
	std::map<int,float> mTfidf;
	std::map<int,float>::const_iterator iter;
	std::set<TfidfValue> setInTfidf;
	float numerator = 0.f;					  // consine 相似度分子

	for(size_t i=0; i< vInTfidf.size(); i++){
		mTfidf.insert(std::make_pair(vInTfidf[i].m_nID,vInTfidf[i].m_nValue));
	}

	for(size_t i=0; i< m_vTfidfValue.size(); i++){	 //问题中每个分词的Tfidf值
		iter = mTfidf.find(m_vTfidfValue[i].m_nID);
		if(iter != mTfidf.end()){
			numerator += m_vTfidfValue[i].m_nValue * iter->second; //关键词中每个分词的Tfidf值和问题中的相乘
		}
	}

	float squareA = 0.f, sqrtA;
	float squareB = 0.f, sqrtB;
	for(size_t i=0; i<m_vTfidfValue.size(); i++){
		squareA += m_vTfidfValue[i].m_nValue * m_vTfidfValue[i].m_nValue;
	}
	
	for(size_t i=0; i<vInTfidf.size(); i++){
		squareB += vInTfidf[i].m_nValue * vInTfidf[i].m_nValue;
	}
	
	sqrtA = sqrt(squareA);
	sqrtB = sqrt(squareB);
	float denominator = sqrtA * sqrtB;	// consine 相似度计算的分母

	if (fabsf(denominator - 0) < 0.000001){
		return 0.f;
	}

	float consineSim = numerator / denominator;    // 欧氏距离的 consine 相似度
	fScore = consineSim;

	return fScore;*/
}

float Sentence::matchByTfidf(const std::vector<int>& vWordIdx, TFIDF* pTFIDF){
	std::vector<TfidfValue> vInTfidf;
	vInTfidf = pTFIDF->caculate(vWordIdx); //关键词中每个分词的Tfidf值

	#ifndef IS_USE_TFIDF_REMEND
	return matchByTfidf(vInTfidf,m_vTfidfValue);	
	#else
	std::vector<TfidfValue> tfidfValues1(vInTfidf);
	std::vector<TfidfValue> tfidfValues2(m_vTfidfValue);
	pTFIDF->m_tfIdfRemend.RemandNeedToMatchTfIdf(tfidfValues1, tfidfValues2);
	return matchByTfidf(tfidfValues1,tfidfValues2);	
	#endif

/*	
	float fScore;
	std::map<int,float> mTfidf;
	std::map<int,float>::const_iterator iter;
	std::set<TfidfValue> setInTfidf;
	float numerator = 0.f;                    // consine 相似度分子

	for(size_t i=0; i< vInTfidf.size(); i++){
		mTfidf.insert(std::make_pair(vInTfidf[i].m_nID,vInTfidf[i].m_nValue));
	}

	for(size_t i=0; i< m_vTfidfValue.size(); i++){   //问题中每个分词的Tfidf值
		iter = mTfidf.find(m_vTfidfValue[i].m_nID);
		if(iter != mTfidf.end()){
			numerator += m_vTfidfValue[i].m_nValue * iter->second; //关键词中每个分词的Tfidf值和问题中的相乘
		}
	}

	float squareA = 0.f, sqrtA;
	float squareB = 0.f, sqrtB;
	for(size_t i=0; i<m_vTfidfValue.size(); i++){
		squareA += m_vTfidfValue[i].m_nValue * m_vTfidfValue[i].m_nValue;
	}
	for(size_t i=0; i<vInTfidf.size(); i++){
		squareB += vInTfidf[i].m_nValue * vInTfidf[i].m_nValue;
	}
	sqrtA = sqrt(squareA);
	sqrtB = sqrt(squareB);
	float denominator = sqrtA * sqrtB;  // consine 相似度计算的分母
	
	if (fabsf(denominator - 0) < 0.000001){
		return 0.f;
	}

	float consineSim = numerator / denominator;    // 欧氏距离的 consine 相似度
	fScore = consineSim;

	return fScore;*/
}
#endif

// 计算Levenshtein Distance
int LevenshteinDistance(const std::vector<std::string>& vIn, const std::vector<std::string>& vQuestion){
	int n = vIn.size();
	int m = vQuestion.size();
	//Construct a matrix
	typedef std::vector<std::vector<int> > Tmatrix;
    Tmatrix matrix(n+1);
    for(int i=0; i<=n; i++){
		matrix[i].resize(m+1);
	}

    for(int i=1;i<=n;i++) matrix[i][0]=i;
    for(int i=1;i<=m;i++) matrix[0][i]=i;
    for(int i=1;i<=n;i++){
        const std::string &si=vIn[i-1];
        for(int j=1;j<=m;j++){
            const std::string &dj=vQuestion[j-1];
            int cost;
            if(si==dj){
                cost=0;
            }else{
                cost=1;
            }
            const int above=matrix[i-1][j]+1;
            const int left=matrix[i][j-1]+1;
            const int diag=matrix[i-1][j-1]+cost;
            matrix[i][j]=min(above,min(left,diag));
        }
     }
     int dist = matrix[n][m];

	return dist;

}

float Sentence::matchByFuzzyPinyin(const std::vector<std::string>& vWordPinyin){
	float fScore;

	// 拼音模糊匹配
	int n = vWordPinyin.size();
	int m = m_vPinyin.size();
	//Construct a matrix
	typedef std::vector<std::vector<int> > Tmatrix;
    Tmatrix matrix(n+1);
    for(int i=0; i<=n; i++){
		matrix[i].resize(m+1);
	}

    for(int i=1;i<=n;i++) matrix[i][0]=i;
    for(int i=1;i<=m;i++) matrix[0][i]=i;
    for(int i=1;i<=n;i++){
        const std::string &si=vWordPinyin[i-1];
        for(int j=1;j<=m;j++){
            const std::string &dj=m_vPinyin[j-1];
            int cost;
			acl::String ssi(si);
			std::vector<std::string> vSi = ssi.toCharactor();
			acl::String ddj(dj);
			std::vector<std::string> vDj = ddj.toCharactor();
			int dist = LevenshteinDistance(vSi, vDj);                // dist 是Levenshtein距离
            if(dist <= 2 && (si.length()>2 && dj.length()>2)){     // 如果 dist<2，则我们认为两个汉字的拼音相同
                cost=0;
            }else{
                cost=1;
            }
            const int above=matrix[i-1][j]+1;
            const int left=matrix[i][j-1]+1;
            const int diag=matrix[i-1][j-1]+cost;
            matrix[i][j]=min(above,min(left,diag));
        }
     }
     int dist = matrix[n][m];
	 fScore = float(n+m-dist) / (n+m);

	return fScore;
}

// fuzzy Pinyin matching
float Sentence::matchByPinyin(const std::vector<std::string>& vWordPinyin){
	// 拼音模糊匹配
	int n = vWordPinyin.size();
	int m = m_vPinyin.size();
	//Construct a matrix
	typedef std::vector<std::vector<int> > Tmatrix;
    Tmatrix matrix(n+1);
    for(int i=0; i<=n; i++){
		matrix[i].resize(m+1);
	}

    for(int i=1;i<=n;i++) matrix[i][0]=i;
    for(int i=1;i<=m;i++) matrix[0][i]=i;
    for(int i=1;i<=n;i++){
        const std::string &si=vWordPinyin[i-1];
        for(int j=1;j<=m;j++){
            const std::string &dj=m_vPinyin[j-1];
            int cost;
            if(si==dj){
                cost=0;
            }else{
                cost=1;
            }
            const int above=matrix[i-1][j]+1;
            const int left=matrix[i][j-1]+1;
            const int diag=matrix[i-1][j-1]+cost;
            matrix[i][j]=min(above,min(left,diag));
        }
     }
     int dist = matrix[n][m];
	 return float(n+m-dist) / (n+m);

}


float Sentence::matchByLD(const std::string& sIn){
	float fScore;
	acl::String s(sIn);
	std::vector<std::string> vChar = s.toCharactor();
	acl::String sent(m_sSentence);
	std::vector<std::string> vSentChar = sent.toCharactor();

	// 通过DP算法计算改变经过的路径，而得到相差的距离
	int n = vChar.size();
	int m = vSentChar.size();
	//Construct a matrix
	typedef std::vector<std::vector<int> > Tmatrix;
    Tmatrix matrix(n+1);
    for(int i=0; i<=n; i++){
		matrix[i].resize(m+1);
	}

    for(int i=1;i<=n;i++) matrix[i][0]=i;
    for(int i=1;i<=m;i++) matrix[0][i]=i;
    for(int i=1;i<=n;i++){
        const std::string &si=vChar[i-1];
        for(int j=1;j<=m;j++){
            const std::string &dj=vSentChar[j-1];
            int cost;
            if(si==dj){
                cost=0;
            }else{
                cost=1;
            }
            const int above=matrix[i-1][j]+1;
            const int left=matrix[i][j-1]+1;
            const int diag=matrix[i-1][j-1]+cost;
            matrix[i][j]=min(above,min(left,diag));
        }
     }
     int dist = matrix[n][m];
	 fScore = float(n+m-dist) / (n+m);

	return fScore;
}

}//namespace general
}//namespace robosay
