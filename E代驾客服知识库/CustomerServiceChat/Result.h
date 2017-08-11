#ifndef __ROBOSAY__BASE__RESULT__H__
#define __ROBOSAY__BASE__RESULT__H__
#include <vector>
#include "Common/lib4/CommonDef.h"
#include "Common/json/Json.hpp"
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>
#include "Result_info.hpp"
#include "TranslatorEx.hpp"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

namespace robosay{
class InternalServiceIf;
}

namespace robosay{
namespace base{

class ExternalAPI_info;

class Path;
class Result{
public:
	static          int max_result_number;
	Path* 			m_pPath;
	std::set<int>	m_sMatchTitleWords;
	std::string		m_sMatchDetailWords;	
	
private:
	int             m_nScore;
	Result_info		m_info;
	std::string		m_sAnalyze;


public:
	Result(const std::string& sType, Path* pPath = NULL);

	virtual ~Result();

	virtual bool isValid() const;

	virtual std::string toStr() const;

	virtual bool parse(const json::Object& oObj);

	virtual bool calcScore(void* pParam);

	virtual std::string getAnswer() const;
	void setAnswer(const std::string& sAnswer, const std::string& way, const std::string&id);
	void setAnswerInfo(const Answer_info& answer);

	StringStringMap getSmtcWords() const;
	StringStringMap& getSmtcWordsRef();
	void setSmtcWords(const StringStringMap& mWords, const std::string& sIDs);
	void insertSmtcWord(const std::string& l, const std::string& r, const std::string& id);

	void setTarget(const std::string& sTarget, const std::string& sID);
	std::string getTarget()const;

	std::string getInput() const;
	void setInput(const std::string& sIn);

	int getScore() const;
	void setScore(const int nScore);
	void setScore(const float fScore);

	std::string getAnalyze() const;
	void setAnalyze(const std::string& sAnalyze);

    std::string getKBType() const;
	void setKBType(const std::string& sType);

    //Result_info getInfo()const{return m_info;}
    Answer_info getAnswerInfo()const{ return m_info.m_answer;}
    Question_info getQuestionInfo()const{ return m_info.m_question;}
    Relate_info getRelateInfo()const { return m_info.m_relate;}
    void setRelateQuestions(const StringVector& vQuestions);

	void setQuestionType(const std::string& type) { m_info.m_question.set_type(type);}

private:
	Result(const Result& rst);

	Result& operator=(const Result& rst);
};

typedef struct tag_TypeWeightMatrix{
	std::string sType;
	int			nWeight;
}TypeWeightMatrix;
static TypeWeightMatrix twMatrix[] = {
	{"Line", 4},
	{"Tree", 3},
	{"FAQ", 2},
	{"Table", 1},
};

static int twMatrixCount = sizeof(twMatrix) / sizeof(twMatrix[0]);
static int CalcWeightWithType(const std::string& sType){
	for(int i = 0; i < twMatrixCount; ++i){
		if(sType == twMatrix[i].sType){
			return twMatrix[i].nWeight;
		}
	}
	return 0;
}

static bool RstCompare(const Result* lhs, const Result* rhs){
	int nLhsWeight = CalcWeightWithType(lhs->getKBType());
	int nRhsWeight = CalcWeightWithType(rhs->getKBType());
	if(nLhsWeight > nRhsWeight){
		return true;
	}else if(nLhsWeight == nRhsWeight){
		return lhs->getScore() > rhs->getScore();
	}else{
		return false;
	}
}

static bool RstSemanticCompare( Result* lhs,  Result* rhs){
	if(lhs->getSmtcWordsRef().size() > rhs->getSmtcWordsRef().size()){
		return true;

	}else{
		return false;
	}
}


class VResult : public std::vector<robosay::base::Result*>{
public:
	std::string sMessage;

public:
	VResult();

	virtual ~VResult();

	void sort();

	std::string toStr() const;

	bool fromStr(const std::string& sStr);

	void clear();
	
	void ClearPathInfo();

	void moveFrom(VResult& rhs);

	void reduceSize(const size_t uSize);

	int getKBIdx() const;

    bool getResultInfo(std::string& sContext, Result_info& resultInfo,const std::map<int,std::string>& mapIdToWord) const;

	bool setKBType(const std::string& sType);

	void selectBest();

	bool PathInVResult(robosay::base::Result* path) const;
private:
	std::string getAnswer(robosay::base::ExternalAPI_info* _extAPI) const;

    std::string executeExternalAPI(const std::string& sCmd, robosay::base::ExternalAPI_info* _extAPI) const;
	void selectServiceScenario();

private:
	VResult(const Result& rhs);

	VResult& operator=(const Result& rhs);
};

robosay::base::Result* createResult(const std::string& sType);

}//namespace base
}//namespace robosay

#endif //__ROBOSAY__BASE_CHAT__RESULT__H__
