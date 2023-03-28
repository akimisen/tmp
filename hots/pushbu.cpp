/* --------------------------------------------------------------------
 * 修改日期: 2020-12-24
 * 修改人员: 李向东
 * 修改描述: pushbu支持非交易委托推送 888008
 * 备注信息: SECREQ-12733 【海通】【融资融券】HOSTBU支持两融深圳非交易业务
 * 版本信息: <ver>
 * 专题信息: SECREQ-12733 【海通】【融资融券】HOSTBU支持两融深圳非交易业务
 * -------------------------------------------------------------------
 * 修改日期: 2021-03-04
 * 修改人员: 李向东
 * 修改描述: pushbu支持非交易设置推送 888018 ；"PUSH","TABLE" 为8
 * 备注信息: SECRZRQ-9174
 * 版本信息: <ver>
 * 专题信息：【20210303】SECREQ-13212 HOTSBU支持推送非交易设置信息
 * -------------------------------------------------------------- */
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

#define VERSION "PUSHBU Version 1.0.0.0. CREATE IN 2020.09.27 AUTHOR BY ZW"

ST_SQLDB g_SQLDB;

ST_BUNIT g_BUnit;              // 这里只有单一业务处理单元
CSvrLink *g_pSvrLink;          // 与业务调度中心的连接
CLogFile g_LogFile;            // 被移到SvrLink.cpp中，作为系统必要的属性
extern TBDefine g_PushXBDefines[]; // 在具体的函数定义部分，如BDFTest.cpp -> BDFunc.inl中
int g_iDebug; // 设置DEBUG的类型
ST_PUSHBU_PARAM g_pushbu_param;
int g_iChoose; // 推送表的选择


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
    for (pBDefine = g_PushXBDefines; pBDefine->RequestType != 0; pBDefine++)
    {
        g_pSvrLink->UpdateRegFunction(pBDefine->RequestType, pBDefine->szRTName, pBDefine->szProgrammer, pBDefine->iPriority, pBDefine->bStoped);
    }

    return (0);
}

extern "C"
{
    BOOL F888000(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
    BOOL F888004(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
    BOOL F888005(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
    BOOL F888008(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
    BOOL F888018(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
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
    char sCLLogRIniFile[1024] = {0};

    g_LogFile.Open("pushbulog");      // 用程序的进程号作为各个BU日志文件存放目录, 用进程号区分日志中的信息
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

#ifndef NOIPCS
    tf.ReadTString("BCC", "MQNAME", "", szBuf, sizeof(szBuf));
    BUSetMQName(szBuf);
#endif
    tf.ReadTString("SQLSVR", "NAME", "KSDBS", g_SQLDB.szServer, sizeof(g_SQLDB.szServer));
    tf.ReadTString("SQLSVR", "DATABASE", "KSDBS", g_SQLDB.szDatabase, sizeof(g_SQLDB.szDatabase));
    tf.ReadTString("SQLSVR", "LOGIN", "", g_SQLDB.szLogin, sizeof(g_SQLDB.szLogin));
    tf.ReadTString("SQLSVR", "PASSWORD", "", g_SQLDB.szPassword, sizeof(g_SQLDB.szPassword));

    g_iDebug = tf.ReadInt("COMMON", "DEBUG", 12);        // 全部调试跟踪，并且显示
    g_iDebug = (g_iDebug % 10) | ((g_iDebug / 10) << 4); // 转换为16进制格式，便于WriteLog的时候使用

    memset(&g_pushbu_param, 0, sizeof(g_pushbu_param));
    tf.ReadTString("PUSH","BWORK_TIME","09:15:00",g_pushbu_param.sbegintime,sizeof(g_pushbu_param.sbegintime));
    tf.ReadTString("PUSH","EWORK_TIME","15:30:00",g_pushbu_param.sendtime,sizeof(g_pushbu_param.sendtime));
    g_pushbu_param.ipushInterval = tf.ReadInt("PUSH","PUSH_INTERVAL",1000);
    g_pushbu_param.inumPerPush = tf.ReadInt("PUSH","PUSH_NUMBER",500);
    g_pushbu_param.itryTimes = tf.ReadInt("PUSH","TRY_TIMES",3);
    g_iChoose = tf.ReadInt("PUSH","TABLE",0);

    tf.Close();

    return (true);
}

int main(int argc, char *argv[])
{
    int rtn = 0;
    APPFUNC pFunc = NULL;
    if(argc == 0)
        return -1;

    g_pSvrLink = BUPubInitialize(g_PushXBDefines,NULL,WriteAppInfo,&g_LogFile);

    SetLogShowLevel(0);

    // 读bqbu的配置文件
    if (argc < 2)
        ReadIni("pushbu.ini");
    else
        ReadIni(argv[1]);

    ResetBPFunctions();
    if (argc>2)
    {
        ListBPFunctions(argv[2]);
    }

    if(g_iChoose == 0)
    {
        pFunc = F888000; // 推送HOTS_FUND_STK_TMP_CHG
    }
    else if (g_iChoose == 1)
    {
        pFunc = F888004; // 推送HOTS_XY_ENTRUST
    }
    else if (g_iChoose == 2)
    {
        pFunc = F888005; // 推送HOTS_REAL_DONE
    }
    else if (g_iChoose == 3)
    {
        pFunc = F888008; // 推送HOTS_OTHER_ENTRUST
    }
    else if (g_iChoose == 8)
    {
        pFunc = F888018; // 推送HOTS_NOTRADE_TABLE_CHG
    }
    else
    {
        pFunc = F888000; // 缺省为推送 HOTS_FUND_STK_TMP_CHG
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
            DEBUG_RUNTIME_MSGOUT(VERSION);
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
            return(rtn);
        }
    } while (1);


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
            if(CheckValidTime(g_pushbu_param.sbegintime, g_pushbu_param.sendtime))
            {
                (*pFunc)(NULL, 0, NULL, NULL, NULL);
            }
            mysleep(g_pushbu_param.ipushInterval);
        }

    }

    g_pSvrLink->Close();
    DEBUG_RUNTIME_MSGOUT("业务处理单元BU系统正常退出!\n");
    g_LogFile.Close();
    return(0);
}
