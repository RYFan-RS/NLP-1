//#include <mysql/mysql.h>

#include <Common/mysql/MySqlDatabase.hpp>
#include <cstdio>
#include <iostream>
using namespace std;

acl::MySqlDatabase database;

void CreateTable()
{
	//创建一个表
	string strCreateTable = "CREATE TABLE person(id int(11), name varchar(255), age int(3)) character set utf8";
	if (!database.ExecSQL(strCreateTable))
	{
		cout << "create table fail" << endl;
	}	
}

void InsertData()
{
    // 第二部测试插入数据  
    if (!database.ExecSQL("insert into person(id,name,age) values(1,'李明',23)"))
    {
		cout << "database.ExecSQL fail" << endl;
    }
    if (!database.ExecSQL("insert into person(id,name,age) values(2,'张宏',26)"))
    {
		cout << "database.ExecSQL fail" << endl;
    }
    if (!database.ExecSQL("insert into person(id,name,age) values(3,'王华',21)"))
    {
		cout << "database.ExecSQL fail" << endl;
    }	
}

void TraversalData()
{
	acl::MySQLQuery mySqlQuery;  
	database.QuerySQL("select * from person", mySqlQuery ); 
	
	while (!mySqlQuery.IsTraversalFinished())
	{
		cout << mySqlQuery.GetLongField("Id") << "\t";  
		cout << mySqlQuery.GetStringField("NaMe") << "\t";  
		cout << mySqlQuery.GetIntField("age") << endl;  
		mySqlQuery.ToNext();
	}
	cout << "-----------------" << endl;
	mySqlQuery.ResetTraversal();
	while (!mySqlQuery.IsTraversalFinished())
	{
		cout << mySqlQuery.GetLongField(0) << "\t";  
		cout << mySqlQuery.GetStringField(1) << "\t";  
		cout << mySqlQuery.GetIntField(2) << endl;  
		mySqlQuery.ToNext();
	}	
}

int main(int argc, char *argv[])
{
    // 第一步连接数据库  
    if (!database.OPen("service", "localhost", "crawl", "505658"))
    {
		cout << "database.OPen fail" << endl;
		return 1;
    }
	
	//创建表
	CreateTable();
	
	//清空表中数据，不回滚
	//database.ExecSQL("truncate table person");
	
    // 第二部测试插入数据  
    InsertData();
	
	//遍历数据 
	TraversalData();
	
	//cout << "finished" << endl;
	return 0;
}


