#include "ExtractRule.h"



bool ExtractRule::CompareRules( std::vector<std::set<std::string> > &set1, std::vector<std::set<std::string> > &set2){
	std::vector<std::set<std::string> > ::iterator iter1;
	std::vector<std::set<std::string> > ::iterator iter2;
	std::set<std::string> sResult;

	for(iter1=set1.begin();iter1!=set1.end();iter1++){
		for(iter2=set2.begin();iter2!=set2.end();iter2++){
			if((*iter1).size() == (*iter2).size()){
				//求交集
				std::set_intersection((*iter1).begin(),(*iter1).end(),(*iter2).begin(),(*iter2).end(),std::inserter(sResult,sResult.begin()));
				if((*iter1).size() == sResult.size()){
					return true;
				}
			}
		}
	}

	return false;
}
void ExtractRule::setRuleClass(int Class1,int Class2,std::map<int,int> &mClassRecord,std::vector<std::vector<int> > &vSameRule){
	std::map<int,int>::iterator iter1; 
	std::map<int,int>::iterator iter2; 
	std::vector<int> vTemp;


	iter1 = mClassRecord.find(Class1);
	iter2 = mClassRecord.find(Class2);

	//如果1和2都没有类别，那么创立一个新的类别
	if((iter1 == mClassRecord.end())&&(iter2 == mClassRecord.end())){
		vTemp.push_back(Class1);
		vTemp.push_back(Class2);
		vSameRule.push_back(vTemp);
		mClassRecord.insert(std::make_pair(Class1,vSameRule.size()-1));
		mClassRecord.insert(std::make_pair(Class2,vSameRule.size()-1));	
		return;
	}

	//如果1有类别，而2没有，那么将2分类为1
	if((iter1 != mClassRecord.end())&&(iter2 == mClassRecord.end())){
		vSameRule[iter1->second].push_back(Class2);
		mClassRecord.insert(std::make_pair(Class2,iter1->second));	
		return;	
	}
	//如果2有类别，而1没有，那么将1分类为2
	if((iter2 != mClassRecord.end())&&(iter1 == mClassRecord.end())){
		vSameRule[iter2->second].push_back(Class1);
		mClassRecord.insert(std::make_pair(Class1,iter2->second));	
		return;	
	}	

	//如果1，2均有类别但是两者不一样，则报错
	assert((iter2 != mClassRecord.end())&&(iter1 != mClassRecord.end())&&(iter1!=iter2));
}



void ExtractRule::getSameRule(std::map<int,std::vector<std::set<std::string> > > &mAllMap,std::vector<std::vector<int> > &vSameRule){
	std::map<int,std::vector<std::set<std::string> > >::iterator mIter; 
	std::map<int,std::vector<std::set<std::string> > >::iterator mIterNext; 	
	std::map<int,int> mClassRecord;//记录哪些rule属于哪一个类别
	std::map<int,int>::iterator mIterRecord;
	std::vector<std::string>::iterator vIter;
	std::vector<std::string>::iterator vIterNext;
	bool bfind = false;

	//每2种规则的vector进行比较，完全相同的话就存入vSameRule
	for(mIter = mAllMap.begin();mIter!=mAllMap.end();++mIter){
		mIterNext = mIter;
		mIterNext++;
		for(;mIterNext!=mAllMap.end();++mIterNext){
			if(true==CompareRules(mIter->second,mIterNext->second)){
				//记录哪些rule属于哪一个类别
				setRuleClass(mIter->first,mIterNext->first,mClassRecord,vSameRule);
			}
		}
	}
	
	return;
}
void ExtractRule::initStaticParser(const std::string& userDefineWordFilePath)
{
    // 初始化静态分词引擎
    if(cppjiebaParser == NULL) {
		const char* const DICT_PATH = "./cppjieba/dict/jieba.dict.utf8";
		const char* const HMM_PATH = "./cppjieba/dict/hmm_model.utf8";
		//const char* const USER_DICT_PATH = "./data/dict/user.dict.utf8";
		const char* const IDF_PATH = "./cppjieba/dict/idf.utf8";
		//const char* const STOP_WORD_PATH = "../cppjieba/dict/stop_words.utf8";	
		
        cppjiebaParser = new cppjieba::Jieba(DICT_PATH,HMM_PATH, userDefineWordFilePath,IDF_PATH);	
    }
}
void ExtractRule::showSameRule(CmdArgs & cmd,std::vector<std::vector<int> > &vSameRule,std::string fileName){
	std::string outPut = cmd.get_OutFileName()+fileName;
	std::fstream ofp(outPut.c_str(),std::ios::out);
	std::map<int,std::string>::iterator iter;

	std::string sCommand = "mkdir -p " + cmd.get_OutFileName();
	system(sCommand.c_str());

	if(0 == vSameRule.size()){
		ofp<<"########"<<fileName<<":抱歉,本文档种没有相同的规则句式"<<"########"<<std::endl;
		ofp.close();
		return;
	}


	ofp<<"########"<<outPut<<"########"<<std::endl;
	ofp<<"########"<<"聚类结果种类:"<<vSameRule.size()<<"########"<<std::endl;
	
	for(size_t i = 0;i<vSameRule.size();i++){
		ofp<<"......................."<<i<<"......................."<<std::endl;
		for(size_t j = 0;j<vSameRule[i].size();j++){
			//查找index对应的原始rule
			iter = mOriginRuleIndex.find(vSameRule[i][j]);
			if(iter != mOriginRuleIndex.end()){
				ofp<<iter->second<<std::endl;
			}
		}
	}
	ofp.close();

	return;
}

