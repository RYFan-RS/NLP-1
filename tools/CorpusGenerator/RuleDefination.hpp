#ifndef __SAE__RULE_DEFINATION__HPP__
#define __SAE__RULE_DEFINATION__HPP__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <stdlib.h>
#include <iostream>
#include <vector>
#include "LineIndexManager.h"
#include "CommonDefination.hpp"
#include "BusinessDefination.hpp"
#include "VariableDefination.hpp"
#include "Utility.h"
#include "lib4/FileName.hpp"
#include "lib4/Algorithm.hpp"
#include "lib4/String.hpp"
#include "CmdArgs.hpp"



namespace sae
{


class RuleDefination
{
public:
	RuleDefination(){
		m_pLineIndexManager = NULL;
	}
	
	void setLineIndexManagerPointer(LineIndexManager* pLineIndexManager){
		m_pLineIndexManager = pLineIndexManager;
	}
	
	bool parse(FILE& file, const std::string& sBussiness, size_t nIdx, const std::vector<FILE>& vFiles, CommonDefination& comDef, BusinessDefination& bizDef, VariableDefination& varDef, ErrorData& errData){
		std::map<std::string,size_t>::const_iterator iterM; 
		std::multimap<std::string,size_t>::const_iterator iterMM; 
		std::stringstream s;
		
		for(int i=0,size = file.m_file.size(); i<size; i++){
			if(file.m_file[i].m_line.empty()) continue;//空行
			
			size_t nPos = file.m_file[i].m_line.find("#");
			if(nPos == 0){ //注释行
				continue;
			}else if(nPos == std::string::npos){
				s<<m_pLineIndexManager->getFileName(file.m_file[i].m_lineNum)+":"<<m_pLineIndexManager->getLineNum(file.m_file[i].m_lineNum);
				m_mRulePair.insert(std::make_pair(s.str(),file.m_file[i].m_line));	
				s.str("");
				if(parseLine(file.m_file[i],sBussiness,comDef,bizDef,varDef,errData) == false){
					errData.errorTrace(__FILE__,__LINE__-1);
					return false;
				}
			}else{
				file.m_file[i].m_line = file.m_file[i].m_line.substr(0,nPos);
				s<<m_pLineIndexManager->getFileName(file.m_file[i].m_lineNum)+":"<<m_pLineIndexManager->getLineNum(file.m_file[i].m_lineNum);
				m_mRulePair.insert(std::make_pair(s.str(),file.m_file[i].m_line));
				s.str("");				
				if(parseLine(file.m_file[i],sBussiness,comDef,bizDef,varDef,errData) == false){
					errData.errorTrace(__FILE__,__LINE__-1);
					return false;
				}
			}
		}
		
		m_vRuleFiles.push_back(file);
		
		return true;
	}
	
