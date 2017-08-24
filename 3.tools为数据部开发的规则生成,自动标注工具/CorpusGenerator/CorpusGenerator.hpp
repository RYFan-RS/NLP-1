#ifndef __SAE__CORPUS_GENERATOR__HPP__
#define __SAE__CORPUS_GENERATOR__HPP__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "LineIndexManager.h"
#include "CommonDefination.hpp"
#include "BusinessDefination.hpp"
#include "VariableDefination.hpp"
#include "RuleDefination.hpp"
#include "TypeDefine.h"
#include "ErrorData.h"
#include "Utility.h"
#include "lib4/Directory.hpp"
#include "lib4/Definations.hpp"
#include "lib4/File.hpp"
#include "DynamicParser/Parser.h"
#include "lib4/Debug.hpp"
#include <map>
#include "lib4/IO.hpp"
#include <string.h>
#include "CmdArgs.hpp"
#include "lib4/Time.hpp"

using namespace sae;
namespace sae
{
class CorpusGenerator{
public:
	CorpusGenerator(){
		m_ruleIndexID = -1;
	}

	bool readFile(std::string sPath, FILE_DATA_TYPE fileType){
		if(fileType == COMMON_DEFINATION){
			sPath += "/common";
		}else if(fileType == BUSINESS_DEFINATION){
			sPath += "/business";
		}else if(fileType == VARIABLE_DEFINATION){
			sPath += "/variable";
		}else if(fileType == RULE_DEFINATION){
			sPath += "/rules";
		}else{
			ErrorData errData;
			errData.m_errMsg = "未知的数据文件类型";
			errData.errorTrace(__FILE__,__LINE__-1);
			m_errs.push_back(errData);
			return false;
		}
		
		acl::Directory dir(sPath);
		if(dir.recursivelyRead() == false){
			ErrorData errData;
			errData.m_errMsg = "读取目录["+sPath+"]出错";
			errData.errorTrace(__FILE__,__LINE__-1);
			m_errs.push_back(errData);
			return false;
		}
		
		for(size_t i=0; i<dir.size(); i++){
			if(readFile(sPath,dir[i],fileType) == false){
				return false;
			}
		}
		
		return true;
	}
	
	bool parse(){
		//开始解析前，将LineIndex信息传递给各个管理器
		m_commonDefination.setLineIndexManagerPointer(&m_lineIndexManager);
		m_commonDefination.setFilesPointer(&m_files);
		m_variableDefination.setLineIndexManagerPointer(&m_lineIndexManager);
		m_variableDefination.setFilesPointer(&m_files);
		m_ruleDefination.setLineIndexManagerPointer(&m_lineIndexManager);
		
		ErrorData errData;
		for(size_t i=0,size=m_files.size(); i<size; i++){
			if(m_files[i].m_type == COMMON_DEFINATION){
				if(m_commonDefination.parse(m_files[i],i,m_files,errData) == false){
					errData.errorTrace(__FILE__,__LINE__-1);
					m_errs.push_back(errData);
					return false;
				}
			}else if(m_files[i].m_type == BUSINESS_DEFINATION){
				if(m_businessDefination.parse(m_files[i],i,m_files,errData) == false){
					errData.errorTrace(__FILE__,__LINE__-1);
					m_errs.push_back(errData);
					return false;
				}
			}else if(m_files[i].m_type == VARIABLE_DEFINATION){
				if(m_variableDefination.parse(m_files[i],i,m_files,errData) == false){
					errData.errorTrace(__FILE__,__LINE__-1);
					m_errs.push_back(errData);
					return false;
				}
			}else if(m_files[i].m_type == RULE_DEFINATION){
				std::string sBussiness = getRuleBussiness(m_files[i].m_name);
				if(m_ruleDefination.parse(m_files[i],sBussiness,i,m_files,m_commonDefination,m_businessDefination,m_variableDefination,errData) == false){
					errData.errorTrace(__FILE__,__LINE__-1);
					m_errs.push_back(errData);
					return false;
				}
			}
		}
		
		//m_ruleDefination.dump(m_lineIndexManager);
		
		return true;
	}
	
	std::string getRuleBussiness(const std::string& sFileName){
		size_t nEnd = sFileName.rfind("/");
		size_t nBeg = sFileName.rfind("/",nEnd-1);
		return sFileName.substr(nBeg+1,nEnd-nBeg-1);
	}
	
	void generateCorpus(){
		m_ruleDefination.generateCorpus(m_lineIndexManager);
	}
	
	void writeToDirectory(const char* sOutDirName){
		m_ruleDefination.writeToDirectory(sOutDirName);
	}
	
