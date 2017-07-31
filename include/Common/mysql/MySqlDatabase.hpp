#pragma once
#include <Common/lib4/String.hpp>
#include <mysql++.h>

/**************************************************************************
//mysql++参考文档
//http://tangentsoft.net/mysql++/doc/html/refman/annotated.html
//安装mysql API : sudo apt-get install libmysql++-dev

备注：
如果遇到如下错误：
/usr/include/mysql++/qparms.h: 在构造函数‘mysqlpp::SQLParseElement::SQLParseElement(std::string, char, signed char)’中:
/usr/include/mysql++/qparms.h:253:2: 错误： 年代错误的旧式基类初始值设定 [-fpermissive]
/usr/include/mysql++/qparms.h:252:11: 错误： 对没有基类的‘mysqlpp::SQLParseElement’的无名初始化
/usr/include/mysql++/qparms.h:253:2: 错误： expected primary-expression before ‘)’ token
/usr/include/mysql++/qparms.h:253:2: 错误： expected ‘{’ before ‘__null’

是由于 src/SemanticAnalysis/RuleAnalysis/objio.h  中的宏 option  和 mysql++/qparms.h中的SQLParseElement的变量option冲突
解决方法：

在使用此库的相关.cpp中添加如下语句：
#undef option 

*************************************************************************/

namespace acl
{

class MySQLQuery  
{   
public:
    MySQLQuery():m_curSelRow(0){}
  
    ~MySQLQuery(){}
  
public:  
    //获取查询到的行数
    u_long GetRowNum()const {  return ( u_long )m_res.num_rows(); }
    //获取查询到的列数
    size_t GetFieldsNum()const { return m_res.num_fields(); }
	
	////////////////////////////////////////////////////////////////////////////
	///GetXXXField接口是配合遍历接口使用，都是获取当前遍历行的相应数据
	///begin
  
	/************************************************************************
	/*功能：获取当前int类型的数据
	/*参数说明：
	/*	int nField : 查询的列索引，从0开始
	/*	int nNullValue : 当查询失败时，返回该值，默认为-1，可自行传参设置
	/*返回值：int 查询到的结果
	/*备注:
	************************************************************************/
	int GetIntField(int nField, int nNullValue=-1)const
	{  
		if (!IsValidField(nField)) { return nNullValue; }
	  
		return (int)m_res[ m_curSelRow ][ nField ]; 
	}
	
	/************************************************************************
	/*功能：获取当前int类型的数据
	/*参数说明：
	/*	const acl::String& szField: 查询的列名称
	/*	int nNullValue : 当查询失败时，返回该值，默认为-1，可自行传参设置
	/*返回值：int 查询到的结果
	/*备注:
	************************************************************************/	  
	int GetIntField(const acl::String& szField, int nNullValue=-1)const
	{  
		if (!IsValidField(szField)) {  return nNullValue; } 
	  
		return m_res[m_curSelRow][szField.c_str()];  
	}  

	long long GetLongField( int nField, long long nNullValue=-1)  const
	{  
		if (!IsValidField(nField)) { return nNullValue; }
	  
		return m_res[ m_curSelRow ][ nField ];  
	}  

	long long GetLongField(const acl::String& szField, long long nNullValue=-1)const
	{  
		if (!IsValidField(szField)) {  return nNullValue; }  
	  
		return m_res[ m_curSelRow ][ szField.c_str()];
	}  

	double GetDoubleField( int nField, double fNullValue=0.0)const
	{  
		if (!IsValidField(nField)) { return fNullValue; }
	  
		return m_res[ m_curSelRow ][ nField ];  
	}  

	double GetDoubleField(const acl::String& szField, double fNullValue=0.0)const  
	{  
		if (!IsValidField(szField)) {  return fNullValue; }  
		return m_res[ m_curSelRow ][ szField.c_str()];  
	}  

	std::string GetStringField( int nField, const std::string& szNullValue="")const 
	{  
		if (!IsValidField(nField)) { return szNullValue; }
		return std::string(m_res[m_curSelRow][nField]); 
	}  

	std::string GetStringField(const acl::String& szField, const std::string& szNullValue="")const
	{  
		if (!IsValidField(szField)) { return szNullValue; }  
		return std::string(m_res[m_curSelRow][szField.c_str()]);  
	}
	
