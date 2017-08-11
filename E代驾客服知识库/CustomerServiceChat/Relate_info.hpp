#ifndef ___RELATE_INFO__H_
#define ___RELATE_INFO__H_

class Relate_info{
public:
	Relate_info(){}

	~Relate_info(){}

	void init(){
        m_questions.clear();
    }

	void set_questions(const std::vector<std::string>& questions){m_questions = questions;}
	std::vector<std::string> get_questions(){return m_questions;}

	std::string toStr() const {
		std::string ret("<relate_info>");
		const std::vector<std::string>::const_iterator last = m_questions.end();
		std::vector<std::string>::const_iterator itr = m_questions.begin();
		for(; last != itr; ++itr){
			ret.append("<question><![CDATA[" + *itr + "]]></question>");
		}
		ret.append("</relate_info>");
		return ret;
	}

private:
	std::vector<std::string>	m_questions;
};

#endif
