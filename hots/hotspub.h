/********************************************************************
	created:	2005/11/05
	created:	5:11:2005   14:28
	filename: 	D:\proj\ksbass\ctbu\sctpub.h
	file path:	D:\proj\ksbass\ctbu
	file base:	sctpub
	file ext:	h
	author:		CHENYH
	
	purpose:	 这些函数是为了兼容大集中专门增加的函数：
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

//时间格式类型转换 "18:35:46" -> 183546
int TimeStr2Int(char *str);

//函数功能：检查机器时间是否在指定时间段内
BOOL CheckValidTime(char *sbegintime, char *sendtime);

void trim(char *s);

// 函数名: PrintMsg
// 编程  : 陈永华 2005-11-19 0:34:18
// 描述  : 选择性的将信息输出到本BU的日志文件中，建议直接使用WriteLog(1,...)
// 返回  : void 
// 参数  : char *fmt [IN]: 输出格式定义
// 参数  : ...
SCTPUB_API void PrintMsg(char *fmt, ...);

// 函数名: WriteLog
// 编程  : 陈永华 2005-11-19 0:40:56
// 描述  : 为了兼容集中交易, 写信息到日志文件中
// 返回  : void 
// 参数  : int level [IN]: 写入日志的错误级别，作如下转换 -1:89000; 0:8900; 1:1900; 2:890; 其他不变
// 参数  : char *fmt [IN]: 日志信息格式定义
// 参数  : ... [IN]: 参数列表
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
