#ifndef __ROBOSAY__XML_PARSER_H__
#define __ROBOSAY__XML_PARSER_H__
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/tree.h>
#include <set>
#include <map>
#include <vector>
#include <string>
#include "Common/lib4/String.hpp"
#include "Common/lib4/Vector.hpp"
#include "Common/lib4/FileName.hpp"
#include "Common/lib4/Definations.hpp"

namespace robosay
{

class XmlParser
{
public:
	XmlParser(){
		xmlKeepBlanksDefault(0);
		xmlIndentTreeOutput = 1;// indent with \n 
		m_doc = NULL;
	}
	
	~XmlParser(){
		xmlFreeDoc(m_doc);
	}
	
	bool parse(const std::string& s){
		m_doc = xmlParseMemory(s.c_str(),s.length());
		if(NULL == m_doc){
			PRINT("Parse "<<s<<" error.")
			return false;
		}
		
		m_sFileName = "[string]";
		
		return true;
	}
	
	bool read(const acl::FileName& sFileName){
		m_doc = xmlReadFile(sFileName.c_str(),"UTF-8",XML_PARSE_NOBLANKS);
		if(NULL == m_doc){
			PRINT("Parse "<<sFileName<<" error.")
			return false;
		}
		
		m_sFileName = sFileName;
		
		return true;
	}
	
	void write(const acl::FileName& sFileName){
		xmlSaveFormatFileEnc(sFileName.c_str(), m_doc, "UTF-8", 1);
	}
	
	bool reload(const acl::FileName& sFileName){
		xmlFreeDoc(m_doc);
		m_doc = NULL;
		return read(sFileName);
	}
	
	std::vector<xmlNodePtr> getXPathNodeVec(const std::string& sXPath) const{
		std::vector<xmlNodePtr> vResult;
		
		xmlXPathObjectPtr result = getXPath(sXPath); 
		if (result != NULL){
			for (int i=0; i < result->nodesetval->nodeNr; i++) {
				vResult.push_back(result->nodesetval->nodeTab[i]);
			}
			xmlXPathFreeObject(result);
		}
		
		return vResult;
	}
	
	std::vector<std::string> getXPathValueVec(const std::string& sXPath) const{
		std::vector<std::string> vResult;
		
		xmlXPathObjectPtr result = getXPath(sXPath); 
		if (result != NULL){
			for (int i=0; i < result->nodesetval->nodeNr; i++) {
				if(result->nodesetval->nodeTab[i]->xmlChildrenNode != NULL)
					vResult.push_back((char *)xmlNodeListGetString(m_doc, result->nodesetval->nodeTab[i]->xmlChildrenNode, 1));
				else
					vResult.push_back("");
			}
			xmlXPathFreeObject(result);
		}
		
		return vResult;
	}
	
	std::string getXPathValue(const std::string& sXPath) const{
		std::string sResult;
		
		xmlXPathObjectPtr result = getXPath(sXPath); 
		if (result != NULL){
			xmlChar* sMidResult = xmlNodeListGetString(m_doc, result->nodesetval->nodeTab[0]->xmlChildrenNode, 1);
			if(sMidResult != NULL){
				sResult = (char*)sMidResult;
			}
			xmlXPathFreeObject(result);
		}
		
		return sResult;
	}
	
	std::set<std::string> getXPathValueSet(const std::string& sXPath) const{
		std::set<std::string> seResult;
		
		xmlXPathObjectPtr result = getXPath(sXPath); 
		if (result != NULL){
			for (int i=0; i < result->nodesetval->nodeNr; i++) {
				seResult.insert((char *)xmlNodeListGetString(m_doc, result->nodesetval->nodeTab[i]->xmlChildrenNode, 1));
			}
			xmlXPathFreeObject(result);
		}
		
		return seResult;
	}
	
	xmlXPathObjectPtr getXPath(const std::string& sXPath) const{
		xmlXPathContextPtr context = xmlXPathNewContext(m_doc); //创建一个XPath上下文指针
		if (context == NULL){  
			return NULL;
		}
		
		xmlXPathObjectPtr result = xmlXPathEvalExpression(BAD_CAST(sXPath.c_str()), context); //查询XPath表达式，得到一个查询结果
		xmlXPathFreeContext(context); //释放上下文指针
		if (result == NULL){
			return NULL;
		}
		
		if (xmlXPathNodeSetIsEmpty(result->nodesetval)){ //检查查询结果是否为空
			xmlXPathFreeObject(result);
			return NULL;
			//return result;
		}else{
			return result;
		}
	}
	
