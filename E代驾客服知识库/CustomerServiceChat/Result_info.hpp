#ifndef ___RESULT_INFO__H_
#define ___RESULT_INFO__H_

#include "KB_info.hpp"
#include "Question_info.hpp"
#include "Answer_info.hpp"
#include "Robot_info.hpp"
#include "Relate_info.hpp"

class Result_info{
public:
	Result_info(){ m_iScore = 0; }

	~Result_info(){}

    void init(){
		m_iScore = 0;
        m_kb.init();
        m_question.init();
        m_answer.init();
        m_robot.init();
        m_relate.init();
    }

    std::string toXml(robosay::XmlParser& xmlRequest,const std::string& answerInfo=""){
        // chat info
        std::string sXml("<?xml version=\"1.0\" encoding=\"utf-8\"?><chat xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">");
        sXml.append("<way>" + xmlRequest.getXPathValue("//chat/way") + "</way>");
        sXml.append("<message_id>" + xmlRequest.getXPathValue("//chat/message_id") + "</message_id>");
        sXml.append("<from_user>" + xmlRequest.getXPathValue("//chat/to_user") + "</from_user>");
        sXml.append("<to_user>" + xmlRequest.getXPathValue("//chat/from_user") + "</to_user>");

        //1. kb_info
        sXml.append(m_kb.toStr());

        //2. question_info
        sXml.append(m_question.toStr());

        //3. answer_info
        if(!answerInfo.empty()){
       		sXml.append(answerInfo);
    	}else{
        	sXml.append(m_answer.toStr());
		}

        //4. robot_info
        sXml.append(m_robot.toStr());

        //5. relate_info
        sXml.append(m_relate.toStr());

        sXml.append("</chat>");

        return sXml;
    }

	int 			m_iScore;	//when keyword search, it's match sorce, otherwise is zero
	KB_info			m_kb;
	Question_info 	m_question;
	Answer_info		m_answer;
	Robot_info		m_robot;
	Relate_info		m_relate;

};

#endif
