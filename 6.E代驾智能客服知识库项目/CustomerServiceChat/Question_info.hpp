#ifndef ___QUESTION_INFO__H_
#define ___QUESTION_INFO__H_

#define  SEMANTIC_SERVICE_FLAG  "serviceRobosay"

class Question_info{
public:
	Question_info(){
        init();
	}

	~Question_info(){}

	  void init(){
        m_id = "0";
        m_type = "";  /*0代表默认答案;1代表语义匹配的答案;2代表KeyWord匹配的答案*/
        m_target = "";
        m_source = "";
    }

	void set_id(const std::string& id){m_id = id;}
	std::string get_id()const{return m_id;}
    void append_id(const std::string& id){
        if(!m_id.empty()){
            m_id.append("|" + id);
        }else{
            m_id = id;
        }
    }

	void set_type(const std::string& type){m_type = type;}
	std::string get_type()const{return m_type;}

	void set_target(const std::string& target){m_target = target;}
	std::string get_target()const{return m_target;}

	void set_source(const std::string& source){m_source = source;}
	std::string get_source()const{return m_source;}

	std::string toStr() const {
		std::string sTarget = m_target;
		std::string type = m_type;

		if(m_type == Question_info::DefaultAnwser){
			type = "0";
		}
		else if(m_type == Question_info::KeyWordAnwser ){
			type = "2";
		}
		else
		{
            type = "1";			
		}
		if(!m_mSmtcWords.empty()){

			if(!sTarget.empty()){
				std::cout<<"Exception, both mSmtcWords and target have value!!!"<<std::endl;
			}
			const StringStringMap::const_iterator last = m_mSmtcWords.end();
			StringStringMap::const_iterator itr = m_mSmtcWords.begin();
			for(; last != itr; ++itr){
				sTarget.append(itr->first + ":" + itr->second + "|");
			}
		}
		
		std::string ret("<question_info>");
		ret.append("<id><![CDATA[" + m_id + "]]></id>");
		ret.append("<type><![CDATA[" + type + "]]></type>");
		ret.append("<target><![CDATA[" + sTarget + "]]></target>");
		ret.append("<source><![CDATA[" + m_source + "]]></source>");
		ret.append("</question_info>");
		return ret;
	}

	StringStringMap getSmtcWords() const {return m_mSmtcWords;}
	StringStringMap& getSmtcWordsRef(){return m_mSmtcWords;}
	void setSmtcWords(const StringStringMap& mSmtcWords){ m_mSmtcWords = mSmtcWords;}
	void insertSmtcWord(const std::string& l, const std::string& r){
		m_mSmtcWords.insert(std::make_pair(l, r));
	}

	/*判断语义标签中是否有业务标签*/
	std::string hasServiceSemantic(){
		StringStringMap::const_iterator it = m_mSmtcWords.begin();
		for(;it!=m_mSmtcWords.end();it++){
			if(it->second==SEMANTIC_SERVICE_FLAG){
				return it->first;
			}
		}

		return ""; 
	}
private:
	std::string m_id;
	std::string m_type;
	std::string m_target;
	std::string m_source;
	StringStringMap m_mSmtcWords;

public:
	static std::string DefaultAnwser;
	static std::string KeyWordAnwser;
	static std::string SemanticAnswer;
	static std::string SearchByPinyinAnwser;
	static std::string SearchByLevenshteinDistanceAnwser;	
	static std::string tagsAnwser;
	static std::string detailAnwser;

};


#endif
