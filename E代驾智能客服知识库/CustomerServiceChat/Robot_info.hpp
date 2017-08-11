#ifndef ___ROBOT_INFO__H_
#define ___ROBOT_INFO__H_

class Robot_info{
public:
	Robot_info(){
        init();
	}

	~Robot_info(){}

	void init(){
        m_context = "0";
        m_beg_time = "";
        m_end_time = "";
    }

	void set_context(const std::string& context){m_context = context;}
	std::string get_context(){return m_context;}

	void set_end_time(const std::string& end_time){m_end_time = end_time;}
	std::string get_end_time(){return m_end_time;}

	void set_beg_time(const std::string& beg_time){m_beg_time = beg_time;}
	std::string get_beg_time(){return m_beg_time;}

	std::string toStr() const {
		std::string ret("<robot_info>");
		ret.append("<context><![CDATA[" + m_context + "]]></context><time>");
		ret.append("<begin><![CDATA[" + m_beg_time + "]]></begin>");
		ret.append("<end><![CDATA[" + m_end_time + "]]></end></time>");
		ret.append("</robot_info>");
		return ret;
	}

private:
	std::string m_context;
	std::string m_beg_time;
	std::string m_end_time;
};

#endif
