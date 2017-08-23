#ifndef _CMDARGS_HPP_
#define _CMDARGS_HPP_
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <string.h>
#include "lib4/IO.hpp"
#include "lib4/Debug.hpp"


class CmdArgs {
     public:
         CmdArgs(){}
     public:
         std::string usage() {
             std::string sRet;
     
             sRet += "USAGE:\n";
             sRet += "    argument              parameter           intorduction.\n";
             sRet += "    -d,--data             [file-name]         Specify the path of data.\n";
             sRet += "    -o,--output           [file-name]         Write output to a directory .\n";
             sRet += "    -h,--help                      			Help info .\n";			 
             sRet += "\n";
             sRet += "EXAMPLES:\n";
             sRet += "    (查询帮助信息)               ./main -h\n";
             sRet += "    (抽取相同规则)               ./main     -d data -o result\n"; 
             sRet += "\n    版权所有 2007-2017@vcyber.com 2013-2017@robosay.com\n";
             sRet += "    使用中问题请联系:chenpeng@mail.vcyber.com\n";
     
             return sRet;
         }     
         bool parse(int argc, char** argv, std::string& sRet){
             std::vector<std::string> vArg;
             for(int i=1; i<argc; i++){
                 vArg.push_back(argv[i]);
             }
     
             return parse(vArg,sRet);
         }
     
         bool parse(const std::vector<std::string>& vArg, std::string& sRet){
             if(vArg.empty()){
                 sRet+=usage();
                 return false;
             }
     
             size_t uLast = vArg.size();
             //read arguments
             for(size_t i=0; i<vArg.size(); i++){
                 if(vArg.at(i) == "-h" || vArg.at(i) == "--help"){
                     sRet+=usage();
                     return false;
                 }
				 else if(vArg.at(i) == "-d" || vArg.at(i) == "--data"){
                     if(i < uLast){
                             m_sInFileName = vArg.at(++i);
                         }else{
                             return false;
                         }					 			
				 }
				 else if(vArg.at(i) == "-o" || vArg.at(i) == "--output"){
                     if(i < uLast){
                             m_sOutFileName = vArg.at(++i);
                         }else{
                             return false;
                         }					 			
				 }				 
             }
     
             return true;
         } 
		 std::string get_InFileName(){
			return m_sInFileName;
		 }
		 std::string get_OutFileName(){
			return m_sOutFileName;
		 }		 
     private:
         std::string m_sInFileName;
		 std::string m_sOutFileName;
};    

#endif //_CMDARGS_HPP_














