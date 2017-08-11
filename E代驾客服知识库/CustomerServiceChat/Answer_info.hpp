#ifndef ___ANSWER_INFO__H_
#define ___ANSWER_INFO__H_

#include "Common/lib4/Utility.hpp"

class Answer_info{
public:
	Answer_info(){
        init();
    }

	~Answer_info(){}

    void init(){
        m_id = "0";
        m_way = "";
        m_answer = "";
    }

	void set_way(const std::string& way){m_way = way;}
	std::string get_way()const{return m_way;}

	void set_id(const std::string& id){m_id = id;}
	std::string get_id()const{return m_id;}

	void set_answer(const std::string& answer){
		m_answer = answer;
		acl::Utility::stringReplace(m_answer, "_CDATA_BEG", "<![CDATA[");
		acl::Utility::stringReplace(m_answer, "_CDATA_END", "]]>");
	}
	std::string get_answer() const{return m_answer;}
	std::string getAnswerContent()
	{
		std::string sAnswerContent;
		if(isXmlAnswer(m_answer)){
            robosay::XmlParser newXml;
			if(false == newXml.parse(m_answer))
			{
				return "";
			}
	
			sAnswerContent = newXml.getXPathValue("/answer/content");
        }else{
            sAnswerContent = m_answer;
        }
	
		return sAnswerContent;
	}
	void setAnswerContent(const std::string& answerContent)
	{
		std::string sAnswerContent;
		if(isXmlAnswer(m_answer)){
            robosay::XmlParser newXml;
			std::string flagStr("<content>");
			size_t nBeg = m_answer.find(flagStr);
			if(nBeg == std::string::npos) return;
				
			size_t nEnd = m_answer.find("</content>",nBeg);
			if(nEnd == std::string::npos) return;

			sAnswerContent = "<![CDATA[" + answerContent + "]]>";
			m_answer = m_answer.substr(0,nBeg+flagStr.size()) + sAnswerContent + m_answer.substr(nEnd);
        }else{
           m_answer = answerContent;
        }
	}	
	
    std::string toStr()  const{
        std::string ret("<answer_info><way>");
			
        if(!m_way.empty()){
            ret.append(m_way + "</way>");
        }else{
            ret.append("0</way>");
        }
        /*if(isXmlAnswer(m_answer)){
			acl::Utility::stringReplace(m_answer, "_CDATA_END", "]]>");
			acl::Utility::stringReplace(m_answer, "_CDATA_BEG", "<![CDATA[");
            ret.append(m_answer);
        }else{
            ret.append("<answer><type>text</type>");
            ret.append("<content><![CDATA[" + m_answer+ "]]></content></answer>");
        }
		*/
        ret.append("<answer>");		
  	    ret.append("<tag><![CDATA[]]></tag>");		
  	    ret.append("<content><![CDATA[]]></content>");

		ret.append("<title><![CDATA[]]></title>");
		ret.append("</answer>");
        ret.append("</answer_info>");
        return ret;
    }

	void SetTitle(const std::string& title) { m_title = title; }
	std::string GetTitle()const { return m_title; }

private:
    bool isXmlAnswer(const std::string& sAnswer) const{
		return (!sAnswer.empty() && sAnswer[0] == '<' && sAnswer[sAnswer.length()-1] == '>') ? true : false;
	}

private:
	std::string m_way;
	std::string m_id;
	mutable std::string m_answer;
	std::string m_title;
};

#endif