	bool parseLine(LINE& line, const std::string& sBussiness, CommonDefination& comDef, BusinessDefination& bizDef, VariableDefination& varDef, ErrorData& errData){
		LINE newLine;
		newLine.m_lineNum = line.m_lineNum;
		
		//检查变量的合法性
		bool bIsVariable = false;
		std::string sVariable;
		for(size_t i=0,length=line.m_line.length(); i<length; i++){
			if(line.m_line[i] == '@'){
				bIsVariable = true;
			}else if(line.m_line[i] == ' '){
				if(bIsVariable == true){
					if(varDef.hasVariable(sVariable) == false){
						errData.m_errMsg = m_pLineIndexManager->dumpLineIndexInfo(line.m_lineNum)+"=>"+line.m_line+"=>内变量["+sVariable+"]没有被定义";
						errData.errorTrace(__FILE__,__LINE__-1);
						return false;
					}
					sVariable = "";
					bIsVariable = false;
				}
			}else{
				if(bIsVariable == true){
					sVariable += line.m_line[i];
				}
			}
		}
		
		//集合进行扩展
		bool bInPair = false;
		std::string sWord;
		for(size_t i=0,length=line.m_line.length(); i<length; i++){
			if(line.m_line[i] == '{'){
				newLine.m_line += line.m_line[i];
				bInPair = true;
			}else if(line.m_line[i] == '}'){
				if(sWord.empty()){
					newLine.m_line += line.m_line[i];
				}else{
					//P("INPUT>"<<sWord<<"#"<<sBussiness)
					bool bBizExpaned = false;
					if(!sBussiness.empty()){ //如果领域定义了，则先使用领域定义
						std::string sBizWords = bizDef.getWordsString(sWord,sBussiness);
						//P("  ->BUSINESS EXPAND>"<<sBizWords)
						if(!sBizWords.empty()){
							newLine.m_line += sBizWords;
							bBizExpaned = true;
						}
					}
					std::string sComWords = comDef.getWordsString(sWord,sBussiness);
					//P("  ->COMMON EXPAND>"<<sComWords)
					if(sComWords.empty()){
						errData.m_errMsg = "通用集合和领域集合中都没有找到含有单词["+sWord+"]的集合";
						errData.errorTrace(__FILE__,__LINE__-1);
						return false;
					}
					if(bBizExpaned){
						newLine.m_line += "/" + sComWords + line.m_line[i];
					}else{
						newLine.m_line += sComWords + line.m_line[i];
					}
					sWord = "";
				}
				bInPair = false;
			}else{
				if(bInPair == true){
					if(line.m_line[i] == '/'){
						if(sWord.empty()){
							newLine.m_line += line.m_line[i];
						}else{
							//P("INPUT>"<<sWord<<"#"<<sBussiness)
							bool bBizExpaned = false;
							if(!sBussiness.empty()){ //如果领域定义了，则先使用领域定义
								std::string sBizWords = bizDef.getWordsString(sWord,sBussiness);
								P("  ->BUSINESS EXPAND>"<<sBizWords)
								if(!sBizWords.empty()){
									newLine.m_line += sBizWords;
									bBizExpaned = true;
								}
							}
							std::string sComWords = comDef.getWordsString(sWord,sBussiness);
							//P("  ->COMMON EXPAND>"<<sComWords)
							if(sComWords.empty()){
								errData.m_errMsg = "通用集合和领域集合中都没有找到含有单词["+sWord+"]的集合";
								errData.errorTrace(__FILE__,__LINE__-1);
								return false;
							}
							if(bBizExpaned){
								newLine.m_line += "/" + sComWords + line.m_line[i];
							}else{
								newLine.m_line += sComWords + line.m_line[i];
							}
							
							sWord = "";
						}
					}else{
						sWord += line.m_line[i];
					}
				}else{
					newLine.m_line += line.m_line[i];
				}
			}
			//P(newLine.m_line)
		}
		
		line = newLine;

		return true;
	}
	
	void generateCorpus(const LineIndexManager& lineMgr){
		std::string sRuleIndex;
		std::string sRule;
		std::map<std::string,std::string>::iterator iter;
		for(size_t i=0; i<m_vRuleFiles.size(); i++){
			for(size_t j=0,size=m_vRuleFiles[i].m_file.size(); j<size; j++){
				std::stringstream ss;
				ss<<lineMgr.getFileName(m_vRuleFiles[i].m_file[j].m_lineNum)<<":"<<lineMgr.getLineNum(m_vRuleFiles[i].m_file[j].m_lineNum);
				sRuleIndex = ss.str();
				ss.str("");
				ss << "#####" <<sRuleIndex<< "#####";
				m_vRuleFiles[i].m_corpus.push_back(ss.str());
				std::vector<std::string> vResult = acl::Algorithm::permutationAndCombination(m_vRuleFiles[i].m_file[j].m_line);

				iter = m_mRulePair.find(sRuleIndex);
				if(iter == m_mRulePair.end()){
					sRule = "error";
				}
				else
				{
					sRule = iter->second;
				}
				for(size_t k=0,size1=vResult.size(); k<size1; k++){
					m_vRuleFiles[i].m_corpus.push_back(vResult[k] + "\t" + m_vRuleFiles[i].m_file[j].m_line+"\t"+sRule);
				}
			}
		}
	}
	
