
#include "stdafx.h"
#include <stdio.h>
#include "profile.h"
#include "slist.h"
#include "cpack.h"
#include "hbusqc.h" // 与各个sqc程序相关的全局变量和函数定义
#include "svrlink.h"
#include "bdefine.h"
#include "logfile.h"
#include "hotspub.h"
#include "ssmtfunc.h"
#include "svr5_2.h"
#include "hotsparam.h"
#include "BDFunc.inl"

ST_SQLDB g_SQLDB;

ST_BUNIT g_BUnit;              // 这里只有单一业务处理单元
CSvrLink *g_pSvrLink;          // 与业务调度中心的连接
CLogFile g_LogFile;            // 被移到SvrLink.cpp中，作为系统必要的属性
T_PUBCTRL *g_pPubCtrl = NULL; // SSMT内存控制块指针
char g_ssmtpath[256];
extern TBDefine g_XBDefines[]; // 在具体的函数定义部分，如BDFTest.cpp -> BDFunc.inl中

// 集中交易中专用的全局数据
int g_iDebug; // 设置DEBUG的类型
int DoProcessPack(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);

/* ****** Updated by CHENYH at 2005-11-5 14:51:49 ****** 
将本函数从bupub.cpp中移到这里，是为了不同系统根据自己的环境进行调整
*******************************************************/
// 函数名: WriteAppInfo
// 编程  : 陈永华 2004-3-22 15:34:38
// 描述  : BCC向第一个连接好的BU发起的一个刷新后台的请求，以便业务模块部分能够作相关处理
//     注意：该函数属于框架程序在接收到BCC的WriteAppInfo的时候调用的。
// 返回  : int
// 参数  : int bccBaseFuncNo
// 参数  : int bccID
int WriteAppInfo(int bccBaseFuncNo, int bccID)
{
    // 当需要采用动态注册功能的时候，这里可以进行本BU组向BCC注册功能的处理
    TBDefine *pBDefine;
    for (pBDefine = g_XBDefines; pBDefine->RequestType != 0; pBDefine++)
    {
        g_pSvrLink->UpdateRegFunction(pBDefine->RequestType, pBDefine->szRTName, pBDefine->szProgrammer, pBDefine->iPriority, pBDefine->bStoped);
    }

    return (0);
}

#define NEED_CHECK 1046
int CallFunctions(int fno, void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg)
{
    int rtn;
    APPFUNC pFunc = g_XBDefines[fno].pFunc;

    rtn = (*pFunc)(handle, iRequest, rPack, iRetCode, szMsg);
    return (rtn);
}

int DoProcessPack(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg)
{
    int fno;
    fno = FindBPFunction(iRequest);
    if (fno >= 0)
    {
        return (CallFunctions(fno, handle, iRequest, rPack, iRetCode, szMsg));
    }
    else
    {
        PrintMsg("不支持的交易编码:%d.", iRequest);
        sprintf(szMsg, "不支持的交易编码:%d.", iRequest);
        *iRetCode = 0x100;
        return 0;
    }
}

