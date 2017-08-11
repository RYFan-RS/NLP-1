#ifndef _KEY_WORD_SCORE_RECORD_FZ_H_
#define _KEY_WORD_SCORE_RECORD_FZ_H_

#include "Common/lib4/String.hpp"
#include <map>
#include <string>

namespace robosay{
namespace base{

class CKeyWordScoreRecord
{
	typedef std::pair<int,int> ScoreType; //first : current score; second : before score
	
public:
	CKeyWordScoreRecord();

public:
	bool Initialize();
	void RecordRetToFile();
	
public:
	void AddCurQustionScore(const std::string& strQuestion, int iScore);
	void AddBefQustionScore(const std::string& strQuestion, int iScore);

public:
	//read and write operator

	void HandleOneLine(const acl::String& strOneLine);
private:
	void ReadScoreFile();
	void RenameScoreFile();
	std::string GetScoreCmpRet(const std::string strQuestion, const ScoreType& scores);
	
private:
	std::string m_strCurScoreFileName; 	//current record score file name
	std::string m_strOldScoreFieName;		//before record score file name
	std::string m_strCmpScoreFileName;	//compare score record file name
	
	std::map<std::string, ScoreType> m_questionScores; 

};

}
}

#endif
