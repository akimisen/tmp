/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2004-2-7 15:36:16
  �汾: V1.00 
  ˵��: �����sqc������ص�ȫ�ֱ����ͺ�������
  2004-9-9 14:22:26 : Ϊ���ܹ�֧�ֶ����ݿ����Ӷ������޸ģ�
 ***********************************************************************************/
#ifndef _DB2SQC_H
#define _DB2SQC_H

#include "cpack.h"
#include "mypub.h"
#include "bupub.h"
#include "pass.h"
#define  SC_OK       0
#define  SC_NOTFOUND 100  // No data found 

// maximum SQL Server identifier length
//#define SQLID_MAX 30

typedef struct 
{
   char szLogin[33];     // SQL Server login ID
   char szPassword[33];  // SQL Server password
   char szServer[33];    // SQL Server name
   char szDatabase[33];  // SQL Server database to use
      
   int bConnected;            // are we connected to SQL Server?  1 as Connected, 0 as Disconnected
   
   // ���һ�δ���ʱ��Ĵ�����Ϣ��
   long lErrorCode;        // = SQLCODE  The SQLCODE field contains the negative SQL Server error code (the ESQL/C standard requires that error codes be negative). 
   long lErrorNumber;      // = SQLERRD1 The SQLERRD1 field contains the error number
   char szErrorMsg[80];    // The SQLERRMC field contains the text of the error message. 
   char szErrorState[6];   // SQLSTATE codes return values that are error, warning, or "no data found" messages. 
} ST_SQLDB;


//extern ST_SQLDB g_SQLDB;

#ifdef __cplusplus
extern "C" {
#endif
//////////////////////////////////////////////////////////////////////
// Ŀǰֻ���ǵ�һ���ݿ�����

void SQLErrorHandler();
void SQLInitialize(ST_SQLDB *pSQLDB);
void SQLExit();

// ������: SQLConnectToServer
// ���  : ������ 2004-2-7 23:24:49
// ����  : ����g_SqlDB�ж����SQL���ݿ����Ӳ�������������
// ����  : long   0: ��ʾ���ӳɹ�������������ʧ�ܣ����g_SqlDB�е�SQL����ʧ����Ϣ
int SQLConnectToServer();
int  SQLIsConnectOK();
void SQLDisconnect1();

int RollBack();
int Commit();

void PrintErrMsg(char *appMsg,struct sqlca * pSqlca,int line,char *file);

#ifdef _IN_SQC_
// install Embedded SQL for C error handler

/* ****** Updated by CHENYH at 2004-7-1 9:35:44 ****** 
EXEC SQL WHENEVER SQLERROR CALL SQLErrorHandler();

#define SQLSTATE (char *)(sqlca->sqlstate)
*/

#endif

#ifdef __cplusplus
}
#endif

#endif