	///end
	//////////////////////////////////////////////////////////////////////////////////////
	
public:
	//遍历下一行
	void ToNext() { m_curSelRow++; }
	//是否遍历完成
	bool IsTraversalFinished() { return m_curSelRow >= m_res.size(); }
	//重置遍历接口，重新遍历
	void ResetTraversal() { m_curSelRow = 0; }
	
	void ClearRes()
	{  
		m_res.clear();
		m_fields.clear();
		ResetTraversal();
	}	
public:
	//要想更多的使用查询结果，可使用此接口获取 mysqlpp::StoreQueryResult
    const mysqlpp::StoreQueryResult& GetQueryResultPlr()const{ return m_res; }
	
private:	
	void SetStoreQueryResult(const mysqlpp::StoreQueryResult& res)
	{ 
		m_res = res;
		m_fields.clear();
		int nCol = GetFieldsNum();  
		for (int i=0; i<nCol; ++i)  
		{
			m_fields.insert(m_res.field_name(i));
		}
	}

private:
	bool IsValidField(const acl::String& szField)const
	{
		if (szField.empty() || m_res.empty()) { return false; }
		
		acl::String strField(szField);
		strField.toLower();
		
		return m_fields.end() != m_fields.find(strField);
	}
	
	bool IsValidField(int nField)const
	{
		if (m_res.empty()) { return false; }
		
		if (nField >= GetFieldsNum()) { return false; }
		
		return true;	
	}

private:
    size_t m_curSelRow;
    mysqlpp::StoreQueryResult m_res;
	std::set<std::string> m_fields;
	
	friend class MySqlDatabase;
};

///////////////////////////////////////////////////////////////////////////////////////////
///
class MySqlDatabase
{  
public:
    MySqlDatabase():m_conn(false){} 
    ~MySqlDatabase(){m_conn.disconnect();}

public: 
	/************************************************************************
	/*功能：打开数据库
	/*参数说明：
	/*	const std::string& dbName : 数据库名称
	/*	const std::string& host : 数据库地址
	/*	const std::string& user : 数据库用户名称
	/*	const std::string& passwd : 数据库密码
	/*返回值：成功true, 否则 false
	/*备注:
	************************************************************************/ 
	bool OPen(const std::string& dbName, const std::string& host, const std::string& user, const std::string& passwd, unsigned int uPort=0) 
	{
		if (m_conn.connected()) { return false; }
		
		// 首先设置字符集   如果你不设置的话到时候存进去的utf8格式会乱码  
		if (!m_conn.set_option(new mysqlpp::SetCharsetNameOption("utf8"))) { return false; }
	  
		// 连接数据库  
		if (!m_conn.connect(dbName.c_str(), host.c_str(), user.c_str(), passwd.c_str(),uPort)) {return false; }
	 
		return true;
	}
	
	/************************************************************************
	/*功能：执行SQL语句
	/*参数说明：
	/*	const std::string& sql : sql语句
	/*返回值：成功true, 否则 false
	/*备注:
	************************************************************************/
	bool ExecSQL(const std::string& sql)  
	{  
		if (!m_conn.connected()) { return false; }
		mysqlpp::Query query = m_conn.query(); 
	 
		if (!query.exec(sql)) { return false;}
	 
		return true;
	} 
	
	/************************************************************************
	/*功能：执行查询数据库的sql语句
	/*参数说明：
	/*	const std::string& sql : sql查询语句
	/*	MySQLQuery& pQuery : 查询结果存放到对象中，使用该对象获取结果
	/*返回值：成功true, 否则 false
	/*备注:
	************************************************************************/	
	bool QuerySQL(const std::string& sql, MySQLQuery& pQuery)
	{  
		if (!m_conn.connected()) { return false; }
		mysqlpp::Query query = m_conn.query();
		// 将sql语句执行，查询到的结果在query.store()里 
		query << sql;

		// 将结果赋值给结果对象  
		pQuery.SetStoreQueryResult(query.store());
		return true;  
	}

	void Close()
	{
		if (!m_conn.connected()) { return ; }

		m_conn.disconnect();
	}
	
private:  
    mysqlpp::Connection m_conn;  // 建立一个连接对象 
};

}
