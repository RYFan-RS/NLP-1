#include "DataStorage.h"
#include "Utility.h"
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32

namespace sae
{

void DataStorage::distributeDataManagerPointer()
{
    m_normalRuleManager.setDataManagerPointer(&m_dataManager);
    m_entryManager.setNormalRuleManagerPointer(&m_normalRuleManager,&m_dataManager);
}

bool DataStorage::initAfterSerializing()
{
    distributeDataManagerPointer();
    m_dataManager.getWordManager().recordIndexOfLastSerializedWord();

    return true;
}

bool DataStorage::parse(const std::vector<FILE>& files, std::vector<ErrorData>& vErrData)
{
    size_t i,j;

    //init data and pointers
    distributeDataManagerPointer();

    //parse the files
    for(i=0; i<files.size(); i++) {
        std::vector<ErrorData> vMidErr;

        if(parseDataFile(files[i],vMidErr) == false){
			for(j=0; j<vMidErr.size(); j++) {
				vMidErr[j].m_fileIndex = i;
				vMidErr[j].errorTrace(__FILE__,__LINE__);
				vErrData.push_back(vMidErr[j]);
			}

			return false;
		}
    }

    //check index refer in execute rules
    std::vector<ErrorData> vErrorDatasInChecking;
    if(m_dataManager.checkReferOfIndexAndExecuteMethods(vErrorDatasInChecking) == false) {
        for(j=0; j<vErrorDatasInChecking.size(); j++) {
            vErrorDatasInChecking[j].errorTrace(__FILE__,__LINE__);
            vErrData.push_back(vErrorDatasInChecking[j]);
        }
        return false;
    }

    //make the entries map
    m_entryManager.makeEntries();

    return true;
}

bool DataStorage::divideKeyAndValue(std::string rule, std::string& key, std::string& value) const{
    rule = Utility::getStringBetween(rule, "(", ")");
    Utility::removeSpaceNear(rule,"=");
    std::vector<std::string> v = Utility::divide(rule,"=");
    if(v.size() != 2) {
        return false;
    }
    int ret = Utility::hasEmptyCell(v);
    if( ret > -1 ) {
        return false;
    }

    key = getRuleNameStr(rule,"=");
    value = v[1];

    return true;
}

bool DataStorage::ParseDefineRule(const std::string& line, std::map<int, std::vector<int> >& mUsrWords){
    std::string key, value;
    bool bRet = divideKeyAndValue(line, key, value);
    if(!bRet || key.empty() || value.empty()){
        return false;
    }

    WordManager& wordManager = m_dataManager.getWordManager();
    const int nKeyID = wordManager.find(key);
    if(nKeyID == BidirectionalMap::ErrorCode){
        return false;
    }

    std::vector<std::string> val = Utility::divide(value,",");

    int ret = Utility::hasEmptyCell(val);
    if( ret > -1 ) {
        return false;
    }

    std::vector<int>& vWordIDs = mUsrWords[nKeyID];
    const std::vector<std::string>::const_iterator last = val.end();
    std::vector<std::string>::const_iterator itr = val.begin();
    for(; last != itr; ++itr){
       vWordIDs.push_back(wordManager.insertFind(*itr));
    }

    return true;
}

bool DataStorage::parseUsrDic(const acl::String& file){
    std::vector<std::string> vUsdDics = file.separate(";");
    const std::vector<std::string>::const_iterator last = vUsdDics.end();
    std::vector<std::string>::const_iterator itr = vUsdDics.begin();
    std::map<int, std::vector<int> > mUsrWords;
    for(; last != itr; ++itr){
        acl::File file(*itr);
        for(size_t i=0; i<file.size(); i++) {
            if(file[i].empty()) continue;
            bool bParseResult = true;
            ErrorData errData;
            std::string sType = getLineType(file[i], errData);
            if(sType.empty()) {
                return false;
            } else if(sType == "#DEFINE") {
             bParseResult = ParseDefineRule(file[i], mUsrWords);
    		} else {
             return false;
            }

            if(bParseResult == false) {
                return false;
            }
        }
    }

    if(!mUsrWords.empty()){
        bool bRet = m_normalRuleManager.addUsrDic(mUsrWords);
        if(!bRet){
            return false;
        }
    }

    return true;
}

void DataStorage::makeEntries(){

	m_dataManager.getWordManager().recordIndexOfLastSerializedWord();
    //make the entries map
    m_entryManager.makeEntries();
}

bool DataStorage::addUsrWord(const std::map<std::string, std::set<std::string> >* pWords){
    if(!pWords){
        return true;
    }
    std::map<int, std::vector<int> > mUsrWords;
    WordManager& wordManager = m_dataManager.getWordManager();
    const std::map<std::string, std::set<std::string> >::const_iterator last = pWords->end();
    std::map<std::string, std::set<std::string> >::const_iterator itr = pWords->begin();
    for(; last != itr; ++itr){
        const int nKeyID = wordManager.find(itr->first);
        if(nKeyID == BidirectionalMap::ErrorCode){
            return false;
        }
        std::vector<int>& seWIds = mUsrWords[nKeyID];
        const std::set<std::string>& seWords = itr->second;
        const std::set<std::string>::const_iterator l = seWords.end();
        std::set<std::string>::const_iterator i = seWords.begin();
        for(; l!=i; ++i){
            seWIds.push_back(wordManager.insertFind(*i));
        }
    }

    if(!mUsrWords.empty()){
        bool bRet = m_normalRuleManager.addUsrDic(mUsrWords);
        if(!bRet){
            return false;
        }
    }

    return true;
}

bool DataStorage::parseDataFile(const FILE& file, std::vector<ErrorData>& vErrData)
{
    //read the line in the file
    bool returnFlag = true;

    for(size_t i=0; i<file.m_file.size(); i++) {
        if(file.m_file[i].m_line.empty()) continue;
        ErrorData errData;
        bool bParseResult = true;

        std::string sType = getLineType(file.m_file[i],errData);
        if(sType.empty()) {
            errData.m_lineIndex = i;
            errData.errorTrace(__FILE__,__LINE__);
            vErrData.push_back(errData);
            returnFlag = false;
        } else if(sType == "#INDEX") {
            bParseResult = m_dataManager.getIndexManager().parse(file.m_file[i],errData);
        } else if(sType == "#DEFINE" || sType == "#COND_DEFINE" || sType == "#KEY_DEFINE") {
            bParseResult = m_dataManager.getDefineManager().parse(file.m_file[i],sType,errData);
        } else if(sType == "#RULE") {
            bParseResult = m_normalRuleManager.parse(file.m_file[i],&m_dataManager,errData);
        } else if(sType == "#TAG") {
            bParseResult = m_dataManager.getTagManager().parse(file.m_file[i],errData);
        } else if(sType == "#EXECUTION") {
            bParseResult = m_dataManager.getExecuteManager().parse(file.m_file[i],&m_dataManager,errData);
        } else if(sType == "#COMMET") {
            //do nothing here, maybe comment will be recorded somewhere, but not now
        } else if(sType == "#TEMPLATE") {
			bParseResult = m_dataManager.getTemplate().parse(file.m_file[i],errData);
		} else if(sType == "@TEMPLATE") {
			bParseResult = m_normalRuleManager.parseTemplate(file.m_file[i],&m_dataManager,errData);
		} else if (sType == "#KEYWORD") {
			bParseResult = m_dataManager.GetKeyWordManager().Parse(file.m_file[i],errData);
		}else {
            errData.m_lineIndex = i;
            errData.m_errMsg = Message::msg008(sType);
            errData.errorTrace(__FILE__,__LINE__);
            vErrData.push_back(errData);
            returnFlag = false;
        }

        if(bParseResult == false) {
            errData.m_lineIndex = i;
            errData.errorTrace(__FILE__,__LINE__);
            vErrData.push_back(errData);
            returnFlag = false;
        } else if(!errData.m_warnMsg.empty()) {
            errData.m_lineIndex = i;
            errData.warnTrace(__FILE__,__LINE__);
            vErrData.push_back(errData);
        }
    }

	//test 
	//m_dataManager.GetConstKeyWordManager().Output();

    return returnFlag;
}

std::string DataStorage::getRuleTypeStr(const std::string& line) const
{
    std::string s;
    int pos = line.find("\t");
    if(pos == (int)std::string::npos) return "";
    if(line[0] == '(') {
        s = line.substr(1,pos-1);
    } else {
        s = line.substr(0,pos);
    }
    return s;
}

std::string DataStorage::getRuleNameStr(const std::string& line, const std::string& end/*="\t"*/) const
{
    std::string s;
    int pos = line.find("\t");
    if(pos == (int)std::string::npos) return "";
    int pos2 = line.find(end, pos+1);
    if(pos2 == (int)std::string::npos) return "";
    s = line.substr(pos+1,pos2-(pos+1));
    return s;
}

std::string DataStorage::getLineType(const LINE& line, ErrorData& errData){
    return getLineType(line.m_line, errData);
}

std::string DataStorage::getLineType(const std::string& line, ErrorData& errData)
{
    if(line.empty()) return "";

    if(line[0] == '/') {
        return "#COMMET";
    }

    if(checkHeadAndTailBrackets(line) == false) {
        errData.m_errMsg = Message::msg009();
        errData.errorTrace(__FILE__,__LINE__);
        return "";
    }

    if(checkPairBrackets(line) == false) {
        errData.m_errMsg = Message::msg010();
        errData.errorTrace(__FILE__,__LINE__);
        return "";
    }

    std::string sType = getRuleTypeStr(line);
    if(sType.empty()) {
        errData.m_errMsg = Message::msg011();
        errData.errorTrace(__FILE__,__LINE__);
        return "";
    }

    return sType;
}
bool DataStorage::checkHeadAndTailBrackets(const std::string& line) const
{
    if((line[0] == '(' && line[line.length()-1] == ')')
       || (line[0] != '(' && line[line.length()-1] != ')')) {
        return true;
    }
    return false;
}

bool DataStorage::checkPairBrackets(const std::string& line) const
{
    bool isPaired = true;
    for(size_t i=0; i<line.length(); i++) {
        if(line[i] == '(') {
            if(i>0) {
                if(Utility::isTranslatedChar(line,i) == false) {
                    Utility::switchBoolTag(isPaired);
                }
            } else {
                Utility::switchBoolTag(isPaired);
            }
        } else if(line[i] == ')') {
            if(Utility::isTranslatedChar(line,i) == false) {
                Utility::switchBoolTag(isPaired);
            }
        }
    }

    return isPaired;
}

void DataStorage::dump(std::ostream& ofs) const
{
    m_dataManager.dump(ofs);
    ofs<<"=== NORMAL RULE: ==========================="<<std::endl;
    m_normalRuleManager.dump(ofs);
    ofs<<"=== ENTRY    MAP: ==========================="<<std::endl;
    m_entryManager.dump(ofs);
}

}//namespace sae
