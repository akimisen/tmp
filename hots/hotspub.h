/********************************************************************
	created:	2005/11/05
	created:	5:11:2005   14:28
	filename: 	D:\proj\ksbass\ctbu\sctpub.h
	file path:	D:\proj\ksbass\ctbu
	file base:	sctpub
	file ext:	h
	author:		CHENYH
	
	purpose:	 ��Щ������Ϊ�˼��ݴ���ר�����ӵĺ�����
*********************************************************************/
#ifndef __SCTPUB__
#define __SCTPUB__

#ifdef SCTPUB_EXPORTS
#define SCTPUB_API __declspec(dllexport)
#else
#ifdef WIN_DLL
#define SCTPUB_API __declspec(dllimport)
#else
#define SCTPUB_API 
#endif
#endif


#ifdef __cplusplus
extern "C" {
#endif

void GetRequestHead(ST_PACKHEAD *phead);

//ʱ���ʽ����ת�� "18:35:46" -> 183546
int TimeStr2Int(char *str);

//�������ܣ�������ʱ���Ƿ���ָ��ʱ�����
BOOL CheckValidTime(char *sbegintime, char *sendtime);

void trim(char *s);

// ������: PrintMsg
// ���  : ������ 2005-11-19 0:34:18
// ����  : ѡ���ԵĽ���Ϣ�������BU����־�ļ��У�����ֱ��ʹ��WriteLog(1,...)
// ����  : void 
// ����  : char *fmt [IN]: �����ʽ����
// ����  : ...
SCTPUB_API void PrintMsg(char *fmt, ...);

// ������: WriteLog
// ���  : ������ 2005-11-19 0:40:56
// ����  : Ϊ�˼��ݼ��н���, д��Ϣ����־�ļ���
// ����  : void 
// ����  : int level [IN]: д����־�Ĵ��󼶱�������ת�� -1:89000; 0:8900; 1:1900; 2:890; ��������
// ����  : char *fmt [IN]: ��־��Ϣ��ʽ����
// ����  : ... [IN]: �����б�
SCTPUB_API void WriteLog(int level,char *fmt, ...);

SCTPUB_API int SetColEx(void *handle, ST_CPACK *ppack , ...);

int PutQx(void *handle, int DestNo, int MainFunction, char *queuename, ST_CPACK cpack, char *pMsg);

int PutQ(void *handle, int DestNo, int MainFunction, ST_CPACK cpack, char *pMsg);

int PutWTQ(void *handle, int DestNo, int MainFunction, ST_CPACK cpack, char *pMsg);

int PutCJHBQ(void *handle, int DestNo, int MainFunction, ST_CPACK cpack, char *pMsg);

int PutQ3(void *handle, int DestNo, int MainFunction, ST_CPACK cpack, char *pMsg);

int PutQ2(void *handle, char *comm_handle, ST_CPACK cpack, char *pMsg);

int PutQ2M(void *handle, char *comm_handle, char *databuf, int buflen, char *pMsg);

char Bit4_HexC(unsigned char bit4);

unsigned char HexC_Bit4(char cHex);

char *Data2HexString(void *pData, int len, char *oStr);

void GetTransHandle(void *handle, char *pTransHandle);

int mystrncpy(char *dest, const char *source, int dsize);

void MakeContract(char *pPrfx,long lNo,char *pContract,int iFixLen);

BOOL CanOpenAcc(char *sourcestr,char *deststr);

BOOL OFS_ConvertIden15to18(char *Scert_no15,char *Scert_no18);

void GetStrValue(char *instr,char sigstr,int order ,char *outstr);

int ResetFunctions();
int InDoProcessPack(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
int InCallFunctions(int fno, void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);

BOOL BAK_ACC_HIS(char *sTacode,char *sCustNo,char *sBranchCode,int *piRetCode,char *omsg);
////////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif

#undef SCTPUB_API 

#endif