	void dumpErrorData(const std::string& msg = "", std::ostream& ofs = std::cout) const{
		if(!msg.empty()) {
			ofs<<msg<<std::endl;
		}
		for(size_t i=0; i<m_errs.size(); i++) {
			m_errs[i].dump(m_files,m_lineIndexManager,ofs);
		}
	}
	
	void dumpFileData(const std::string& msg = "", std::ostream& ofs = std::cout) const{
		ofs<<msg<<std::endl;
		for(size_t i=0; i<m_files.size(); i++){
			ofs<<m_files[i].m_name<<std::endl;
			ofs<<"----------------------------"<<std::endl;
			for(size_t j=0; j<m_files[i].m_file.size(); j++){
				ofs<<m_lineIndexManager.getLineNum(m_files[i].m_file[j].m_lineNum)<<":"<<m_files[i].m_file[j].m_line<<std::endl;
			}
		}
	}
	
	class DictType{
		public:
			int nType; //1:common;2:business;3:variable;
			std::string sVariableTag; //仅在nType=3时有值
			std::string sFileName;//仅在nType=3时有值
	};
	
	void build_dict(std::set<std::string>& seWord, std::multimap<std::string,DictType> &mmap){					
		for(size_t i=0,size=m_files.size(); i<size; i++){
			for(size_t j=0;j<m_files[i].m_file.size();j++){
				const acl::Line& sLine = m_files[i].m_file[j].m_line;
				if(sLine.empty()) continue;//空行
				
				if(m_files[i].m_type == VARIABLE_DEFINATION){
					size_t pos = sLine.find("\t",0);
					//没有找到\t
					if(pos == std::string::npos) continue;
					acl::String sVariable = sLine.substr(0, pos);
					//去除掉空格
					sVariable.removeChar(' ');
					seWord.insert(sVariable);
					
					DictType dicType;
					dicType.nType = 3;
					//m_files[i].m_name为"/limitline/2_date.txt"
                    std::string temp=m_files[i].m_name.substr(1);
                    dicType.sFileName = acl::Utility::extractAlphaNumberString(temp);
					dicType.sVariableTag = sLine.substr(pos+1,sLine.size());
					mmap.insert(std::make_pair(sVariable,dicType));
				}else if(m_files[i].m_type == BUSINESS_DEFINATION){
					seWord.insert(sLine);
					
					DictType dicType;
					dicType.nType = 2;
					mmap.insert(std::make_pair(sLine,dicType));
				}else if(m_files[i].m_type == COMMON_DEFINATION){
					if(sLine.isBeginWith("#")) continue;
					seWord.insert(sLine);
					
					DictType dicType;
					dicType.nType = 1;
					mmap.insert(std::make_pair(sLine,dicType));
				}							
			}
		}
	}
		
