#ifndef ___KB_INFO__H_
#define ___KB_INFO__H_

class KB_info{
public:
	KB_info(){}

	~KB_info(){}

    void init(){
        m_id = "0";
        m_type = "1";
        m_industry = "2";
        m_level = "3";
        m_business = "4";
    }

	void set_id(const std::string& id){m_id = id;}
	std::string get_id() const {return m_id;}

	void set_type(const std::string& type){m_type = type;}
	std::string get_type() const {return m_type;}

	void set_name(const std::string& name){m_name = name;}
	std::string get_name() const {return m_name;}

	void set_industry(const std::string& industry){m_industry = industry;}
	std::string get_industry() const {return m_industry;}

	void set_level(const std::string& level){m_level = level;}
	std::string get_level() const {return m_level;}

	void set_business(const std::string& business){m_business = business;}
	std::string get_business() const {return m_business;}

	std::string toStr() const {
		std::string ret("<kb_info>");
		ret.append("<id><![CDATA[" + m_id + "]]></id>");
		ret.append("<type><![CDATA[" + m_type + "]]></type>");
		ret.append("<name><![CDATA[" + m_name + "]]></name>");
		ret.append("<industry><![CDATA[" + m_industry + "]]></industry>");
		ret.append("<level><![CDATA[" + m_level + "]]></level>");
		ret.append("<business><![CDATA[" + m_business + "]]></business>");
		ret.append("</kb_info>");
		return ret;
	}

private:
	std::string m_id;
	std::string m_type;
	std::string m_name;
	std::string m_industry;
	std::string m_level;
	std::string m_business;
};

#endif
