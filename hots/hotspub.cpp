/********************************************************************
    created:	2006/08/16
    created:	16:8:2006   10:30
    filename: 	D:\proj\ksbass\zszq\zqbu\zspub.cpp
    file path:	D:\proj\ksbass\zszq\zqbu
    file base:	zspub
    file ext:	cpp
    author:		CHENYH
    
    purpose:	 本文件是为了招商证券系统中替换如下几部分代码：
            cics.css\svr.css\
*********************************************************************
 * 修改日期: 2022-01-19
 * 修改人员: 李向东
 * 修改描述: 修正高版本gcc下部分字符串拷贝内存重叠问题
 * 备注信息: SECRZRQ-10964
 * 版本信息: 3.22.3.3
 * 专题信息: SECREQ-15050 【海通】【融资融券】融资融券支持linux7.6后台改造
 * --------------------------------------------------------------*/
#include "stdafx.h"
#include <stdio.h>
#include "cpack.h"
#include "mypub.h"
#include "bupub.h"
#include "svrlink.h"
#include "hotspub.h"
#include "comstru.h"
#include <sql.h>
#include "pubc.h"
#include "math.h"
#include "bdefine.h"

#ifndef WIN32
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/io.h>
#include <sys/stat.h>
#endif


#define MAXCOMMSIZE 8000
#define RESERVELEN 100
#define MAXMSG1 32000 /* 队列元素的最大大小 */
#define FILE_MODE 1
#define QUEUE_MODE 0
#define WAIT_WRITE 0
#define NOWAIT_WRITE 1

// 在LINUX下可能MSG_R、MSG_W没有定义
#ifndef IPC_NOWAIT
#define IPC_NOWAIT 1
#endif
#ifndef IPC_WAIT
#define IPC_WAIT 0
#endif
#ifndef MSG_W
#define MSG_W 0000200
#endif
#ifndef MSG_R
#define MSG_R 0000400
#endif

extern int g_iDebug;
extern TBDefine g_InXBDefines[]; // 在具体的函数定义部分，如BDFTest.cpp -> BDFunc.inl中
char g_emp_code[7] = "";  //added by ly@20170912 for SECJZJY-20921
char g_menu_no[256] = ""; //added by ly@20170912 for SECJZJY-20921

// 为了保持原集中交易中的日志管理，所以仍然保留本模式
typedef struct mymsgbuf
{
    int mtype;
    char mtext[MAXMSG];
};
extern CSvrLink *g_pSvrLink; // 与业务调度中心的连接


//时间格式类型转换 "18:35:46" -> 183546
int TimeStr2Int(char *str)
{
    int ret = -1;
    char tmpstr[20] = {0};
    int hour = 0;
    int minute = 0;
    int second = 0;

    if (strlen(str) == 8)
    {
        strncpy(tmpstr, str, sizeof(tmpstr));
        if ( (tmpstr[0]>='0' && tmpstr[0]<='9')
             && (tmpstr[1]>='0' && tmpstr[1]<='9')
             && (tmpstr[3]>='0' && tmpstr[3]<='9')
             && (tmpstr[4]>='0' && tmpstr[4]<='9')
             && (tmpstr[6]>='0' && tmpstr[6]<='9')
             && (tmpstr[7]>='0' && tmpstr[7]<='9')
             && (tmpstr[2]==':' && tmpstr[5]==':') )
        {
            tmpstr[2] = tmpstr[5] = 0;
            hour = atoi(tmpstr);
            minute = atoi(tmpstr + 3);
            second = atoi(tmpstr + 6);

            if ( (hour >= 0 && hour < 24)
                 && (minute >= 0 && minute < 60)
                 && (second >= 0 && second < 60) )
            {
                ret = hour * 10000 + minute * 100 + second;
            }
        }
    }

    return ret;
}

