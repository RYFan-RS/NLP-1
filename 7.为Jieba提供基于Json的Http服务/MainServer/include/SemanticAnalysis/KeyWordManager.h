#ifndef __SAE__KEY__WORD_MANAGER_FZ__
#define __SAE__KEY__WORD_MANAGER_FZ__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32

#define __ENCRYPT_DATA__
#ifdef __ENCRYPT_DATA__
#include "lib4/Encoder.hpp"
#endif

#include "ErrorData.h"
#include "objio.h"

#include "TypeDefine.h"
#include <map>
#include <set>
#include <string>
#include <boost/bind/bind.hpp>

namespace sae
{

//一条语义对应的所有关键字集合
class CKeyWordSet
{
public:
	void AddKeyWord(const std::string& strKeyWord)
	{
		(void)m_keyWordSets.insert(strKeyWord);
	}

	//BackCallFunc  function prototype define: 
	//only one para : (const string& strElem)
	//return value : void
	template<class BackCallFunc>
	void ForEach(BackCallFunc backCallFunc)const
	{
		std::set<std::string>::const_iterator iter = m_keyWordSets.begin();
		while (iter != m_keyWordSets.end()){
			(void)backCallFunc(*iter);
			++iter;
		}
	}

public:
    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) 
    {
        int iSize = 0;
		
        Format &format = stream.get_format();
        format.input_start_array(iSize);
		
        for(int i = 0; i < iSize; i++) 
		{
			std::string str;
			stream.simple(str);
#ifdef __ENCRYPT_DATA__
            AddKeyWord(acl::Encoder::decode(str));
#else
            AddKeyWord(str);
#endif
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) 
    {
        int iSize = m_keyWordSets.size();
		std::set<std::string>::iterator iter;
  
        Format &format = stream.get_format();
        format.output_start_array(iSize);
		
        for(iter=m_keyWordSets.begin(); iter!=m_keyWordSets.end(); ++iter) 
		{
#ifdef __ENCRYPT_DATA__
			stream.simple(acl::Encoder::encode(*iter));
#else
			stream.simple(*iter);
#endif
        }
		
        format.output_end_array();
    }

private:
	std::set<std::string> m_keyWordSets; 
};

class CKeyWordManager
{
public:
	//bool : true successful, false : fail
	bool Parse(const LINE& line, ErrorData& errData);

	//bool : true successful, false : fail
	bool FindKeyWordSetByTagIndexNum(int iTagIndexNum, const CKeyWordSet*& pKeyWordSet)const;

//test begin
public:
	void Output(std::ostream& ofs=std::cout)const
	{
		std::map<int, CKeyWordSet>::const_iterator iter = m_keyWords.begin();
		while (iter != m_keyWords.end()) {
			ofs << "begin : " << iter->first << "\n";
			iter->second.ForEach(bind(&CKeyWordManager::HandleOneKeyWordSet,this,_1,&ofs));
			ofs << "end : " << iter->first << "\n";
			++iter;  
		}
	}	

	void HandleOneKeyWordSet(const std::string& str,std::ostream* pOfs=&std::cout)const
	{
		if (NULL == pOfs) { return; }
		*pOfs << str << "\n";
	}

//test end
	

public:
    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) 
    {
        int iSize = 0;
		
        Format &format = stream.get_format();
        format.input_start_array(iSize);
		
        for(int i = 0; i < iSize; i++) 
		{
			int indexNum = 0;
			stream.simple(indexNum);
			stream.content(m_keyWords[indexNum]);
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) 
    {
        int iSize = m_keyWords.size();
		std::map<int, CKeyWordSet>::iterator iter;
  
        Format &format = stream.get_format();
        format.output_start_array(iSize);
		
        for(iter=m_keyWords.begin(); iter!=m_keyWords.end(); ++iter) 
		{
			stream.simple(iter->first);
			stream.content(iter->second);
        }
		
        format.output_end_array();
    }	
	
private:
	std::map<int, CKeyWordSet> m_keyWords;//first: tag 序号; second: 关键字集合
};

}
#endif

