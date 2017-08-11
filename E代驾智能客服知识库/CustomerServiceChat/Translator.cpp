#include "Translator.h"

namespace aisms
{

bool TransData::read(const acl::Line& line){
	acl::Line sWordPart;
	
	size_t nCondPos = line.find("::");
	if(nCondPos != std::string::npos){
		if(nCondPos == 0) return false;
		acl::Line sConditionPart = line.substr(0, nCondPos);
		vCondition = sConditionPart.separate(',');
		sWordPart = line.substr(nCondPos+2);
	}
	
	if(sWordPart.empty()){
		vWord = line.separate(",");
	}else{
		vWord = sWordPart.separate(",");
	}
	
	for(size_t i=0; i<vWord.size(); i++){
		if(vWord[i].empty()) return false;
		if(vWord[i][vWord[i].length()-1] == ')'){
			size_t nPos = vWord[i].find('(');
			if(nPos == std::string::npos) return false;
			std::string sAttribute = vWord[i].substr(nPos+1,vWord[i].length()-nPos-2);
			vWord[i] = vWord[i].substr(0,nPos);
			mAttribute_WordIdx.insert(std::make_pair(sAttribute,i));
		}
	}
	
	return true;
}

bool TransData::hasCondition(const std::string& sCondition) const{
	for(size_t i=0; i<vCondition.size(); i++){
		if(vCondition[i] == sCondition){
			return true;
		}
	}
	return false;
}

bool Translator::init(const acl::FileName& sXmlFile){
	std::string sFileFullPath;
	#if 0
	KBConfig config;
	if(config.init(sXmlFile) == false) {
		return false;
	}

	if(config.has("SemanticSearch") == false) {
		return false;
	}
	
	acl::FileName sTransFile = config.get("SemanticSearch","translation_dic");
	#endif
	
	if(!sXmlFile.empty()) {
#if 0
//#ifdef USE_ENCODE_DECODE
		sFileFullPath = sXmlFile.getPath() + "/common" + "Translation.dat";
		if(readEncryptionFile(sFileFullPath) == false) {
#else
		sFileFullPath = sXmlFile + "/common/" + "Translation.dic";
		if(readNormalFile(sFileFullPath) == false) {
#endif
			PRINT("Translation dictionary init with file \""<<sFileFullPath<<"\" failed!")
			return false;
		}
		std::cerr<<"  |-Dictionary ["<<sFileFullPath<<"] loaded."<<std::endl;
	}
	
	return true;
}


void Translator::setMixParserPointer(mxpsr::Parser *pMixParser){
	m_pMixParser = pMixParser;
}


const std::string& Translator::translate(const std::string& sDicName, const std::string& sIn) const{
	std::map<std::string,std::multimap<std::string,size_t> >::const_iterator iter;
	std::pair<std::multimap<std::string,size_t>::const_iterator,std::multimap<std::string,size_t>::const_iterator> pIter;
	
	iter = m_mDicIdx.find(sDicName);
	if(iter == m_mDicIdx.end()){
		return m_sEmpty;
	}
	
	pIter = iter->second.equal_range(sIn);
	if(pIter.first == pIter.second){
		return m_sEmpty;
	}
	
	return m_vTransData[pIter.first->second].vWord[0];
}

std::string Translator::translateWithParser(const std::string& sDicName, const std::string& sIn) const{
	std::map<std::string,std::multimap<std::string,size_t> >::const_iterator iter;
	std::pair<std::multimap<std::string,size_t>::const_iterator,std::multimap<std::string,size_t>::const_iterator> pIter;
	acl::String sRet;

	if(NULL == m_pMixParser)
	{	
		return sRet;		
	}
	
	iter = m_mDicIdx.find(sDicName);
	if(iter == m_mDicIdx.end()){
		return m_sEmpty;
	}
	
	pIter = iter->second.equal_range(sIn);
	if(pIter.first == pIter.second){
		return m_sEmpty;
	}
	
	std::vector<std::string> vVal = acl::Utility::separate(m_vTransData[pIter.first->second].vWord[0],";");
	for(size_t i=0; i<vVal.size(); i++){
		if(i != 0) sRet += ";";
		sRet += m_pMixParser->dynamicParse(vVal[i],aisms::gOption.sDate)[0].getWord();
	}
	
	return sRet;
}

const std::string& Translator::translateOrgName(const std::string& sDicName, const std::string& sIn) const{
	std::map<std::string,std::multimap<std::string,size_t> >::const_iterator iter;
	std::pair<std::multimap<std::string,size_t>::const_iterator,std::multimap<std::string,size_t>::const_iterator> pIter;
	
	iter = m_mDicIdx.find(sDicName);
	if(iter == m_mDicIdx.end()){
		return m_sEmpty;
	}
	
	pIter = iter->second.equal_range(sIn);
	if(pIter.first == pIter.second){
		return m_sEmpty;
	}
	
	return m_vTransData[pIter.first->second].vWord[1];
}

const std::string& Translator::translateWithCondition(const std::string& sDicName, const std::string& sIn, const std::string& sCondition) const{
	std::map<std::string,std::multimap<std::string,size_t> >::const_iterator iter;
	std::pair<std::multimap<std::string,size_t>::const_iterator,std::multimap<std::string,size_t>::const_iterator> pIter;
	std::multimap<std::string,size_t>::const_iterator i;
	
	iter = m_mDicIdx.find(sDicName);
	if(iter == m_mDicIdx.end()){
		return m_sEmpty;
	}
	
	pIter = iter->second.equal_range(sIn);
	if(pIter.first == pIter.second){
		return m_sEmpty;
	}
	
	for(i=pIter.first; i!=pIter.second; ++i){
		if(m_vTransData[i->second].hasCondition(sCondition)){
			return m_vTransData[i->second].vWord[0];
		}
	}
	
	return m_sEmpty;
}

bool Translator::toFile(const std::string& sFilename) const{
	acl::File file;
	std::map<std::string,std::multimap<std::string,size_t> >::const_iterator iDic;
	std::multimap<std::string,size_t>::const_iterator iter;
	
	for(iDic = m_mDicIdx.begin(); iDic != m_mDicIdx.end(); ++iDic){
		file.push_back("[" + iDic->first + "]");
		const std::multimap<std::string,size_t>& m = iDic->second;
		for(iter=m.begin(); iter!=m.end(); ++iter){
			const std::vector<std::string>& v = m_vTransData[iter->second].vWord;
			std::string sLine;
			for(size_t i=0; i<v.size(); i++){
				if(i != 0) sLine += ",";
				sLine += v[i];
			}
			file.push_back(sLine);
		}
	}
	
	file.write(sFilename);
	
	return true;
}

bool Translator::readEncryptionFile(const std::string& sFilename){
	acl::FileEncoder fileEncoder;
	std::vector<std::string> orgFile;
	acl::File file;
	
	if(fileEncoder.decodeFile(sFilename,orgFile) == false){
		PRINT("Can't decode dictionary file ["<<sFilename<<"].");
		return false;
	}
	
	// 将文件拷贝一次，虽然费时间，但是只有一次
	for(size_t i=0; i<orgFile.size(); i++) {
		file.push_back(orgFile.at(i));
	}
	
	return initData(file);
}

bool Translator::readNormalFile(const std::string& sFilename){
	acl::File file;

	if( file.read(sFilename) == false) {
		PRINT("Can't open dictionary file ["<<sFilename<<"].");
		return false;
	}
	
	return initData(file);
}

bool Translator::initData(const acl::File& file){
	std::string sDicName;
	std::multimap<std::string,size_t> mDicIdx;
	
	for(size_t i=0; i<file.size(); i++){
		const acl::Line& line = file.at(i);
		if(line.empty() || line.isBeginWith("#")){
			continue;
		}else if(line.isBeginWith("[") && line.at(line.length()-1) == ']'){
			if(!sDicName.empty()){
				m_mDicIdx.insert(std::make_pair(sDicName,mDicIdx));
			}
			if(line != "[EOF]"){
				sDicName = line.substr(1,line.length()-2);
			}
			mDicIdx.clear();
		}else{
			TransData data;
			
			if(data.read(line) == false){
				PRINT("Read line "<<i+1<<" failed! Skip this line.")
				continue;
			}
			
			m_vTransData.push_back(data);
			size_t nIndex = m_vTransData.size()-1;
			
			for(size_t j=0; j<data.vWord.size(); j++){
				mDicIdx.insert(std::make_pair(data.vWord[j],nIndex));
			}
		}
	}
	
	return true;
}
	
}// namespace aisms

