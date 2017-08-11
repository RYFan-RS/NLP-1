#include "Filter.h"

namespace robosay{
namespace base{


bool Filter::init(robosay::XmlParser& xml) 
{
	xmlNodePtr rootNode = xml.getRootNode("scml");
	if (NULL == rootNode) { return false; }
	
	xmlNodePtr sorterPtr = xml.getFirstChildNode(rootNode, "sorter");
	
	//如果没有sorter也是支持的
	if (NULL == sorterPtr) { return true; }

	xmlNodePtr semanticPtr = xml.getFirstChildNode(sorterPtr, "semantic");
	
	while (NULL != semanticPtr)
	{
		//std::cout << xml.getValue(semanticPtr) << std::endl;
		std::string strSortSmtc = xml.getValue(semanticPtr);
		if (strSortSmtc.empty()) { continue; }
		
		m_mFilter.insert(std::make_pair(strSortSmtc,""));
		
		semanticPtr = semanticPtr->next;
	}
	
	return true;
}

std::string Filter::toStr() const {
	std::string s;
	std::map<std::string,std::string>::const_iterator iter;
	bool bAddSepFlag = false;
	
	s += "<FLTR>";
	for(iter=m_mFilter.begin(); iter!=m_mFilter.end(); ++iter) {
		if(iter->second.empty()) continue;
		if(bAddSepFlag) s += ",";
		else bAddSepFlag = true;
		s += iter->first + "-" + iter->second;
	}
	s += "</FLTR>";
	
	return s;
}

bool Filter::fromStr(const std::string& s) {
	if(s.find("<FLTR>") != 0) return false;
	if(s.rfind("</FLTR>") != s.length()-7) return false;
	
	std::vector<std::string> vElement = acl::Utility::separate(s.substr(6,s.length()-13),",");
	
	for(size_t i=0; i<vElement.size(); i++){
		for(size_t j=0; j<vElement[i].length(); j++){
			if(vElement[i][j] == '-'){
				const std::string sKey = vElement[i].substr(0,j);
				const std::string sVal = vElement[i].substr(j+1);
				std::map<std::string,std::string>::iterator iter;
				iter = m_mFilter.find(sKey);
				if(iter == m_mFilter.end()){
					m_mFilter.insert(std::make_pair(sKey,sVal));
				}else{
					iter->second = sVal;
				}
				break;
			}
		}
	}
	
	m_bUpdated = false;
	
	return true;
}

void Filter::scanFilter(const std::map<std::string, std::string>& mSemanticVariable, const Translator& translator, std::string& sHotelRelated){
	std::map<std::string, std::string>::const_iterator i;
	std::map<std::string, std::string>::iterator iter;
	
	for(i=mSemanticVariable.begin(); i!=mSemanticVariable.end(); ++i) {
		if(i->first.find("[filter]") == 0){
			std::string sKey = i->first.substr(8);
			if(sKey == "Reset"){
				reset();
				m_bUpdated = true;
				return;
			}
			std::string sVal = i->second;
			iter = m_mFilter.find(sKey);
			if(iter != m_mFilter.end()){
				iter->second = sVal;
				if(sKey == "AreaRegion" || sKey == "CityRegion"){
					if(!sHotelRelated.empty()) sHotelRelated += ";";
					sHotelRelated += sKey + "-" + sVal;
				}
			}
			m_bUpdated = true;
		}
	}
}

void Filter::scanFilter(const std::map<std::string, std::string>& mSemanticVariable)
{
	std::map<std::string, std::string>::const_iterator i;
	std::map<std::string, std::string>::iterator iter;
	
	for(i=mSemanticVariable.begin(); i!=mSemanticVariable.end(); ++i) {
		if(i->first.find("[filter]") == 0){
			std::string sKey = i->first.substr(8);
			if(sKey == "Reset"){
				reset();
				m_bUpdated = true;
				return;
			}
			std::string sVal = i->second;
			iter = m_mFilter.find(sKey);
			if(iter != m_mFilter.end()){
				iter->second = sVal;
			}
			m_bUpdated = true;
		}
	}	
}

void Filter::scanRange(const std::map<std::string, std::string>& mSemanticVariable){
	std::map<std::string, std::string>::const_iterator i;
	acl::StringMap::iterator iSMap;
	
	for(i=mSemanticVariable.begin(); i!=mSemanticVariable.end(); ++i) {
		if(i->first.find("[range]") == 0){
			std::string sKey = i->first.substr(7,i->first.length()-8);
			char cRangeMark = i->first[i->first.length()-1];
			for(iSMap=m_mFilter.begin(); iSMap!=m_mFilter.end(); ++iSMap){
				if(iSMap->first == sKey){
					iSMap->second += cRangeMark;
				}
			}
		}
	}
}

void Filter::scan(const std::map<std::string, std::string>& mSemanticVariable, const Translator& translator, std::string& sHotelRelated) {
	scanFilter(mSemanticVariable,translator,sHotelRelated);
	scanRange(mSemanticVariable);
}

const acl::StringMap& Filter::getFilter() const{
	return m_mFilter;
}

void Filter::reset() {
	std::map<std::string,std::string>::iterator iter;

	for(iter=m_mFilter.begin(); iter!=m_mFilter.end(); ++iter) {
		iter->second = "";
	}
	
	m_bUpdated = false;
}

bool Filter::isUpdated() const{
	return m_bUpdated;
}

}//namespace base
}// namespace robosay