/*
函数功能：检查机器时间是否在指定时间段内
函数参数：sbegintime-开始时间
          sendtime-结束时间
          offset-时间偏移量，单位秒
返 回 值：true-在时间段内；false-不在时间段内（时间格式错误也认为不在）
*/
BOOL CheckValidTime(char *sbegintime, char *sendtime)
{
    time_t t1 = 0;
    struct tm *pt = NULL;

    time(&t1);
    pt = localtime(&t1);
    int curtime = pt->tm_hour * 10000 + pt->tm_min * 100 + pt->tm_sec;
    int bgntime = TimeStr2Int(sbegintime);
    int endtime = TimeStr2Int(sendtime);

    if (bgntime != -1 && endtime != -1)
    {
        if ((curtime >= bgntime) && (curtime <= endtime))
            return true;
    }
    else
        return true;//设置的时间格式错误也认为在时间段内

    return false;
}


// ZW REMARK
void WriteLog(int level, char *fmt, ...)
{
    char buf[MAXMSG];
    time_t long_time;
    struct tm logtime;
    char current_time[30];
    int msgbufferlen;
    va_list marker;
    struct mymsgbuf msgbuffer;

    if (level >= (g_iDebug & 0x0f))
        return; // g_iDebug的个位，对应level。1 as Level(0,1); 2 as Level(all)

    memset(buf, 0, sizeof(buf));
    va_start(marker, fmt);
    vsprintf(buf, fmt, marker);

    time(&long_time);
    memcpy(&logtime, localtime(&long_time), sizeof(logtime));

    memset(&msgbuffer, 0, sizeof(msgbuffer));
    sprintf(msgbuffer.mtext, "%4d %5d ", g_pSvrLink->iThisProcID, getpid());

    strftime(current_time, sizeof(current_time), "%Y%m%d %H:%M:%S ", &logtime);
    strcat(msgbuffer.mtext, current_time);
    strcat(msgbuffer.mtext, buf);
    strcat(msgbuffer.mtext, "\n");

    msgbuffer.mtype = 5;
    msgbufferlen = strlen(msgbuffer.mtext);

    int mi;
    if (level < 0)
    {
        mi = 44000 - level;
    }
    else if (level == 0)
    {
        mi = 14000;
    }
    else if (level == 1)
    {
        mi = 4000;
    }
    else
    {
        mi = 400;
    }
    TextLogOut(mi, buf);

    if ((level < 0) || g_iDebug >= 0x10)
        printf("%s", msgbuffer.mtext);

    return;
}

/*	打印中间调试信息到日志
*
* 	输出的调试信息不需要加\n, 系统会自动加
*	从PrintMsg输出的日志信息级别为1,也就是说传入的DEBUG参数中,低字节第1位置为1,该信息就会在日志中输出
*/
void PrintMsg(char *fmt, ...)
{
    char chMsg[8192];
    va_list pArg;

    if ((g_iDebug & 0xff) == 0)
        return; // 为了提高效率 2006-2-14 14:59:58

    va_start(pArg, fmt);
    vsprintf(chMsg, fmt, pArg);
    va_end(pArg);

    if (chMsg[0] == '\n')
        chMsg[0] = ' ';
    WriteLog(1, "%s", chMsg);
}

void GetRequestHead(ST_PACKHEAD *phead)
{
    g_pSvrLink->GetRequestHead(phead);
}

int SetColEx(void *handle, ST_CPACK *ppack, ...)
{
    va_list ap;
    int arg;
    va_start(ap, ppack);
    while ((arg = va_arg(ap, int)) > 0)
    {
        SetParmBit(&(ppack->head), arg);
    }
    va_end(ap);
    return -1;
}



#define REQQUEUENAME "bank.1"
#define REQQUEUENAME2 "bank.2"
#define REQQUEUENAME3 "bank.3"
#define REQQUEUENAMEWT "wt"
#define REQQUEUENAMECJ "cjhb"

/* 以下代码是用来字节对齐使用的 */
#ifdef WIN32
#define __PACKED__
#pragma pack(push, 1)
#else
#ifdef __GNUC__
#define __PACKED__ __attribute__((packed))
//#pragma pack(push,1)
#else
#ifdef HP_UX
#define __PACKED__
#pragma pack 1
#else
#define __PACKED__
#pragma options align = packed
#endif
#endif
#endif

typedef struct _ST_REQQUEUE
{
    int DestNo;
    int MainFunction;
    int buflen;
    char buffer[MAXCOMMSIZE];
} ST_REQQUEUE;

typedef struct _ST_ANSQUEUE
{
    char handle[RESERVELEN];
    int buflen;
    char buffer[MAXCOMMSIZE];
} ST_ANSQUEUE;