	void setXPathValue(const std::string& sXPath, const std::string& sValue){
		xmlXPathObjectPtr result = getXPath(sXPath);
		
		if (result != NULL){
			for (int i=0; i < result->nodesetval->nodeNr; i++) {
				xmlNodeSetContent(result->nodesetval->nodeTab[i],BAD_CAST sValue.c_str());
			}
			xmlXPathFreeObject(result);
		}
	}
	
	void setXPathCDataValue(const std::string& sXPath, const std::string& sValue){
		xmlXPathObjectPtr result = getXPath(sXPath);
		
		xmlNodePtr ptr = xmlNewCDataBlock(m_doc,BAD_CAST sValue.c_str(),sValue.length());
		
		if (result != NULL){
			for (int i=0; i < result->nodesetval->nodeNr; i++) {
				xmlNodeSetContent(result->nodesetval->nodeTab[i],BAD_CAST "");
				xmlAddChild( result->nodesetval->nodeTab[i], ptr );
			}
			xmlXPathFreeObject(result);
		}
	}
	
	
	
	std::string getValue(xmlNodePtr p) const{
		xmlChar* sMidResult = xmlNodeListGetString(m_doc, p->xmlChildrenNode, 1);
		if(sMidResult == NULL){
			return "";
		}else{
            std::string sRet((char*)sMidResult);
            xmlFree(sMidResult);
			return sRet;
		}
	}
	
	std::string getName(xmlNodePtr p) const{
		std::string s((char *)p->name);
		return s;
	}
	
	xmlNodePtr getRootNode(const char* sName) const{
		xmlNodePtr rootNode = xmlDocGetRootElement(m_doc);
		
		if (NULL == rootNode){
			PRINT("KB file '"<<m_sFileName<<"' is an empty xml file.");
			return NULL;
		}
		
		if (xmlStrcmp(rootNode->name, BAD_CAST sName)){
			PRINT("The root node of KB file '"<<m_sFileName<<"' is not '"<<sName<<"'.");
			return NULL;
		}
		
		return rootNode;
	}
	
	xmlNodePtr getFirstChildNode(xmlNodePtr pntNode, const char* sName) const{
		xmlNodePtr curNode = pntNode->xmlChildrenNode;
		while(curNode != NULL){
			if(curNode->type == XML_ELEMENT_NODE && !xmlStrcmp(curNode->name, BAD_CAST sName)){
				return curNode;
			}
			curNode = curNode->next;
		}
		return NULL;
	}
	
	xmlNodePtr getFirstChildNode(xmlNodePtr pntNode) const{
		xmlNodePtr curNode = pntNode->xmlChildrenNode;
		while(curNode != NULL){
			if(curNode->type == XML_ELEMENT_NODE){
				return curNode;
			}
			curNode = curNode->next;
		}
		return NULL;
	}
	
	std::string getAttribute(xmlNodePtr curNode, const char* sName) const{
		std::string sValue;
		
		xmlAttrPtr attrPtr = curNode->properties;
		while (attrPtr != NULL){
			if (!xmlStrcmp(attrPtr->name, BAD_CAST sName)){
				xmlChar* szAttr = xmlGetProp(curNode,BAD_CAST sName);
				sValue = (char*)szAttr;
				xmlFree(szAttr);
                break;
			}
			attrPtr = attrPtr->next;
		}
		
		return sValue;
	}

	bool setAttribute(xmlNodePtr curNode, const char* sName, const char* sVal) const{
        xmlSetProp(curNode, BAD_CAST sName, BAD_CAST sVal);
		return true;
	}

	bool setXPathAttribute(const std::string sXPath, const char* sName, const char* sVal) const{
		xmlXPathObjectPtr result = getXPath(sXPath);
		
		if (result != NULL){
			for (int i=0; i < result->nodesetval->nodeNr; i++) {
				setAttribute(result->nodesetval->nodeTab[i], sName, sVal);
			}
			xmlXPathFreeObject(result);
		}

		return true;
	}

	bool setXPathAttributes(const std::string sXPath, const std::map<std::string,std::set<std::string> >& mAttrVals) const{
		xmlXPathObjectPtr result = getXPath(sXPath);
		
		if (result != NULL){
			for (int i=0; i < result->nodesetval->nodeNr; i++) {
				setAttributes(result->nodesetval->nodeTab[i], mAttrVals);
			}
			xmlXPathFreeObject(result);
		}

		return true;
	}

    std::string StringSet2Str(const std::set<std::string>& seVals)const{
        const std::set<std::string>::const_iterator last = seVals.end();
        std::set<std::string>::const_iterator itr = seVals.begin();
		std::string sRet;
        for(; last != itr; ++itr){
            sRet += *itr + ",";            
        }    
        if(!seVals.empty()){
            sRet.resize(sRet.length() - 1);        
        }
        return sRet;
    }

