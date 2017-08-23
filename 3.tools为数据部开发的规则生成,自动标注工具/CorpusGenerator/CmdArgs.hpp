#ifndef _CMDARGS_HPP_
#define _CMDARGS_HPP_
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <string.h>
#include "lib4/IO.hpp"
#include "lib4/Debug.hpp"
#include "TypeDefine.h"

namespace sae
{
class CmdArgs {
     public:
         CmdArgs():m_bCompile(true),m_bSearch(false),m_bSimple(false){}
     public:
         std::string usage() {
             std::string sRet;
     
             sRet += "USAGE:\n";
             sRet += "    argument              parameter           intorduction.\n";
             sRet += "    -h,--help                                 Print this message.\n";
             sRet += "    -v,--version                              Print version message.\n";
             sRet += "    --history                                 Print tool information and then exit.\n";
             sRet += "    -d,--data             [file-name]         Specify the path of data.\n";
             sRet += "    -s,--search                               search rule by result.\n";
             sRet += "    -si,--simple                              when user only want to know rules.\n";        
             sRet += "    -c,--compile          [file-name]         Read data and compile.\n";
             sRet += "    -o,--output           [file-name]         Write output to a directory .\n";
             sRet += "    -i,--input            [file-name]         Search rules by result which is writen in file when mode is search.\n";
             sRet += "    -r,--result           [file-name]         Write output to a file when user specify rules which is in files\n"; 
             sRet += "    -b,--business         [file-name]         Specify business mode\n";
             sRet += "\n";
             sRet += "EXAMPLES:\n";
             sRet += "    (查询帮助信息)               ./main -h\n";
             sRet += "    (默认是编译模式)             ./main     -d dict1 -o dict2\n";
             sRet += "    (在线搜索模式)               ./main -s  -d dict1 -o dict2\n";
             sRet += "    (文件搜索模式)               ./main -s  -d dict1 -o dict2 -i text1.txt -r text2.txt\n";
             sRet += "    (文件搜索模式)               ./main -s  -d dict1 -o dict2 -i text1.txt -r text2.txt -b navigation\n";
             sRet += "    (简单在线搜索模式)           ./main -si -d dict1 -o dict2\n";
             sRet += "    (简单文件搜索模式)           ./main -si -d dict1 -o dict2 -i text1.txt -r text2.txt\n";   
             sRet += "\n    版权所有 2007-2017@vcyber.com 2013-2017@robosay.com\n";
             sRet += "    使用中问题请联系:chenpeng@mail.vcyber.com\n";
     
             return sRet;
         }
     
