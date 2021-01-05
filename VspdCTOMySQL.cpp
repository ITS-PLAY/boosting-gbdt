
#include "VspdCTOMySQL.h"

VspdCToMySQL::VspdCToMySQL()
{
}
VspdCToMySQL::~VspdCToMySQL()
{
}

//��ʼ������
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


//��ѯ����

string VspdCToMySQL::SelectData(char * SQL,int Cnum,char * Msg)

{

	MYSQL_ROW m_row; 

	MYSQL_RES *m_res; 

	char sql[2048]; 

	sprintf_s(sql,SQL); 

	int rnum = 0;

	char rg = '@';//�и���

	char cg = '?';//�ֶθ���



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

//��ѯ����

string VspdCToMySQL::SelectData(char * SQL,int Cnum,char * Msg,int *rnum)

{

	MYSQL_ROW m_row; 

	MYSQL_RES *m_res; 

	char sql[2048]; 

	sprintf_s(sql,SQL); 

	int rnumtemp=0;

	char rg = '@';//�и���

	char cg ='?';//�ֶθ���



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




//��������

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



//��������

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



//ɾ������

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

//�������ݱ�
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


//�ر����ݿ�����

void VspdCToMySQL::CloseMySQLConn()

{

	mysql_close(&mysql); 

}