// CYH + 2018/2/28 星期三 18:53:11
//extern int g_mrPackNo; // 当g_bMultiReqPack时候，当前正在处理请求记录的序号，从0开始的
extern bool g_bMultiReqPack;
int g_mrPackNo = 0;
void BeginMultiRPProcess();
void EndMultiRPProcess(TRUSERID *handle, int *iRetCode, char *szMsg);
/* ****** Updated by CHENYH at 2005-11-5 14:50:13 ****** 
将本函数从bdefine.cpp中提取到这里是为了让各个系统自行调整本函数的调用方法
*******************************************************/
int CallBDFunc(int fno, TRUSERID *handle, ST_CPACK *rpack, ST_PACK *pArrays, int *iRetCode, char *szMsg)
{
    int rtn;
    PrintMsg("UserID[%02x],hostname[%s],queuename[%s],queuetype[%02x]", rpack->head.hook.UserID, rpack->head.hook.hostname, rpack->head.hook.queuename, rpack->head.hook.queuetype);

    /* Removed by CHENYH @ 2018/2/28 星期三 19:10:12
   rtn = CallFunctions(fno,handle,rpack->head.RequestType,&(rpack->pack),iRetCode,szMsg);
   改为如下这种方式了 */
    // 2018/2/28 星期三 18:12:41 为了能够支持多记录请求框架 by CYH  -- 不必要为多记录请求另外改写代码
    g_mrPackNo = 0;
    if (rpack->head.recCount > 1)
    {
        ST_PACK *pp = &(rpack->pack);
        BeginMultiRPProcess();
        for (; g_mrPackNo < rpack->head.recCount; ++g_mrPackNo)
        {
            rtn = CallFunctions(fno, handle, rpack->head.RequestType, pp, iRetCode, szMsg);
            if (rtn == 0 && *iRetCode != -999 && *iRetCode != NEED_CHECK)
            {
                // 表示交易编码处理异常
                g_bMultiReqPack = false; // 阻止 EndMultiRPProcess 内部 PutRow，否则下面的报错返回会被客户端丢弃
                break;
            }
            pp = pArrays + g_mrPackNo;
        }
        EndMultiRPProcess(handle, iRetCode, szMsg);
    }
    else
    {
        rtn = CallFunctions(fno, handle, rpack->head.RequestType, &(rpack->pack), iRetCode, szMsg);
    }
    //////////////////////////////////////////////////////////////////////////
    /* ****** Updated by CHENYH at 2006-8-16 11:35:21 ****** */
    // 集中交易中的事务处理
    if (rtn != 0 || *iRetCode == -999 || *iRetCode == NEED_CHECK)
    {
        Commit();

        // +2015/10/12 11:32:02 CYH 为了彻底兼容appsvr，参见svr5.sqc中的
        szMsg[0] = '\0';
        //////////////////////////////////////////////////////////////////////////
    }
    else
    {
        RollBack();
        PrintMsg("DoProcessPack:%u ErrCode=%d %s", rpack->head.RequestType, *iRetCode, szMsg);
        g_pSvrLink->Err_Deal(szMsg, *iRetCode); //增加错误信息vsmess返回
    }
    /*********************************************************/

    return (rtn);
}

#ifdef WIN32
#include <conio.h>

void setnoblockgetch()
{
}

int mygetch()
{
    if (kbhit())
        return (getch());
    else
        return (0);
}
#else
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

void setnoblockgetch()
{
    int oldstatus;
    oldstatus = fcntl(0, F_GETFL);
    fcntl(0, F_SETFL, oldstatus | O_NDELAY);
}

int mygetch()
{
    int rtn;
    char kbuf[2];
    rtn = read(0, kbuf, 1);
    if (rtn <= 0)
        return (0);
    else
        return (kbuf[0]);
}
#endif

bool ReadIni(char *inifile)
{
    TIniFile tf;
    CSList tmpstr;

    char szBuf[1024] = {0};

    g_LogFile.Open("transbulog");      // 用程序的进程号作为各个BU日志文件存放目录, 用进程号区分日志中的信息
    g_LogFile.SetFlushDelay(-10); // 每个信息都必须马上刷新到LOG文件中

    memset(&g_BUnit, 0, sizeof(g_BUnit));
    printf("Begin to read ini-file:%s...\n", inifile);
    if (!tf.Open(inifile))
    {
        sprintf(szBuf, "不能打开配置文件<%s>\n", inifile);
        DEBUG_RUNTIME_MSGOUT(szBuf);
        exit(-1000);
    }
    g_LogFile.RemoveOverdueLogFile(tf.ReadInt("COMMON", "LOGOVERDUE", 10));
    //g_BUnit.iBUnitGourpID = tf.ReadInt("COMMON","GID",1);
    tf.ReadString("COMMON", "GID", "", g_BUnit.szBUGroupID, sizeof(g_BUnit.szBUGroupID) - 1);
    tf.ReadString("BCC", "BCCSVR", "127.0.0.1:6666", szBuf, sizeof(szBuf) - 1);
    if (!tmpstr.FillStrings(szBuf, ':') || tmpstr.GetCount() < 2)
    {
        sprintf(szBuf, "配置文件<%s>的配置项[BCC] BCCSVR=???? 错误! 格式为：BCCSVR=xxx.xxx.xxx.xxx:port\n", inifile);
        DEBUG_RUNTIME_MSGOUT(szBuf);
        exit(-1000);
    }
    strncpy(g_BUnit.szSvrIP, tmpstr.GetAt(0), sizeof(g_BUnit.szSvrIP) - 1);
    g_BUnit.iSvrPort = atoi(tmpstr.GetAt(1));
    g_BUnit.iHBInterval = tf.ReadInt("BCC", "HEARTBEAT", 5000);
    if (g_BUnit.iHBInterval < 100)
        g_BUnit.iHBInterval = 100;

        /******* Updated by CHENYH at 2007-11-23 9:46:43 ******/
        // 为了能够支持MQ模式
#ifndef NOIPCS
    tf.ReadTString("BCC", "MQNAME", "", szBuf, sizeof(szBuf));
    BUSetMQName(szBuf);
#endif
    /******************************************************/

    g_iDebug = tf.ReadInt("COMMON", "DEBUG", 12);        // 全部调试跟踪，并且显示
    g_iDebug = (g_iDebug % 10) | ((g_iDebug / 10) << 4); // 转换为16进制格式，便于WriteLog的时候使用

    tf.ReadTString("SQLSVR", "NAME", "KSDBS", g_SQLDB.szServer, sizeof(g_SQLDB.szServer));
    tf.ReadTString("SQLSVR", "DATABASE", "KSDBS", g_SQLDB.szDatabase, sizeof(g_SQLDB.szDatabase));
    tf.ReadTString("SQLSVR", "LOGIN", "", g_SQLDB.szLogin, sizeof(g_SQLDB.szLogin));
    tf.ReadTString("SQLSVR", "PASSWORD", "", g_SQLDB.szPassword, sizeof(g_SQLDB.szPassword));

    tf.ReadTString("COMMON", "SSMTPATH", "../ssmt/", g_ssmtpath, sizeof(g_ssmtpath));

    tf.Close();

    return (true);
}