	bool setAttributes(xmlNodePtr curNode, const std::map<std::string,std::set<std::string> >& mAttrVals) const{
		
        const std::map<std::string,std::set<std::string> >::const_iterator last = mAttrVals.end();
        std::map<std::string,std::set<std::string> >::const_iterator itr = mAttrVals.begin();
		for(; last != itr; ++itr){
            std::string sVal = StringSet2Str(itr->second);
            setAttribute(curNode, itr->first.c_str(), sVal.c_str());
        }
    }
	
	std::map<std::string,std::set<std::string> > getAttributes(xmlNodePtr curNode) const{
		std::map<std::string,std::set<std::string> > mAttribute;
		
		xmlAttrPtr attrPtr = curNode->properties;
		while(attrPtr != NULL){
			acl::String sName = (char*)attrPtr->name;
			acl::String sValue = (char*)attrPtr->children->content;
			acl::Vector<std::string> vValue = sValue.separate(",");
			mAttribute.insert(std::make_pair(sName,vValue.toSet()));
			attrPtr = attrPtr->next;  
		} 
		
		return mAttribute;
	}
	
	std::string toStr() const{
		xmlChar* buf;
		int len;
		xmlDocDumpFormatMemoryEnc(m_doc, &buf, &len, "UTF-8", 1);
		acl::String sRet((char*)buf);
		xmlFree(buf);
		
		// 删除空行
		std::vector<acl::String> v = sRet.separate_("\n");
		std::vector<acl::String> newV;
		for(size_t i=0; i<v.size(); i++){
			for(size_t j=0; j<v[i].length(); j++){
				if(v[i][j] != ' '){
					int nPos = v[i].find_last_not_of(' ');
					if(nPos != std::string::npos){
						newV.push_back(v[i].substr(0,nPos+1));
					}else{
						newV.push_back(v[i]);
					}
					break;
				}
			}
		}
		sRet = "";
		for(size_t i=0; i<newV.size(); i++){
			sRet += newV[i] + "\n";
		}
		
		return sRet;
	}
	
	bool insertNode(std::string fatherNode, std::string sonNode, std::string sContent){
		std::vector<xmlNodePtr> vNode = getXPathNodeVec(fatherNode);
		if(!vNode.empty()){
			for(size_t i=0; i < vNode.size(); i++){
				// 写入
				xmlNodePtr son = xmlNewNode(NULL, BAD_CAST(sonNode.c_str()));
				if(!sContent.empty()){
					xmlAddChild(son, xmlNewText(BAD_CAST(sContent.c_str())));
				}
				xmlAddChild(vNode[i],son);
			}
		}else{
			return false;
		}
		
		return true;
	}
	
	void resetChildrenNodes(xmlNodePtr cur){
		if(cur != NULL){
			cur = cur->xmlChildrenNode;
			
			while (cur != NULL){
				if(cur->type == XML_TEXT_NODE){
					xmlChar* sValue = xmlNodeListGetString(m_doc, cur, 1);
					if(sValue != NULL){
						xmlNodeSetContent(cur,BAD_CAST "");
					}
				}
				resetChildrenNodes(cur);
				cur = cur->next;
			}
		}
		
		return;
	}
	
	void deleteXPathNode(const std::string& sXPath){
		std::vector<xmlNodePtr> vNode = getXPathNodeVec(sXPath);
		
		if(!vNode.empty()){
			for(size_t i=0; i < vNode.size(); i++){
				deleteNode(vNode[i]);
			}
		}
	}
	
	void deleteNode(xmlNodePtr curNode){
		if(curNode != NULL){
			xmlNodePtr chdNode = curNode->xmlChildrenNode;
			while (chdNode != NULL){
				deleteNode(chdNode);
				chdNode = chdNode->next;
			}
			xmlNodePtr tempNode = curNode->next;
			xmlUnlinkNode(curNode);
			xmlFreeNode(curNode);
			curNode = tempNode;
		}
	}
	
	void deleteCommentNodes(xmlNodePtr curNode){
		while(curNode != NULL){
			if(curNode->type == XML_COMMENT_NODE){
				xmlNodePtr tempNode = curNode->next;
				xmlUnlinkNode(curNode);
				xmlFreeNode(curNode);
				curNode = tempNode;
				continue;
			}
			
			xmlNodePtr chdNode = curNode->xmlChildrenNode;
			while (chdNode != NULL){
				deleteCommentNodes(chdNode);
				chdNode = chdNode->next;
			}
			
			curNode = curNode->next;
		}
	}
private:
	xmlDocPtr m_doc;
	acl::FileName m_sFileName;
};

}//namespace robosay

#endif //__ROBOSAY__XML_PARSER_H__