bool ExtractRule::readFile(CmdArgs & cmd,std::string fileName){
	std::string sRealfile = cmd.get_InFileName()+fileName;
	acl::File file;
	
	//int是每条规则对应的索引，vector是这条规则对应的不同的句式进行分词后的结果
	std::map<int,std::vector<std::set<std::string> > > mAllMap;
	std::vector<std::vector<int> > vSameRule;
	std::set<std::string> ssTemp;
	std::vector<std::string> vTemp;
	std::vector<std::set<std::string> > vWordSet;
    std::vector<std::string> vRet;
    std::string sRet;
	std::map<std::string,std::vector<std::string> > subSentence;	
	int Index = 0;

	initStaticParser();

	file.setCommentMarks("#");
	if(false == file.read(sRealfile)){
			std::cout<<"无法打开输入文件，请输入正确的文件名"<<std::endl;
			return false;
	}

	for(int i = 0;i<file.size();i++){
		std::string s=file[i];
		//read one line from file and record it

		//每条规则对应一个索引进行优化
		mOriginRuleIndex.insert(std::make_pair(Index++,s));
		
		//获取原始的规则和对应的解析后的规则
		vTemp = acl::Algorithm::permutationAndCombination(s);
		//对每一句话进行分词
		for(size_t i = 0;i<vTemp.size();i++){
			//subSentence是扩展后的结果，本处不用
			(reinterpret_cast<cppjieba::Jieba const*>(cppjiebaParser))->CutForSearchEx(vTemp[i],vRet,subSentence,true);
			for(size_t j = 0;j<vRet.size();j++){
				ssTemp.insert(vRet[j]);
			}
			vWordSet.push_back(ssTemp);				
			ssTemp.clear();
		}
		
		mAllMap.insert(std::make_pair(Index-1,vWordSet));
		vWordSet.clear();
	}
	
	//获取相同的规则
	getSameRule(mAllMap,vSameRule);

	//显示聚类
	showSameRule(cmd,vSameRule,fileName);

	mOriginRuleIndex.clear();

	return true;
	
}

void ExtractRule::parse(CmdArgs & cmd){
	//读输入文件
	acl::Directory dir(cmd.get_InFileName());
	if(dir.recursivelyRead() == false){
		std::cout<<"读取目录["+cmd.get_InFileName()+"]出错"<<std::endl;
		return;
	}

	//解析文件
	for(size_t i=0; i<dir.size(); i++){
		if(readFile(cmd,dir[i]) == false){
			return;
		}
	}
}