         std::string version(){
             std::string sRet;
             sRet += "基于规则的语料生成工具 Ver2.4 20170809\n";
             return sRet;
         }
         std::string history(){
             std::string sRet;
             sRet +="-----------------------------\n";
             sRet +="基于规则的语料生成工具 Ver1.0 201704\n";
             sRet +="初始版本\n";
             sRet +="-----------------------------\n";
             sRet +="基于规则的语料生成工具 Ver1.1 201705\n";
             sRet +="增加了对common集合定义中!符号的支持\n";
             sRet +="-----------------------------\n";
             sRet +="基于规则的语料生成工具 Ver1.2 20170518\n";
             sRet +="增加了对business集合定义的符合#business的支持\n";
             sRet +="-----------------------------\n";
             sRet +="基于规则的语料生成工具 Ver1.3 20170518\n";
             sRet +="修复了生成corpus是出现截断的问题\n";
             sRet +="-----------------------------\n";
             sRet +="基于规则的语料生成工具 Ver1.4 20170608\n";
             sRet +="输出文件中增加了对应规则的打印信息\n";
             sRet +="-----------------------------\n";
             sRet +="基于规则的语料生成工具 Ver1.5 20170615\n";
             sRet +="增加查询规则,命令行参数的功能\n";
             sRet +="-----------------------------\n";
             sRet +="基于规则的语料生成工具 Ver1.6 20170620\n";
             sRet +="增加规则智能推荐功能，增加显示common,variable,business\n";
             sRet +="-----------------------------\n";
             sRet +="基于规则的语料生成工具 Ver1.7 20170622\n";
             sRet +="优化了运行时的速度\n";
             sRet +="-----------------------------\n";			 
             sRet +="基于规则的语料生成工具 Ver1.8 20170630\n";
             sRet +="支持指定业务模式的查找,支持只搜索规则的简单查询模式\n"; 
             sRet +="-----------------------------\n";				 
             sRet +="基于规则的语料生成工具 Ver1.9 20170715\n";
             sRet +="支持在文件中的每一行指定业务模式\n";
             sRet +="-----------------------------\n";				 			 
             sRet +="基于规则的语料生成工具 Ver2.0 20170721\n";
             sRet +="改进分词引擎,优化查找速度,内存消耗情况\n";
             sRet +="-----------------------------\n";				 			 
             sRet +="基于规则的语料生成工具 Ver2.1 20170724\n";
             sRet +="搜索结果支持显示原始未扩充的规则句式\n";
             sRet +="-----------------------------\n";				 			 
             sRet +="基于规则的语料生成工具 Ver2.2 20170727\n";
             sRet +="在生成的规则文件中新增原始的规则句式\n";
             sRet +="-----------------------------\n";				 			 
             sRet +="基于规则的语料生成工具 Ver2.3 20170802\n";
             sRet +="在生成的规则文件中,当查找不到的时候，新增推荐内容\n";
             sRet +="-----------------------------\n";				 			 
             sRet +="基于规则的语料生成工具 Ver2.4 20170809\n";
             sRet +="满足第三期需求:(1)支持同一个词对应不同的标签  (2)支持对文件中的规则进行聚类 (3)优化了推荐方法,修复bug,使推荐内容更加合理\n";

			 
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
                 }else if(vArg.at(i) == "-v" || vArg.at(i) == "--version"){
                     sRet+=version();
                     return false;
                 }else if(vArg.at(i) == "--history"){
                     sRet+=history();
                     return false;
                 }else if(vArg.at(i) == "-d" || vArg.at(i) == "--data"){
                     if(i < uLast){
                             m_sDataFileName = vArg.at(++i);
                         }else{
                             return false;
                         }
                 }else if(vArg.at(i) == "-i" || vArg.at(i) == "--input"){
                     if(i < uLast){
                             m_sInFileName = vArg.at(++i);
                         }else{
                             return false;
                         }           
                 }else if(vArg.at(i) == "-o" || vArg.at(i) == "--output"){
                     if(i < uLast){
                         m_sOutFileName = vArg.at(++i);
                     }else{
                         return false;
                     }
                 }
                 else if(vArg.at(i) == "-s" || vArg.at(i) == "--search"){
                         m_bSearch = true;
                         m_bCompile = false;
                 }
                 else if(vArg.at(i) == "-si" || vArg.at(i) == "--simple"){
                         m_bSearch = true;
                         m_bCompile = false;
                         m_bSimple = true;
                 }            
                 else if(vArg.at(i) == "-c" || vArg.at(i) == "--compile"){
                         m_bCompile = true;
                 }
                 else if(vArg.at(i) == "-r" || vArg.at(i) == "--result"){                
                     if(i < uLast){
                         m_sResultName = vArg.at(++i);
                     }else{
                         return false;
                     }                                           
                 }
                 else if(vArg.at(i) == "-b" || vArg.at(i) == "--business"){              
                     if(i < uLast){
                         m_sBusinessMode = vArg.at(++i);
                     }else{
                         return false;
                     }                                           
                 }            
                 else{
                     sRet+= "Error: Can't parse argument '" + vArg.at(i) + "'!\n\n";
                     sRet+=usage();
                     return false;
                 }
             }
     
             return true;
         }
     
         void printCompileMode(std::string info,std::string color){
             if(m_bCompile){ 
                 std::cout<<color<<info<<color<<RESET_COLOR<<std::endl;
             }           
         }
         void printSearchMode(std::string info,std::string color){
             if(m_bSearch){  
                 std::cout<<color<<info<<color<<RESET_COLOR<<std::endl;
             }   
         }
         bool isSearchMode(){
             return m_bSearch;
         }
         bool isSimpleSearchMode(){
             return m_bSimple;
         }    
         bool isCompileMode(){
             return m_bCompile;
         }
         const std::string& get_InFile(){
             return m_sInFileName;
         }
         const std::string& get_ResultFile(){
             return m_sResultName;
         }   
         const std::string& get_OutFile(){
             return m_sOutFileName;
         }   
         const std::string& get_DataFile(){
             return m_sDataFileName;
         }
         const std::string& get_BusinessMode(){
             return m_sBusinessMode;
         }
        void set_BusinessMode(std::string sMode){
              m_sBusinessMode = sMode;
              return;
         }     
     private:
         std::string m_sInFileName;
         std::string m_sResultName;
         std::string m_sOutFileName;
         std::string m_sDataFileName;
         std::string m_sBusinessMode;
         bool        m_bCompile;
         bool        m_bSearch;
         bool        m_bSimple;
     };    

}//namespace sae

#endif //_CMDARGS_HPP_