	bool writeToDirectory(const std::string& sOutDirName) const{
		for(size_t i=0; i<m_vRuleFiles.size(); i++){
			acl::FileName sFileName = sOutDirName + m_vRuleFiles[i].m_name;
			std::string sCommand = "mkdir -p " + sFileName.getPath();
			system(sCommand.c_str());
			std::fstream ofs(sFileName.c_str(), std::ios::out);
			for(size_t j=0,size=m_vRuleFiles[i].m_corpus.size(); j<size; j++){
				ofs << m_vRuleFiles[i].m_corpus[j] << std::endl;
			}
			ofs.close();
		}
	}
	
	void dump(const LineIndexManager& lineMgr, std::ostream& ofs = std::cout) const{
		for(size_t i=0; i<m_vRuleFiles.size(); i++){
			for(size_t j=0,size=m_vRuleFiles[i].m_file.size(); j<size; j++){
				ofs << lineMgr.getFileName(m_vRuleFiles[i].m_file[j].m_lineNum) << ":" << lineMgr.getLineNum(m_vRuleFiles[i].m_file[j].m_lineNum) << "\t" << m_vRuleFiles[i].m_file[j].m_line << std::endl;
			}
		}
		
		for(size_t i=0; i<m_vRuleFiles.size(); i++){
			for(size_t j=0,size=m_vRuleFiles[i].m_corpus.size(); j<size; j++){
				ofs << m_vRuleFiles[i].m_corpus[j] << std::endl;
			}
		}
	}
	
	bool input_search(const std::string& input,sae::CmdArgs &cmd,std::ostream& ofs=std::cout){
		std::vector<std::string> vRuleFile;
		std::vector<std::string> vResult;
		acl::String sTemp; 
		bool bStop = false;
        bool bSkip = false;
		size_t pos;
		
		for(int i=0;(i<m_vRuleFiles.size())&&(bStop==false);i++){
			vRuleFile=m_vRuleFiles[i].m_corpus;
			//查找每一行是否包含指定的输入
			for(int i=0;i<vRuleFile.size();i++){
				//记录最近的一行注释
				pos = vRuleFile[i].find("####",0);
				if(std::string::npos!=pos){
                    std::string stemp=cmd.get_BusinessMode();
                    if(!stemp.empty()){
                            if(std::string::npos == vRuleFile[i].find(stemp)){
                                    //此文件不满足要求
                                    break;
                                }
                        }
                
					vResult.clear();
					sTemp = vRuleFile[i];
					sTemp.removeChar('#');
					vResult.push_back(sTemp);
					continue;
				}
				//以\t进行切分本行数据
				pos=vRuleFile[i].find('\t',0);
				if(std::string::npos!=pos){
					if(input==vRuleFile[i].substr(0,pos)){
						vResult.push_back(input);
						vResult.push_back(vRuleFile[i].substr(pos+1,vRuleFile[i].size()));
						std::map<std::string,std::string>::iterator iter=m_mRulePair.find(sTemp);
						std::cout<<"input_search:"+sTemp<<std::endl;
						if(m_mRulePair.end() != iter){
								vResult.push_back(iter->second);
							}
						else {
								vResult.push_back("error");
							}
						bStop = true;
						break;
					}
				}
			}
		}

		//没有找到则直接返回
		if(false==bStop) {
			return false;
		}
		
		ofs << "提取规则结果:";
		for (std::vector<std::string>::iterator iter = vResult.begin(); iter != vResult.end(); ++iter){
			ofs << *iter << "\t";
		}
		return true;
	}

	int Editdistance(const std::vector<std::string>& vIn, const std::vector<std::string>& vQuestion){
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
				matrix[i][j]=get_min(above,get_min(left,diag));
			}
		 }
		 int dist = matrix[n][m];
	
