#ifndef __SAE__MEAN_WORDS_STRING_INTERFACE__
#define __SAE__MEAN_WORDS_STRING_INTERFACE__

#include "ErrorData.h"
#include "DataManager.h"
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32


namespace sae
{

class MeanWordsStringInterface
{
private:
    std::string m_originalString;
    std::vector<int> m_tags;
    std::vector<std::string> m_vWordStrings;

public:
    const std::string& getOriginalString() {
#ifdef MEAN_DEBUG
        ssDebug << m_originalString;
        acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL0);
#endif //MEAN_DEBUG
        return m_originalString;
    }
    const std::vector<int>& getTags(DataManager* pDataManager) {
#ifdef MEAN_DEBUG
        for(int i=0; i<m_tags.size(); i++) {
            ssDebug << "Tag " << Utility::int2Str(i) << ": " << pDataManager->getTagManager().find(m_tags[i]) << "\n";
        }
        acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL0);
#endif //MEAN_DEBUG
        return m_tags;
    }
    const std::vector<std::string>& getWordStrings() {
#ifdef MEAN_DEBUG
        for(int i=0; i<m_vWordStrings.size(); i++) {
            ssDebug << "WordString " << Utility::int2Str(i) << ": " << m_vWordStrings[i] << "\n";
        }
        acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL0);
#endif //MEAN_DEBUG
        return m_vWordStrings;
    }

    void setOriginalString(const std::string& originalString) {
        m_originalString = originalString;
    }
    void setTags(const std::vector<int>& vTags) {
        m_tags = vTags;
    }
    void setWordStrings(const std::vector<std::string>& vWordStrings) {
        m_vWordStrings = vWordStrings;
    }

    std::string makeWordString(const int& hinshi, std::vector<int>& vTags, std::string& input, std::string& basic, std::string& basic_org) {
        std::string wordString;
        wordString += Utility::int2Str(hinshi) + "|[";
        for(unsigned int i=0; i<vTags.size(); i++) {
            if(i>0) wordString += ",";
            wordString += Utility::int2Str(vTags[i]);
        }
        wordString += "]|" + input + "|" + basic + "|" + basic_org;
        return wordString;
    }

    void addWordString(const int& hinshi, std::vector<int>& vTags, std::string& input, std::string& basic, std::string& basic_org) {
        std::string wordString = makeWordString(hinshi,vTags,input,basic,basic_org);
        m_vWordStrings.push_back(wordString);
    }

    const int getWordHinshi(const int& index) {
        int hinshi = -1;
        if(m_vWordStrings.size() > 0) {
            std::vector<std::string> vWordParts = Utility::divide(m_vWordStrings[index],"|");
            if(vWordParts.size() > 0) {
                if(Utility::isNumber(vWordParts[0]) && !vWordParts[0].empty()) {
                    hinshi = atoi(vWordParts[0].c_str());
                }
            }
        }
        return hinshi;
    }

    const std::vector<int> getWordTags(const int& index) {
        std::vector<int> vIntTags;
        if(m_vWordStrings.size() > 0) {
            std::vector<std::string> vWordParts = Utility::divide(m_vWordStrings[index],"|");
            if(vWordParts.size() > 1) {
                if(vWordParts[1]!="" && vWordParts[1]!="[]") {
                    if(vWordParts[1][0]=='[' && vWordParts[1][vWordParts[1].size()-1]==']' && !Utility::isTranslatedChar(vWordParts[1],vWordParts[1].size()-1)) {
                        std::vector<std::string> vStringTags = Utility::divide(vWordParts[1].substr(1,vWordParts[1].size()-2),",");
                        for(size_t j=0; j<vStringTags.size(); j++) {
                            if(Utility::isNumber(vStringTags[j]) && !vStringTags[j].empty()) {
                                vIntTags.push_back(atoi(vStringTags[j].c_str()));
                            }
                        }
                    }
                }
            }
        }
        return vIntTags;
    }

    const std::string getWordInput(const int& index) {
        std::string input;
        if(m_vWordStrings.size() > 0) {
            std::vector<std::string> vWordParts = Utility::divide(m_vWordStrings[index],"|");
            if(vWordParts.size() > 2) {
                input = vWordParts[2];
            }
        }
        return input;
    }

    const std::string getWordBasic(const int& index) {
        std::string basic;
        if(m_vWordStrings.size() > 0) {
            std::vector<std::string> vWordParts = Utility::divide(m_vWordStrings[index],"|");
            if(vWordParts.size() > 3) {
                basic = vWordParts[3];
            }
        }
        return basic;
    }

    const std::string getWordBasicOrg(const int& index) {
        std::string basic_org;
        if(m_vWordStrings.size() > 0) {
            std::vector<std::string> vWordParts = Utility::divide(m_vWordStrings[index],"|");
            if(vWordParts.size() > 4) {
                basic_org = vWordParts[4];
            }
        }
        return basic_org;
    }


    void dump(DataManager* pDataManager) {
        std::cout<<"Original String: "<<m_originalString<<std::endl;
        std::cout<<"Tags: ";
        for(unsigned int i=0; i<m_tags.size(); i++) {
            if(i>0) std::cout<<", ";
            std::cout<<pDataManager->getTagManager().find(m_tags[i]);
        }
        std::cout<<std::endl;
        for(unsigned int j=0; j<m_vWordStrings.size(); j++) {
            std::cout<<m_vWordStrings[j]<<std::endl;
        }
    }
};

}//namespace sae

#endif //__SAE__MEAN_WORDS_STRING_INTERFACE__