void DynamicCheckSSMT()
{
    char eMsg[1024];
    if (CheckSSMTStatus(g_ssmtpath, eMsg))
    {
        g_LogFile.WriteLogEx(10000, "%s", eMsg);
    }
    g_pPubCtrl = GetSSMTInfoBlock(eMsg);
}

int main(int argc, char *argv[])
{
    int rtn;

    g_pSvrLink = BUPubInitialize(g_XBDefines, CallBDFunc, WriteAppInfo, &g_LogFile);
    SetLogShowLevel(0);
    if (argc < 2)
        ReadIni("transbu.ini");
    else
        ReadIni(argv[1]);

    ResetBPFunctions();
    ResetFunctions();
    if (argc > 2)
    {
        ListBPFunctions(argv[2]);
    }

    // 初始化数据库连接：
    SQLInitialize(&g_SQLDB); // ini 文件中读到 g_SQLDB
    if (SQLConnectToServer() != 0)
    {
        RAISE_RUNTIME_ERROR("不能正常建立数据库连接, 检查配置和数据库服务器!\n");
        return (-100);
    }
    // 初始化与BCC连接：
    do
    {
        rtn = g_pSvrLink->ToLink(&g_BUnit);
        if (rtn == 1)
        {
            DEBUG_RUNTIME_MSGOUT("与业务调度中心(BCC)的连接成功！\n");
            break;
        }
        else if (rtn == -100)
        {
            DEBUG_RUNTIME_MSGOUT("估计业务调度中心(BCC)尚未正式启动，或者检查配置中的[SERVER]项参数\n");
            mysleep(g_BUnit.iHBInterval);
        }
        else
        {
            // 应该属于故障，或对方并不是BCC
            return (rtn);
        }
    } while (1);

#ifdef WIN32
    setnoblockgetch();
#endif

    while (g_pSvrLink->LinkOK())
    {
        if (g_pSvrLink->bExit)
            break;
#ifdef WIN32
        switch (mygetch())
        {
        case '?':
        case 'h':
        case 'H':
            printf("\nCommand List:\n");
            printf("\t ? or h: display this Help informations!\n");
            printf("\t x: To eXit this business unit.\n");
            printf("\t d: Display functions status.\n");
            printf("\t l: List functions status into <function.lst>.\n");
            printf("Enter command to select:");
            break;
        case 'x':
        case 'X':
            g_pSvrLink->bExit = true;
            continue;
            break;
        case 'd':
        case 'D':
            ListBPFunctions(NULL);
            break;
        case 'l':
        case 'L':
            ListBPFunctions("function.lst");
            break;
        }
#endif
        if (!SQLIsConnectOK())
        {
            mysleep(1000);
            if (!g_pSvrLink->CheckLink())
                break; // BCC exit (可能用户发现数据库连接断开，需要维护系统，导致手工将BCC退出同时也需要BU退出)
            if (SQLConnectToServer() != 0)
            {
                // SQL Link error:
                DEBUG_RUNTIME_MSGOUT("不能正常建立数据库连接, 检查配置和数据库服务器!\n");
            }
            else
            {
                DEBUG_RUNTIME_MSGOUT("与数据库连接成功!\n");
            }
            continue;
        }
        else
        {
            DynamicCheckSSMT();
            g_pSvrLink->Processing(&g_BUnit);
        }
    }

    g_pSvrLink->Close();
    SQLExit();
    
    DEBUG_RUNTIME_MSGOUT("业务处理单元BU系统正常退出!\n");
    g_LogFile.Close();
    return (0);
}
