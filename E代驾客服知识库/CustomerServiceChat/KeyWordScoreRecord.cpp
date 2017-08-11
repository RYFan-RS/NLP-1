#include "CustomerServiceChat/KeyWordScoreRecord.h"
#include "Common/lib4/File.hpp"
#include "Common/lib4/Utility.hpp"
#include <algorithm>
#include <boost/bind/bind.hpp>
#include <fstream>

namespace robosay{
namespace base{

CKeyWordScoreRecord::CKeyWordScoreRecord()
	:m_strCurScoreFileName("score.txt"),
	 m_strOldScoreFieName("score_old.txt"),
	 m_strCmpScoreFileName("scoreCmp.txt")
{
}

bool CKeyWordScoreRecord::Initialize()
{
	ReadScoreFile();

	RenameScoreFile();
	
	return true;
}

void CKeyWordScoreRecord::AddCurQustionScore(const std::string& strQuestion, int iScore)
{
	m_questionScores[strQuestion].first = iScore;
}

void CKeyWordScoreRecord::AddBefQustionScore(const std::string& strQuestion, int iScore)
{
	m_questionScores[strQuestion].second = iScore;
}

void CKeyWordScoreRecord::ReadScoreFile()
{
	acl::File readFile;
   	if (!readFile.read(m_strCurScoreFileName)) { return; }

	std::for_each(readFile.begin(), readFile.end(), bind(&CKeyWordScoreRecord::HandleOneLine,this,_1));
}

void CKeyWordScoreRecord::HandleOneLine(const acl::String& strOneLine)
{
	//m_strCurScoreFileName 2 col, first question; second score
	if (strOneLine.empty()) { return; }
	std::vector<std::string> vecRet = strOneLine.separate('\t');
	if (vecRet.size() != 2) { return; }
	if (!acl::Utility::isNumber(vecRet[1])) { return; }

	int iScore = atoi(vecRet[1].c_str());

	AddBefQustionScore(vecRet[0],iScore);
}


void CKeyWordScoreRecord::RenameScoreFile()
{
	std::stringstream ssRenameCmd;

	ssRenameCmd << "rm " << m_strOldScoreFieName;
	system(ssRenameCmd.str().c_str());
	
	ssRenameCmd.str("");	
	ssRenameCmd << "mv "<< m_strCurScoreFileName << " " << m_strOldScoreFieName;
	system(ssRenameCmd.str().c_str());
}

void CKeyWordScoreRecord::RecordRetToFile()
{
	//std::map<std::string, ScoreType> m_questionScores; 

	if (m_questionScores.size() == 0) { return; }

	std::ofstream ofsWriteScoreCmp(m_strCmpScoreFileName.c_str());
	if (!ofsWriteScoreCmp) { return; }
	ofsWriteScoreCmp << "问题	当前得分	之前得分	与之前比较\n";
	
	std::map<std::string, ScoreType>::iterator iterSrc = m_questionScores.begin();

	std::multimap<int, std::string> tmpScoreQustions;
	
	while (iterSrc != m_questionScores.end())
	{
		//sort by current score
		(void)tmpScoreQustions.insert(std::make_pair(iterSrc->second.first, iterSrc->first));
		//write compare result to scoreCmp.txt
		ofsWriteScoreCmp << GetScoreCmpRet(iterSrc->first, iterSrc->second) << "\n";
		++iterSrc;
	}

	ofsWriteScoreCmp.close();
	///////////////////////////////////////////////////////////////////////////////////////
	//write cur score record file 
	//begin
	std::ofstream ofsWriteCurScord(m_strCurScoreFileName.c_str());
	if (!ofsWriteCurScord) { return; }

	ofsWriteCurScord << "问题	得分\n";
	//std::multimap<int, std::string> tmpScoreQustions;
	std::multimap<int, std::string>::iterator iterCurScore = tmpScoreQustions.begin();
	while (iterCurScore != tmpScoreQustions.end())
	{
		std::stringstream strRet;
		strRet << iterCurScore->second << "\t" << iterCurScore->first << "\n";
		ofsWriteCurScord << strRet.str();
		++iterCurScore;
	}	
}

std::string CKeyWordScoreRecord::GetScoreCmpRet(const std::string strQuestion, const ScoreType& scores)
{
	std::stringstream strRet;

	std::string strCmpRet("不变");
	if ((scores.first - scores.second) > 9)
	{
		strCmpRet = "提升";
	}
	if ((scores.second - scores.first) > 9)
	{
		strCmpRet = "下降";
	}
	
	strRet << strQuestion << "\t" << scores.first << "\t" << scores.second << "\t" << strCmpRet;

	return strRet.str();
}

}
}