	void input_parse(std::vector<std::string> vWord,std::multimap<std::string,DictType>& mdata,sae::CmdArgs &cmd,std::string input,std::ostream& ofs = std::cout){
		std::vector<std::string> ::const_iterator it_vec;
		typedef std::multimap<std::string,DictType>::const_iterator const_dict_iterator;
		const_dict_iterator iter_dict;
		std::pair<const_dict_iterator,const_dict_iterator> pair_iter_dict;
		std::vector<std::string> temp;		
		std::string output;
		std::string output_nochange;
		std::set<std::string> varaiable;
		std::set<std::string> business;
		std::set<std::string> common;	
		std::set<std::string> label;
		//variable需要保证显示的顺序,此处特殊处理
		std::vector<std::string> label2;
		std::vector<std::string> varaiable2;
		
        std::set<std::string>::iterator setIter;
		bool bRecommend = false;
		bool bRepeat = false;
        //ofs<<"\n分词结果为:"<<acl::debug::toStr(vWord)<<std::endl;
        std::string sinput = input;
       
		for(size_t j =0;j<vWord.size();j++){

			bRepeat = false;
			output_nochange += vWord[j];
            
			pair_iter_dict = mdata.equal_range(vWord[j]);
            const_dict_iterator iter = pair_iter_dict.first;
            //说明词库里没有
            if(iter == pair_iter_dict.second){
                     output += vWord[j];
                     continue;
                }
            
            for(;iter != pair_iter_dict.second;++iter){
                    /*多个的情况*/
    				if((iter->second.nType == 3)&&((cmd.get_BusinessMode().empty())||(iter->second.sFileName == cmd.get_BusinessMode()))){
                        //没有指定商业模式的时候，直接查找。指定商业模式的时候，只查找指定的商业模式
                        varaiable.insert(iter->first);
    					label.insert(iter->second.sVariableTag);
						if(label2.end() == find(label2.begin(),label2.end(),iter->second.sVariableTag)){
							//去重
							label2.push_back(iter->second.sVariableTag);
						}
						if(varaiable2.end() == find(varaiable2.begin(),varaiable2.end(),iter->first)){
							//去重
							varaiable2.push_back(iter->first);
						}						
    					vWord[j] = "@"+iter->second.sVariableTag;                    
    				}
                    else if(iter->second.nType == 2){
                        business.insert(vWord[j]);
                    }
                    else if(iter->second.nType == 1){
                        common.insert(vWord[j]);  
                    }  
                    if((iter->second.nType != 3)||(cmd.get_BusinessMode().empty())||(iter->second.sFileName == cmd.get_BusinessMode()))  {
							//同一个词可能存在多个label，此时只保留一个label，稍后再处理
                            if((std::string::npos == output.find(vWord[j]))&&(bRepeat == false)){
                                    //去重
                                    output += vWord[j];
									bRepeat = true;
                                }
                        }                    
                }
		}

		ofs<<cmd.get_BusinessMode()<<"\t"<<output_nochange<<"\t";

		//一个词的label可能存在多个	
		acl::String temp2 = output;
		size_t pos = temp2.find("@");
		bool bFind = false;
		if(pos != std::string::npos) {
				std::string temp3 = acl::Utility::extractAlphaNumberString(output.substr(pos+1));
				for(std::set<std::string>::iterator iter = label.begin();iter!=label.end();iter++){
					std::string temp4 = *iter;
					temp2 = output;
					temp2.replace(temp3,temp4);
					if(true==m_ruleDefination.input_search(temp2,cmd,ofs))
						{
							bFind = true;
							break;
						}					
				}
		}

		if(false == bFind){
			//没有查找到的化,输出tab键来分割
			ofs<<"无法为它提取规则:"<<output<<"\t\t\t\t";
			bRecommend = true;
		}

		ofs<<"common:";
		for(setIter = common.begin();setIter != common.end();setIter++)
			{			
				ofs<<*setIter<<";";				
			}	
		ofs<<"\tbusiness:";
		for(setIter = business.begin();setIter != business.end();setIter++)
			{			
				ofs<<*setIter<<";";				
			}					
		ofs<<"\tvariable:";
		for(size_t j =0;j<varaiable2.size();j++){		          
                ofs<<varaiable2[j]<<";";                
            }					
		ofs<<"\tlabel:";
		for(size_t j =0;j<label2.size();j++){           
                ofs<<label2[j]<<";";                
            }
		if(bRecommend == true){
			ofs<<"\t为您推荐的类似的规则:"; 
			//同一个名词的label可能存在多个
			m_ruleDefination.recommend(output,label,ofs);
		}
		ofs<<std::endl;
	}				
	void input_search(CmdArgs &cmd){
		std::string input;
		aisms::dypsr::Parser diyParser;
		std::set<std::string> seWord;
		std::vector<std::string> vDictWords;
		std::vector<std::string> words;
		std::multimap<std::string,DictType> mdata;        
		
		//构建词库     
		build_dict(seWord,mdata);
		for(std::set<std::string>::const_iterator iter=seWord.begin(); iter!=seWord.end(); ++iter){
			vDictWords.push_back(*iter);
		}
		seWord.clear();
		
		//初始化分词引擎
		diyParser.init("all", vDictWords);
		vDictWords.clear();
		vDictWords.resize(0);
  
		while(1){
			std::cout<<"请输入您想查询的语句(按CTRL+C结束):"<<std::endl;
			getline(std::cin,input);
            aisms::dypsr::Result result = diyParser.parse(input,"all");
            words=result.getWords();
            input_parse(words,mdata,cmd,input);
		}
	}