#ifdef WIN32
#pragma pack(pop)
#else
#ifdef __GNUC__
#else
#ifdef HP_UX
#pragma pack 0
#else
#pragma options align = reset
#endif
#endif
#endif
#undef __PACKED__


int PutQx(void *handle, int DestNo, int MainFunction, char *queuename, ST_CPACK cpack, char *pMsg)
{
        int irtn = 0;
        //irtn = g_pSvrLink->ExtTransfer(DestNo,MainFunction,&cpack,NULL);
        // WriteLog(1,"ExtTransfer(%d,%d,...)=%d",DestNo,MainFunction);
        irtn = g_pSvrLink->PushData(0, DestNo, MainFunction, 0, &cpack, PM_INACK, 2, NULL);
        if (irtn < 0)
        {
            sprintf(pMsg, "Push0(%d,%d:%d)错误:%d",
                    0, DestNo, MainFunction, irtn);
            return (0);
        }
        else
        {
            return (-1);
        }
}

int PutQ(void *handle, int DestNo, int MainFunction, ST_CPACK cpack, char *pMsg)
{
    return (PutQx(handle, DestNo, MainFunction, REQQUEUENAME, cpack, pMsg));
}

int PutWTQ(void *handle, int DestNo, int MainFunction, ST_CPACK cpack, char *pMsg)
{
    return (PutQx(handle, DestNo, MainFunction, REQQUEUENAMEWT, cpack, pMsg));
}

int PutCJHBQ(void *handle, int DestNo, int MainFunction, ST_CPACK cpack, char *pMsg)
{
    return (PutQx(handle, DestNo, MainFunction, REQQUEUENAMECJ, cpack, pMsg));
}

int PutQ3(void *handle, int DestNo, int MainFunction, ST_CPACK cpack, char *pMsg)
{
    return (PutQx(handle, DestNo, MainFunction, REQQUEUENAME3, cpack, pMsg));
}

#define GETSTRINT(i, t, s, len) \
    {                           \
        memcpy(t, (s), len);    \
        t[len] = 0;             \
        i = atoi(t);            \
    }

int PutQ2(void *handle, char *comm_handle, ST_CPACK cpack, char *pMsg)
{

        TRUSERID uid = {0};
        SYS_HEAD *psh = NULL;
        memcpy(&uid, handle, sizeof(uid));
        psh = (SYS_HEAD *)uid.syshead.buf;
        int i = 0;
        char tmp[32] = {0};
        for (i = 0; i < 6; i++)
        {
            GETSTRINT(psh->RouterIn[i], tmp, comm_handle + i * 4, 4);
            GETSTRINT(psh->RouterOut[i], tmp, comm_handle + i * 4 + 24, 4);
        }
        // 由于RouterIndex是一个<6的值，也就一个字符
        psh->RouterIndex = comm_handle[48] - '0';
        GETSTRINT(psh->Function, tmp, comm_handle + 49, 5);
        psh->FunctionPriority = comm_handle[54] - '0';
        GETSTRINT(psh->LastConnectTime, tmp, comm_handle + 55, 12);
        for (i = 0; i < 32; i++)
        {
            tmp[i] = comm_handle[67 + i];
            if (tmp[i] == '|' || tmp[i] == 0)
                break;
        }
        tmp[i] = 0;
        psh->CRC = inet_addr(tmp);

        uid.SourceNo = atoi(comm_handle + 68 + i); // 专门在新的comm_handle格式中的|后面加入了一个原始的SourceNo存放

        //g_pSvrLink->SetAnswerMode(AM_CONFIRM);
        i = g_pSvrLink->AnswerDataEx(&uid, &cpack, NULL, 0, "");
        WriteLog(1, "AnswerDataEx(%s,...)=%d", comm_handle, i);
        if (i < 0)
        {
            sprintf(pMsg, "数据结果转发给%s错误:%d", i);
            return (0);
        }
        else
            return (-1);
}

