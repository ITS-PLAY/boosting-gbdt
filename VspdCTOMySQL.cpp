
#include "VspdCTOMySQL.h"

VspdCToMySQL::VspdCToMySQL()
{
}
VspdCToMySQL::~VspdCToMySQL()
{
}

//初始化数据
int VspdCToMySQL::ConnMySQL(char *host,char * port ,char * Db,char * user,char* passwd,char * charset,char * Msg)
{
	if( mysql_init(&mysql) == NULL ) 
	{ 
		
		strcpy_s(Msg,sizeof(char)*1000,"inital mysql handle error");
		return 1; 
	}     
	if (mysql_real_connect(&mysql,host,user,passwd,Db,0,NULL,0) == NULL) 
	{ 
		strcpy_s(Msg,sizeof(char)*1000,"Failed to connect to database: Error");
	
		return 1; 
	} 
	if(mysql_set_character_set(&mysql,"GBK") != 0)
	{
		strcpy_s(Msg,sizeof(char)*1000,"mysql_set_character_set Error");
		
		return 1;

	}

	return 0;

}


//查询数据

string VspdCToMySQL::SelectData(char * SQL,int Cnum,char * Msg)

{

	MYSQL_ROW m_row; 

	MYSQL_RES *m_res; 

	char sql[2048]; 

	sprintf_s(sql,SQL); 

	int rnum = 0;

	char rg = '@';//行隔开

	char cg = '?';//字段隔开



	if(mysql_query(&mysql,sql) != 0) 

	{ 

		strcpy_s(Msg, sizeof(char) * 1000, "select ps_info Error");

		return ""; 

	} 

	m_res = mysql_store_result(&mysql); 



	if(m_res==NULL) 

	{ 

		strcpy_s(Msg, sizeof(char) * 1000, "select username Error");

		return ""; 

	} 

	string str("");

	while(m_row = mysql_fetch_row(m_res)) 

	{ 

		for(int i = 0;i < Cnum;i++)

		{

			str += m_row[i];

			str += cg;

		}

		str += rg;              

		rnum++;

	} 



	mysql_free_result(m_res); 



	return str;

}

//查询数据

string VspdCToMySQL::SelectData(char * SQL,int Cnum,char * Msg,int *rnum)

{

	MYSQL_ROW m_row; 

	MYSQL_RES *m_res; 

	char sql[2048]; 

	sprintf_s(sql,SQL); 

	int rnumtemp=0;

	char rg = '@';//行隔开

	char cg ='?';//字段隔开



	if(mysql_query(&mysql,sql) != 0) 

	{ 
		strcat_s(Msg,sizeof(char)*1000,"select ps_info Error");
		return ""; 

	} 

	m_res = mysql_store_result(&mysql); 



	if(m_res==NULL) 

	{ 
		strcat_s(Msg,sizeof(char)*1000,"select username Error");

		return ""; 

	} 

	string str("");

	while(m_row = mysql_fetch_row(m_res)) 

	{ 

		for(int i = 0;i < Cnum;i++)

		{
			if (m_row[i]==NULL)
			{
				strcat_s(Msg,sizeof(char)*1000,"read field null Error");
				return ""; 
			}
			str += m_row[i];

			str += cg;

		}

		str += rg;              

		rnumtemp++;

	} 



	mysql_free_result(m_res); 


	*rnum=rnumtemp;
	return str;

}




//插入数据

int VspdCToMySQL::InsertData(char * SQL,char * Msg)

{

	char sql[2048]; 

	sprintf_s(sql,SQL); 

	if(mysql_query(&mysql,sql) != 0) 

	{ 
		strcpy_s(Msg,sizeof(char)*1000,"Insert Data Error");
		

		return 1; 

	} 

	return 0;

}



//更新数据

int VspdCToMySQL::UpdateData(char * SQL,char * Msg)

{

	char sql[2048]; 

	sprintf_s(sql,SQL); 

	if(mysql_query(&mysql,sql) != 0) 

	{ 
		strcpy_s(Msg,sizeof(char)*1000,"Update Data Error");

		return 1; 

	} 

	return 0;

}



//删除数据

int VspdCToMySQL::DeleteData(char * SQL,char * Msg)

{

	char sql[2048]; 

	sprintf_s(sql,SQL); 

	if(mysql_query(&mysql,sql) != 0) 

	{ 

		
		strcpy_s(Msg,sizeof(char)*1000,"Delete Data error");
		return 1; 

	} 

	return 0;

}

//创建数据表
int VspdCToMySQL::CreateTable(char *SQL, char *Msg) {

	char sql[2018];

	sprintf(sql, SQL);

	if (mysql_query(&mysql, sql) != 0)
	{
		strcpy_s(Msg, sizeof(char) * 1000, "Create Table error");
		return 1;
	}

	return 0;
}


//关闭数据库连接

void VspdCToMySQL::CloseMySQLConn()

{

	mysql_close(&mysql); 

}

