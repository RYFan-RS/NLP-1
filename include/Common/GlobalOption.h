#ifndef __AISMS_GLOBAL_OPTION_H__
#define __AISMS_GLOBAL_OPTION_H__
#include <map>
#include "lib4/String.hpp"

namespace aisms{

class GOption{
public:
	bool bDebugMode;
	bool bSilenceMode;
	bool bSingleAnswer;
	std::map<std::string,int> mDebugModule;
	std::string sDate;
	
	std::string toStr() const{
		std::stringstream ss;
		std::map<std::string,int>::const_iterator iter;
		
		ss<<"\"GOption\":{\"Mode\":\""<<(bDebugMode ? "*Debug" : "")<<(bSilenceMode ? "*Slient" : "")<<"\"";
		if(bDebugMode && !mDebugModule.empty()){
			ss<<",\"Module\":{";
			for(iter=mDebugModule.begin(); iter!=mDebugModule.end(); ++iter){
				ss<<"\""<<iter->first<<"\":";
				if(iter->second == 1){
					ss<<"\"INFO\"";
				}else if(iter->second == 2){
					ss<<"\"DEBUG\"";
				}else{
					ss<<"\"OFF\"";
				}
			}
			ss<<"]";
		}
		ss<<"}";
		return ss.str();
	}
	
	bool execute(acl::String sCommand){
		sCommand.toUpper();
		
		if(sCommand == "* ?" || sCommand == "* HELP"){
			printHelp();
			return true;
		}else if(sCommand == "* SHOW STATUS"){
			std::cout<<toStr()<<std::endl;
			return true;
		}else if(sCommand.find("* SET") == 0){
			std::vector<std::string> v = sCommand.separate(" ");
			if(v.size() != 4){
				std::cout<<"Error: \"* set [module-name] [level]\", four elements needed."<<std::endl;
			}
			
			if(set(v[2],v[3]) == false){
				
			}
			return true;
		}else if(sCommand.find("* SWITCH") == 0){
			std::vector<std::string> v = sCommand.separate(" ");
			if(v.size() != 3){
				std::cout<<"Command format error!"<<std::endl;
			}
			
			if(v[2] == "DEBUG"){
				switchBool(bDebugMode);
			}else if(v[2] == "SILENT"){
				switchBool(bSilenceMode);
			}else{
				std::cout<<"Command format error!"<<std::endl;
			}
			
			return true;
		}
		return false;
	}
	
	GOption(){
		bDebugMode = false;
		bSilenceMode = false;
		bSingleAnswer = false;
		sDate = "";
		mDebugModule.insert(std::make_pair("KNOWLEDGEBASE::DOJUMP",0));
		mDebugModule.insert(std::make_pair("INPUT::SHIFTDATE",0));
		mDebugModule.insert(std::make_pair("MIXPARSER::DATEPROCESS",0));
		mDebugModule.insert(std::make_pair("DATECONVERTER::AUTOADJUSTDATERANGE",0));
	}
	
private:
	void printHelp(){
		std::cout<<"Support commands: {\"* show status\",\"* set [module-name] [level]\",\"* switch [Debug/Silent]\"}"<<std::endl;
	}
	
	void switchBool(bool& b){
		if(b == true) b = false;
		else b = true;
	}
	
	bool set(const std::string& sModule, std::string sLevel){
		std::map<std::string,int>::iterator iter = mDebugModule.find(sModule);
		if(iter == mDebugModule.end()){
			std::cout<<"Error: Can't find module ["<<sModule<<"]. Use \"* show status\" to view support modules."<<std::endl;
			return false;
		}
		
		for(size_t i=0; i<sLevel.length(); i++){
			sLevel[i] = toupper(sLevel[i]);
		}
		if(sLevel == "INFO"){
			iter->second = 1;
		}else if(sLevel == "DEBUG" || sLevel == "ON"){
			iter->second = 2;
		}else{
			return false;
		}
		
		return true;
	}
};

extern GOption gOption;

}//namespace aisms

#endif//__AISMS_GLOBAL_OPTION_H__
