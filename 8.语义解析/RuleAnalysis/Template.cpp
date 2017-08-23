#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "Template.h"
#include "Common/lib4/Utility.hpp"

namespace sae
{

bool Template::parse(const LINE& line, ErrorData& errData){
	// check elements number
	std::string sRule = line.m_line;
    ERROR_TRACE( checkWholePartElementsNumber(line.m_line,errData) )
	
	// read rule name
    std::string sName;
    ERROR_TRACE( getRuleNameFromString(sRule,sName,errData) )
	
	// record the rule name and the key part
	std::map<std::string, VectorString >::iterator iter = m_mTemplateRules.find(sName);
	if(iter != m_mTemplateRules.end()){
		iter->second.push_back(sRule);
	}else{
		VectorString vs;
		
		vs.push_back(sRule);
		m_mTemplateRules.insert(std::make_pair(sName,vs));
	}
	
    return true;
}

bool Template::checkWholePartElementsNumber(const std::string& sRule, ErrorData& errData) const{
    size_t pos = sRule.find("\t(",1)+1;
    int count = 0;
    for(size_t i=0; i<pos; i++) {
        if(sRule[i] == '\t') {
            count++;
        }
    }
    if(count != WholePartElementsNumber) {
        ERROR_MESSAGE( "The elements number of whole part should be "+Utility::int2Str(WholePartElementsNumber)+"." )
    } else {
        return true;
    }
}

bool Template::getRuleNameFromString(const std::string& sRule, std::string& sName, ErrorData& errData) const{
	size_t nBeg = sRule.find("\t") + 1;
    size_t nEnd = sRule.find("\t",nBeg);
	
    sName = sRule.substr(nBeg,nEnd-nBeg);
	
    if(sName.empty()) {
        ERROR_MESSAGE( "The name of the template rule shouldn't be empty." )
    }
	
	return true;
}

bool Template::produce(const std::string& sLine, std::vector<std::string>& vRuleLine, ErrorData& errData) const{
	// read rule name
	std::string sRuleName;
	ERROR_TRACE( getRuleNameFromString(sLine,sRuleName,errData) )
	
	// read variable name and variable value
    std::string sVarName,sVarValue;
    ERROR_TRACE( getVariableFromString(sLine,sVarName,sVarValue,errData) )
	
    // read tag part
	std::string sTagPart;
	ERROR_TRACE( getTagPartFromString(sLine,sTagPart,errData) )
	
	// produce the normal rule
	acl::Utility::stringReplace(sRuleName,sVarName,"$(VarName)");
	std::map<std::string, VectorString >::const_iterator iter = m_mTemplateRules.find(sRuleName);
	if(iter == m_mTemplateRules.end()){
		ERROR_MESSAGE( "Template defination '"+sRuleName+"' must precede its use." )
	}
	
	const VectorString& vRule = iter->second;
	for(size_t i=0; i<vRule.size(); i++){
		std::string sRule = vRule[i];
		acl::Utility::stringReplace(sRule,"#TEMPLATE","#RULE");
		acl::Utility::stringReplace(sRule,"$(VarName)",sVarName);
		acl::Utility::stringReplace(sRule,"$(VarValue)",sVarValue);
		sRule += sTagPart;
		vRuleLine.push_back(sRule);
	}
	
	return true;
}

bool Template::getVariableFromString(const std::string& sRule, std::string& sVarName, std::string& sVarValue, ErrorData& errData) const{
	// get the begin position
	size_t nBeg = sRule.find("\t",sRule.find("\t")+1) + 1;
	if(nBeg == std::string::npos){
		ERROR_MESSAGE( "Get the begin position of the variable of the template call error." )
	}
	
	// get the end position
	size_t nEnd = sRule.find(")(");
	if(nEnd == std::string::npos){
		ERROR_MESSAGE( "Get the end position of the variable of the template call error." )
	}
	
	// read the variable string
	std::string sVariable = sRule.substr(nBeg,nEnd-nBeg);
	if(sVariable.empty()){
		ERROR_MESSAGE( "The variable of the template call shouldn't be empty." )
	}
	
	// read the name and the value of the variable
	size_t nPos = sVariable.find("=");
	if(sVariable.empty()){
		ERROR_MESSAGE( "Can't find '=' in the variable of the template call." )
	}
	sVarName = sVariable.substr(0,nPos);
	sVarValue = sVariable.substr(nPos+1);
	if(sVarName.empty() || sVarValue.empty()){
		ERROR_MESSAGE( "Either the ValName or the ValValue shouldn't be empty." )
	}
	
	return true;
}

bool Template::getTagPartFromString(const std::string& sRule, std::string& sTagPart, ErrorData& errData) const{
	// get the begin position
	size_t nBeg = sRule.find(")(") + 1;
	if(nBeg == std::string::npos){
		ERROR_MESSAGE( "Get the end position of the template call error." )
	}
	
	// get the end position
	size_t nEnd = sRule.length() - 1;
	if(sRule[nEnd] != ')'){
		ERROR_MESSAGE( "Get the end position of the template call error." )
	}
	
	sTagPart = sRule.substr(nBeg,nEnd-nBeg+1);
	if(sTagPart.empty()){
		ERROR_MESSAGE( "The tag part of the template call shouldn't be empty." )
	}
	
	return true;
}

}//namespace sae
