#include <string.h>
#include "CmdArgs.hpp"
#include "CorpusGenerator.hpp"

using namespace sae;

int main(int argc, char** argv){  
	std::string sMessage;
	CmdArgs cmd;

	if(false==cmd.parse(argc, argv, sMessage)){
		std::cout<<sMessage<<std::endl;
		return 1; 
	}
	
	CorpusGenerator generator;
	std::string sPath = cmd.get_DataFile();
	
	cmd.printCompileMode("开始读取文件",BRIGHT_GREEN);
	if(generator.readFile(sPath,COMMON_DEFINATION) == false){
		generator.dumpErrorData();
		cmd.printCompileMode("  >< 读取通用集合定义失败",BRIGHT_RED);
		return 2;
	}
	cmd.printCompileMode("  -> 读取通用集合定义完成",BRIGHT_GREEN);	
	if(generator.readFile(sPath,BUSINESS_DEFINATION) == false){
		generator.dumpErrorData();
		cmd.printCompileMode("  >< 读取领域集合定义失败",BRIGHT_RED);
		return 3;
	}
	cmd.printCompileMode("  -> 读取领域集合定义完成",BRIGHT_GREEN);		
	if(generator.readFile(sPath,VARIABLE_DEFINATION) == false){
		generator.dumpErrorData();
		cmd.printCompileMode("  -> 读取变量集合定义完成",BRIGHT_RED);	
		return 4;
	}
	cmd.printCompileMode("  -> 读取变量集合定义完成",BRIGHT_GREEN);			
	if(generator.readFile(sPath,RULE_DEFINATION) == false){
		generator.dumpErrorData();
		cmd.printCompileMode("  >< 读取规则定义失败",BRIGHT_RED);	
		return 5;
	}
	cmd.printCompileMode("  -> 读取规则定义完成",BRIGHT_GREEN);			
	
	//generator.dumpFileData();	
	cmd.printCompileMode("开始解析数据",BRIGHT_GREEN);			
	if(generator.parse() == false){
		generator.dumpErrorData();
		cmd.printCompileMode("  >< 解析数据失败",BRIGHT_RED);		
		return 6;
	}

	cmd.printCompileMode("  -> 解析数据成功",BRIGHT_GREEN);		
	cmd.printCompileMode("开始生成语料",BRIGHT_GREEN);	

	generator.generateCorpus();
	cmd.printCompileMode("  -> 生成语料成功",BRIGHT_GREEN);
	cmd.printCompileMode("开始将语料写入文件",BRIGHT_GREEN);
	generator.writeToDirectory(cmd.get_OutFile().c_str());	
	cmd.printCompileMode("  -> 语料写入成功",BRIGHT_GREEN);		
	cmd.printSearchMode("开始进入搜索模式,请稍后......",BRIGHT_GREEN);

	
	if(cmd.isSearchMode()){
		if(cmd.get_InFile().size()==0){
			//根据用户输入来搜索规则
			generator.input_search(cmd);
		}else{
			//从文件中读取输入来搜索规则			
			if(true==generator.file_search(cmd)){
				cmd.printSearchMode("结果已写入指定输出文件",BRIGHT_GREEN);							
			}else{
				std::cout<<cmd.usage()<<std::endl;
			}
		}
	}

    return 0;
}    