	bool file_search(CmdArgs &cmd){
		acl::File file;
		acl::Line line;
		std::vector<std::string> result;
		std::fstream ofs(cmd.get_ResultFile().c_str(), std::ios::out);
		std::string input;
		aisms::dypsr::Parser diyParser;
		std::set<std::string> seWord;
		std::vector<std::string> vDictWords;
		std::vector<std::string> words;
		std::multimap<std::string,DictType> mdata;			
		
 		file.setCommentMarks("#");
 		if(false == file.read(cmd.get_InFile())){
 				std::cout<<"无法打开输入文件，请输入正确的文件名"<<std::endl;
 				return false;
 			}

		acl::Time timer;
		timer.start("Init");

		//构建词库
		build_dict(seWord,mdata);
		for(std::set<std::string>::const_iterator iter=seWord.begin(); iter!=seWord.end(); ++iter){
			vDictWords.push_back(*iter);
		}
		seWord.clear();
		
		//初始化分词引擎
		diyParser.init("all", vDictWords);
		vDictWords.clear();
		vDictWords.resize(0);

		timer.end("Init");
		PE(timer.toStr("Init"))

		timer.start("File");
		
		//循环处理文件
		for(int i = 0;i<file.size();i++)
		{
			line = file[i];
            //文件中第一列是业务模式，此处做特殊处理，将其提取出来
            size_t pos  = line.find('\t');
            if(pos != std::string::npos){
                    cmd.set_BusinessMode(line.substr(0,pos));
                    line = line.substr(pos+1);
                }            
			aisms::dypsr::Result result = diyParser.parse(line,"all");
			words=result.getWords();
			input_parse(words,mdata,cmd,line,ofs);
		}

		timer.end("File");
		PE(timer.toStr("File"))	

		return true;
	}


private:
	bool isNonUTFLine(const std::string &strIn) const{
		    int amout=0;
		int i=0,length=strIn.length();
		while(1) {
			if(i >= length)
				break;
			if(i<length-1) {
				if(strIn[i] == '/' && strIn[i+1] == '/') break;
			}
			if((strIn[i] & 0x80) == 0x00) {
				amout++;
				i++;
			} else if((strIn[i] & 0xE0) == 0xC0) {
				amout++;
				i+=2;
			} else if((strIn[i] & 0xF0) == 0xE0) {
				amout++;
				i+=3;
			} else {
				return true;
			}
		}
		return false;
	}
	
	std::string normalizeTailOfTheLine(const std::string& s) const{
		int i,length;
		length = s.length();
		size_t pos = std::string::npos;
		for(i=--length; i>-1; i--) {
			if(s[i] == '\0' || s[i] == '\r' || s[i] == '\n' || s[i] == '\t') {
				pos = i;
			} else {
				break;
			}
		}

		return s.substr(0,pos);
	}
	
	bool readFile(const std::string& sPath, const std::string& fileName, FILE_DATA_TYPE fileType){
		std::string s;
		sae::FILE file;
		sae::LINE line;
		int count=0;
		LineIndex lineIndex;
		ErrorData errData;
		const char *utf8bom = "\xef\xbb\xbf";
		std::string sRealFileName = sPath + "/" + fileName;

		std::fstream ifp(sRealFileName.c_str(),std::ios::in);
		if(!ifp) {
			errData.m_errMsg = "无法打开文件["+sRealFileName+"]";
			errData.errorTrace(__FILE__,__LINE__);
			m_errs.push_back(errData);
			return false;
		}
		
		//record the file data type
		file.m_type = fileType;
		
		//record file name in parser
		file.m_name = fileName;

		//record file name in storage
		lineIndex.m_fileName = file.m_name;

		//record line start index number in storage
		lineIndex.m_start = m_ruleIndexID+1;

		while(!ifp.eof()) {
			//read one line from file and record it
			std::getline(ifp,s);

			//if first line contains BOM, skip it.
			if(count==0 && !s.substr(0,3).compare(utf8bom)) s = s.substr(3);

			count++;
			m_ruleIndexID++;
			s = normalizeTailOfTheLine(s);

			//check for non-UTF8 characters
			if(isNonUTFLine(s)==true && !s.empty()) {
				std::stringstream ss;
				ss << "文件'"<< fileName <<"'第"<<count<<"行:'"<<count<<"'含有非UTF8字符。";
				errData.m_errMsg = ss.str();
				errData.errorTrace(__FILE__,__LINE__);
				m_errs.push_back(errData);
				return false;
			}

			line.m_line = s;
			line.m_lineNum = m_ruleIndexID;
			file.m_file.push_back(line);
		}
		ifp.close();
		m_files.push_back(file);
		//record line end index number in storage
		lineIndex.m_end = m_ruleIndexID;
		//record  line index information in storage
		if(lineIndex.m_start <= lineIndex.m_end) {
			m_lineIndexManager.push_back(lineIndex);
		}

		return true;
	}
	

private:
	int m_ruleIndexID;
	std::vector<sae::FILE> m_files;
	LineIndexManager m_lineIndexManager;
	CommonDefination m_commonDefination;
	BusinessDefination m_businessDefination;
	VariableDefination m_variableDefination;
	RuleDefination m_ruleDefination;
	std::vector<ErrorData> m_errs;
};

}//namespace sae

#endif //__SAE__CORPUS_GENERATOR__HPP__