		return dist;
	
	}

	inline int  get_min(int a,int b)
		{
			return ((a>b)?b:a);
		}
	bool isCharactor(char c)
		{
			
			return false;
		}
	std::vector<std::string> parse_string(const acl::String&str1,std::string &slabel)
		{
			
			std::vector<std::string> vChar, v = str1.toCharactor();
			std::map<std::string,int>::iterator iter;
			std::vector<int> vRet;
			std::string sChar;
			
			for(size_t i=0; i<v.size(); i++) {
				if(v.at(i).length() > 1){
					if(!sChar.empty()){
						vChar.push_back(sChar);
						slabel = sChar;
						sChar.erase(sChar.begin(),sChar.end());
					}
					vChar.push_back(v.at(i));
				}else{
					//将非中文字符组装起来
					sChar += v.at(i);
				}
			}
			
		    if(!sChar.empty()){
		        vChar.push_back(sChar);
		    }			

			return vChar;
		}

	std::set<std::string> recommend(const std::vector<std::string>&Output,const std::string&input,std::map<std::string,std::string>&mResult){

	int Number = 0;
	int distance = 0;
	std::string slabel1;
	std::string slabel2;
	std::map<int,std::string> mTemp;
	std::map<int,std::string>::const_iterator iter;
	std::map<std::string,std::string>::const_iterator miter;
	
	std::set<std::string> sResult;

	//1.只显示5个 2.显示的内容按照相似的顺序进行排列		
	for(int i=0;i<Output.size();i++){	
		distance = Editdistance(parse_string(Output[i],slabel1),parse_string(input,slabel2));
		//特殊处理，variable不一样的放后面
		if(distance<=50){
			if(slabel1 != slabel2){
				distance = 1000+i;
			}			
			mTemp.insert(std::make_pair(distance,Output[i]));
		}
	}

	if(mTemp.empty()){
		return sResult;
	}
	//map的key是按从小到大的顺序来排列的
	for(iter = mTemp.begin();iter!=mTemp.end();++iter){
		miter=mResult.find(iter->second);
		if(miter != mResult.end()){
			sResult.insert(miter->second);
			if(sResult.size()==5){
				break;
			}		
		}
	}
	return sResult;
}
	void recommend(const std::string&input,std::ostream& ofs = std::cout)
		{
			std::vector<std::string>RuleFiles;
			std::vector<std::string>Output;
			std::set<std::string>sResult;
			std::map<std::string,std::string> mResult;
			
			std::set<std::string>::iterator siter;			
			size_t pos = 0;
			size_t rpos = 0;

			//获取所有的输出结果
			for(int i=0;i<m_vRuleFiles.size();i++)		
				{	
					RuleFiles=m_vRuleFiles[i].m_corpus;
					for(int j=0;j<RuleFiles.size();j++)
						{
							//去掉注释行内容
							pos = RuleFiles[j].find("####",0);
							if(std::string::npos!=pos)
								{
									continue;
								}
							//以\t进行切分本行数据:兼容这种情况:@moviename原声音乐	@moviename{主题曲/片头曲/片尾曲/插曲/主题歌/原声音乐}	@moviename{主题曲}
							rpos=RuleFiles[j].rfind('\t');
							pos =RuleFiles[j].find('\t');
							if(std::string::npos!=pos)
								{									
									Output.push_back(RuleFiles[j].substr(0,pos));
									mResult.insert(make_pair(RuleFiles[j].substr(0,pos),RuleFiles[j].substr(rpos+1)));									
								}
						}					

				}
			//查询当前输入与所有输出结果间的编辑距离，如果不大于5就推荐
			sResult = recommend(Output,input,mResult);
			if(sResult.empty())
				{
					ofs<<"您的输入，与其他类似的输出差异性较大，无法为您推荐，建议您调整希望查询的内容";					
				}
			else
				{
					for(siter=sResult.begin();siter!=sResult.end();++siter)
						{
							ofs<<*siter<<";";				
						}					
				}

		}
private:
	std::vector<FILE> m_vRuleFiles;
	std::map<std::string,std::string> m_mRulePair;//first:文件名:行号,second:对应的句式(未扩充前)
	LineIndexManager* m_pLineIndexManager;
};

}//namespace sae

#endif //__SAE__RULE_DEFINATION__HPP__