int PutQ2M(void *handle, char *comm_handle, char *databuf, int buflen, char *pMsg)
{

        TRUSERID uid;
        SYS_HEAD *psh;
        memcpy(&uid, handle, sizeof(uid));
        psh = (SYS_HEAD *)uid.syshead.buf;
        int i;
        char tmp[32];
        for (i = 0; i < 6; i++)
        {
            GETSTRINT(psh->RouterIn[i], tmp, comm_handle + i * 4, 4);
            GETSTRINT(psh->RouterOut[i], tmp, comm_handle + i * 4 + 24, 4);
        }
        if (buflen > MAXCOMMSIZE)
        {
            sprintf(pMsg, "返回包长度过大:%d", buflen);
            return 0;
        }
        // 由于RouterIndex是一个<6的值，也就一个字符
        psh->RouterIndex = comm_handle[48] - '0';
        GETSTRINT(psh->Function, tmp, comm_handle + 49, 5);
        psh->FunctionPriority = comm_handle[54] - '0';
        GETSTRINT(psh->LastConnectTime, tmp, comm_handle + 55, 12);
        for (i = 0; i < 32; i++)
        {
            tmp[i] = comm_handle[67 + i];
            if (tmp[i] == '|' || tmp[i] == 0)
                break;
        }
        tmp[i] = 0;
        psh->CRC = inet_addr(tmp);

        uid.SourceNo = atoi(comm_handle + 68 + i); // 专门在新的comm_handle格式中的|后面加入了一个原始的SourceNo存放

        i = g_pSvrLink->AnswerDataEx(&uid, buflen, databuf);
        WriteLog(1, "AnswerDataExbuf(%s,...)=%d", comm_handle, i);
        if (i < 0)
        {
            sprintf(pMsg, "数据结果转发给%s错误:%d", i);
            return (0);
        }
        else
            return (-1);
}

#define CHEX_A_10 0x37 // 'A'-10
char Bit4_HexC(unsigned char bit4)
{
    return ((bit4 < 10) ? ('0' + bit4) : (CHEX_A_10 + bit4));
}

unsigned char HexC_Bit4(char cHex)
{
    return ((cHex >= 'A') ? (cHex - CHEX_A_10) : (cHex - '0'));
}

char *Data2HexString(void *pData, int len, char *oStr)
{
    unsigned char *pByte = (unsigned char *)pData;
    int i;
    for (i = 0; i < len; i++)
    {
        (*oStr) = Bit4_HexC((*pByte) >> 4);
        oStr++;
        (*oStr) = Bit4_HexC((*pByte) & 0x0f);
        oStr++;
        pByte++;
    }
    (*oStr) = '\0';
    return (oStr);
}

//#define SYSHEADKEEPLEN  (sizeof(SYS_HEAD)-sizeof(unsigned int))
void GetTransHandle(void *handle, char *pTransHandle)
{
    TRUSERID *ph = (TRUSERID *)handle;
    SYS_HEAD *psh = (SYS_HEAD *)ph->syshead.buf;
    char address[32];
    struct in_addr in;
    memset(&in, 0, sizeof(in));
    in.s_addr = psh->CRC;
    strcpy(address, inet_ntoa(in));
        sprintf(pTransHandle, "%04d%04d%04d%04d%04d%04d%04d%04d%04d%04d%04d%04d%1d%05d%1d%012d%s|%u",
                psh->RouterIn[0],
                psh->RouterIn[1],
                psh->RouterIn[2],
                psh->RouterIn[3],
                psh->RouterIn[4],
                psh->RouterIn[5],
                psh->RouterOut[0],
                psh->RouterOut[1],
                psh->RouterOut[2],
                psh->RouterOut[3],
                psh->RouterOut[4],
                psh->RouterOut[5],
                psh->RouterIndex,
                psh->Function,
                psh->FunctionPriority,
                psh->LastConnectTime,
                address,
                ph->SourceNo);
}

int mystrncpy(char *dest, const char *source, int dsize)
{
   int i;
   for (i=0;i<dsize;i++)
   {
      dest[i]=source[i];
      if (dest[i]=='\0')
         break;
   }
   if (i==dsize)
   {
      dest[i-1]='\0';
      i--;
   }
   for (i-1;i>=0;i--)
   {
      switch(dest[i])
      {
      case ' ':
      case '\t':
      case '\0':
         dest[i]='\0';
         break;
      default:
         return(i+1);
      }
   }
   return(0);
}


