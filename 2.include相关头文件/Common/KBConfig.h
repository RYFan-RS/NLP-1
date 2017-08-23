#ifndef __AISMS__KB_CONFIG_H__
#define __AISMS__KB_CONFIG_H__
#include <iostream>
#include "libxml2/XmlParser.hpp"

namespace aisms
{

class KBConfig
{
public:
	bool init(const char* sFileName){
		if( m_xml.read(sFileName) == false ) {
			PRINT("Could not load KB file '"<<sFileName<<"'.");
			return false;
		}
		
		xmlNodePtr rootNode = m_xml.getRootNode("scml");
		
		if (NULL == rootNode){
			PRINT("The root node of KB file '"<<sFileName<<"' is not 'scml'.");
			return false;
		}
		
		m_configNode = m_xml.getFirstChildNode(rootNode,"config");
		
		if(NULL == m_configNode){
			PRINT("Can't find config node in KB file '"<<sFileName<<"'.");
			return false;
		}
		
		return true;
	}
	
	bool has(const char* sName) const{
		xmlNodePtr node = m_xml.getFirstChildNode(m_configNode,sName);
		
		return node == NULL ? false : true;
	}
	
	bool isValid(const char* sName) const{
		bool bValid = false;
		
		// 查找名字为sName的节点
		xmlNodePtr curNode = m_xml.getFirstChildNode(m_configNode,sName);
		if(curNode == NULL) return false;
		
		// 在节点内查找属性
		xmlAttrPtr attrPtr = curNode->properties;
		while (attrPtr != NULL)
		{
			if (!xmlStrcmp(attrPtr->name, BAD_CAST "valid"))
			{
				xmlChar* szAttr = xmlGetProp(curNode,BAD_CAST "valid");
				if(!xmlStrcmp(szAttr, BAD_CAST "true")){
					xmlFree(szAttr);
					bValid = true;
					break;
				}else{
					xmlFree(szAttr);
				}
			}
			attrPtr = attrPtr->next;
		}
		
		return bValid;
	}
	
	std::string get(const char* sPntName, const char* sName) const{
		// 查找名字为sName的节点
		xmlNodePtr pntNode = m_xml.getFirstChildNode(m_configNode,sPntName);
		if(pntNode == NULL) return "";
		
		xmlNodePtr node = m_xml.getFirstChildNode(pntNode,sName);
		if(node == NULL) return "";
		
		xmlChar* szKey = xmlNodeGetContent(node);
		std::string sValue((char*)szKey);
		xmlFree(szKey);
		
		return sValue;
	}
	
private:
	robosay::XmlParser m_xml;
	xmlNodePtr m_configNode;
};

}//namespace aisms

#endif //__AISMS__KB_CONFIG_H__