/* 去左右空格*/
void alltrim(char *str)
{
    int i,len;
    len=strlen(str);
    for(i=len-1;i>=0;i--)
    {
        if(str[i]!=' ')
            break;
        else
            str[i]=0;
    }
    for(i=0;i<len;i++)
    {
        if(str[i]!=' ')
            break;
    }
    if(i!=0)
    {
        //strncpy(str,str+i,len-i);
        memmove(str,str+i,len-i);
        str[len-i]=0;
    }
}

void MakeContract(char *pPrfx,long lNo,char *pContract,int iFixLen)
{
    int i0,i1,i2;
    char buf[11];
    char buf1[11];

    alltrim(pPrfx);
    sprintf(buf,"%d",lNo);
    alltrim(buf);

    i0 = strlen(pPrfx);
    i1 = strlen(buf);
    if (i0+i1>iFixLen)
    {
        strcpy(buf1,pPrfx);
        i2 = iFixLen - i0;	/* 前缀后边能放几位 */
        if (i2<0)
            i2 = 0;
        memcpy(buf1+i0,buf+i1-i2,i2);
        buf1[iFixLen]=0;
        strcpy(pContract,buf1);
        return;
    }
    i2 = iFixLen - i0;
    format_str(buf,i2);
    strcpy(buf1,pPrfx);
    strcpy(buf1+i0,buf);
    buf1[iFixLen]=0;
    strcpy(pContract,buf1);
}

void GetStrValue(char *instr,char sigstr,int order ,char *outstr)
{
    char tmpstr[1024]={0};
    int   icnt=0;
    int   iforder=0;
    int   ieorder=0;
    int   i=0;
    BOOL  binit=FALSE;
    for(i=0;i<strlen(instr);i++)
    {
        if( instr[i]==sigstr)
            icnt++;
        if((icnt==(order-1)) && !binit)
        {
            iforder = i+1;
            binit=TRUE;
        }
        if (icnt == order)
        {
            ieorder=i;
            break;
        }
    }
    if (ieorder==0)
    {
        ieorder = strlen(instr);
    }
    if (iforder>0)
    {	
        strncpy(outstr,instr+iforder,ieorder-iforder);
    }
    else
    {
        strcpy(outstr,"");
    }
}

BOOL CanOpenAcc(char *sourcestr,char *deststr)
{
    char szstr[2]={0};
    char tmpdeststr[1024]={0};
    int i=0;
    if (strcmp(deststr,"")==0)
    {
        return TRUE;
    }
    if (strcmp(sourcestr,"")==0)
    {
        return TRUE ;
    }
    GetStrValue(deststr,';',3 ,tmpdeststr);
    for (i=0;i<strlen(sourcestr);i++)
    {
        szstr[0] = sourcestr[i];
        szstr[1] = 0;
        if (strstr(tmpdeststr,szstr)!=NULL)
        {
            return FALSE;
        }
    }
    return TRUE;
}

static int nFunctions=0;
int FComp(const void *t1,const void *t2)
{
   if (((TBDefine *)t1)->RequestType > ((TBDefine *)t2)->RequestType)
      return(1);
   if (((TBDefine *)t1)->RequestType == ((TBDefine *)t2)->RequestType)
      return(0);
   else return(-1);
}


// 函数名: ResetBPFunctions
// 编程  : 陈永华 2004-2-11 22:40:49
// 描述  : 重置功能表中的参数
// 返回  : int :登记的功能个数
int ResetFunctions()
{
   int i;
   TBDefine *pbd=g_InXBDefines;
   for (i=0;;i++,pbd++)
   {
      if (pbd->RequestType==0)
         break;

/* ****** Updated by CHENYH at 2004-2-17 17:29:32 ****** 
      if (pbd->iPriority<1)
         pbd->iPriority = 1;
      else if (pbd->iPriority>3)
         pbd->iPriority = 3;
*/
      pbd->bStoped = false;
      pbd->nSuccess = pbd->nFail = pbd->nTt_max = pbd->nTt_min = 0;
      pbd->dTakentime = 0;
   }
   nFunctions = i;
   qsort(g_InXBDefines,nFunctions,sizeof(TBDefine),FComp);
   return(i);
}


int FindFunction(unsigned int reqtype)
{
    int i = 0;
    int l = 0;
    int r = 0;
    if (reqtype == 0)
        return(-10);
    if (g_InXBDefines[l].RequestType == reqtype)
        return(l);
    if (g_InXBDefines[l].RequestType > reqtype)
        return(-3);
    r = nFunctions - 1;
    if (g_InXBDefines[r].RequestType == reqtype)
        return(r);
    if (g_InXBDefines[r].RequestType < reqtype)
        return(-2);
    while (r > l + 1)
    {
        i = (l + r) >> 1;
        if (g_InXBDefines[i].RequestType == reqtype)
            return(i);
        if (g_InXBDefines[i].RequestType > reqtype)
        {
            r = i - 1;
            if (g_InXBDefines[r].RequestType == reqtype)
                return(r);
        }
        else
        {
            l = i + 1;
            if (g_InXBDefines[l].RequestType == reqtype)
                return(l);
        }
    }

    return(-1);
}
int InCallFunctions(int fno, void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg)
{
    int rtn;
    APPFUNC pFunc = g_InXBDefines[fno].pFunc;

    rtn = (*pFunc)(handle, iRequest, rPack, iRetCode, szMsg);
    return (rtn);
}

int InDoProcessPack(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg)
{
    int fno;
    fno = FindFunction(iRequest);
    if (fno >= 0)
    {
        return (InCallFunctions(fno, handle, iRequest, rPack, iRetCode, szMsg));
    }
    else
    {
        PrintMsg("不支持的交易编码:%d.", iRequest);
        sprintf(szMsg, "不支持的交易编码:%d.", iRequest);
        *iRetCode = 0x100;
        return 0;
    }
}


BOOL OFS_ConvertIden15to18(char *Scert_no15,char *Scert_no18)
{
    long getNum=0;
    char	m_Scert_no[51]={0};
    
                    m_Scert_no[0] = 0;
                    strncpy(m_Scert_no,Scert_no15,6);
                    strncpy(m_Scert_no+6,"19",2);
                    strncpy(m_Scert_no+8,Scert_no15+6,9);
                    //第18效验位的计算 start
                    getNum=((m_Scert_no[0]-'0')*7+ (m_Scert_no[1]-'0')*9+(m_Scert_no[2]-'0')*10+(m_Scert_no[3]-'0')*5
                     +(m_Scert_no[4]-'0')*8+(m_Scert_no[5]-'0')*4+(m_Scert_no[6]-'0')*2+(m_Scert_no[7]-'0')*1
                     +(m_Scert_no[8]-'0')*6+(m_Scert_no[9]-'0')*3+(m_Scert_no[10]-'0')*7+(m_Scert_no[11]-'0')*9
                     +(m_Scert_no[12]-'0')*10+(m_Scert_no[13]-'0')*5+(m_Scert_no[14]-'0')*8+(m_Scert_no[15]-'0')*4
                     +(m_Scert_no[16]-'0')*2)%11;	
              switch (getNum) {
                      case 0 :
                      strncpy(m_Scert_no+17,"1",1);
                      break;
                      case 1 :
                      strncpy(m_Scert_no+17,"0",1);
                      break;
                      case 2 :
                      strncpy(m_Scert_no+17,"X",1);
                      break;
                      case 3 :
                      strncpy(m_Scert_no+17,"9",1);
                      break;
                      case 4 :
                      strncpy(m_Scert_no+17,"8",1);
                      break;
                      case 5 :
                      strncpy(m_Scert_no+17,"7",1);
                      break;
                      case 6 :
                      strncpy(m_Scert_no+17,"6",1);
                      break;
                      case 7 :
                      strncpy(m_Scert_no+17,"5",1);
                      break;
                      case 8 :
                      strncpy(m_Scert_no+17,"4",1);
                      break;
                      case 9 :
                      strncpy(m_Scert_no+17,"3",1);
                      break;
                      case 10 :
                      strncpy(m_Scert_no+17,"2",1);
                      break;
                      }
                    m_Scert_no[18] = 0;
                    strcpy(Scert_no18,m_Scert_no);
    
                    return(TRUE);	
}

BOOL BAK_ACC_HIS(char *sTacode,char *sCustNo,char *sBranchCode,int *piRetCode,char *omsg)
{return TRUE;}