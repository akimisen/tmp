/* --------------------------------------------------------------------
 * �޸�����: 2020-12-24
 * �޸���Ա: ����
 * �޸�����: pushbu֧�ַǽ���ί������ 888008
 * ��ע��Ϣ: SECREQ-12733 ����ͨ����������ȯ��HOSTBU֧���������ڷǽ���ҵ��
 * �汾��Ϣ: <ver>
 * ר����Ϣ: SECREQ-12733 ����ͨ����������ȯ��HOSTBU֧���������ڷǽ���ҵ��
 * -------------------------------------------------------------------
 * �޸�����: 2021-03-04
 * �޸���Ա: ����
 * �޸�����: pushbu֧�ַǽ����������� 888018 ��"PUSH","TABLE" Ϊ8
 * ��ע��Ϣ: SECRZRQ-9174
 * �汾��Ϣ: <ver>
 * ר����Ϣ����20210303��SECREQ-13212 HOTSBU֧�����ͷǽ���������Ϣ
 * -------------------------------------------------------------- */
#include "stdafx.h"
#include <stdio.h>
#include "profile.h"
#include "slist.h"
#include "cpack.h"
#include "hbusqc.h" // �����sqc������ص�ȫ�ֱ����ͺ�������
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

ST_BUNIT g_BUnit;              // ����ֻ�е�һҵ����Ԫ
CSvrLink *g_pSvrLink;          // ��ҵ��������ĵ�����
CLogFile g_LogFile;            // ���Ƶ�SvrLink.cpp�У���Ϊϵͳ��Ҫ������
extern TBDefine g_PushXBDefines[]; // �ھ���ĺ������岿�֣���BDFTest.cpp -> BDFunc.inl��
int g_iDebug; // ����DEBUG������
ST_PUSHBU_PARAM g_pushbu_param;
int g_iChoose; // ���ͱ��ѡ��


/* ****** Updated by CHENYH at 2005-11-5 14:51:49 ****** 
����������bupub.cpp���Ƶ������Ϊ�˲�ͬϵͳ�����Լ��Ļ������е���
*******************************************************/
// ������: WriteAppInfo
// ���  : ������ 2004-3-22 15:34:38
// ����  : BCC���һ�����Ӻõ�BU�����һ��ˢ�º�̨�������Ա�ҵ��ģ�鲿���ܹ�����ش���
//     ע�⣺�ú������ڿ�ܳ����ڽ��յ�BCC��WriteAppInfo��ʱ����õġ�
// ����  : int
// ����  : int bccBaseFuncNo
// ����  : int bccID
int WriteAppInfo(int bccBaseFuncNo, int bccID)
{
    // ����Ҫ���ö�̬ע�Ṧ�ܵ�ʱ��������Խ��б�BU����BCCע�Ṧ�ܵĴ���
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

    g_LogFile.Open("pushbulog");      // �ó���Ľ��̺���Ϊ����BU��־�ļ����Ŀ¼, �ý��̺�������־�е���Ϣ
    g_LogFile.SetFlushDelay(-10); // ÿ����Ϣ����������ˢ�µ�LOG�ļ���

    memset(&g_BUnit, 0, sizeof(g_BUnit));
    printf("Begin to read ini-file:%s...\n", inifile);
    if (!tf.Open(inifile))
    {
        sprintf(szBuf, "���ܴ������ļ�<%s>\n", inifile);
        DEBUG_RUNTIME_MSGOUT(szBuf);
        exit(-1000);
    }
    g_LogFile.RemoveOverdueLogFile(tf.ReadInt("COMMON", "LOGOVERDUE", 10));
    tf.ReadString("COMMON", "GID", "", g_BUnit.szBUGroupID, sizeof(g_BUnit.szBUGroupID) - 1);
    tf.ReadString("BCC", "BCCSVR", "127.0.0.1:6666", szBuf, sizeof(szBuf) - 1);
    if (!tmpstr.FillStrings(szBuf, ':') || tmpstr.GetCount() < 2)
    {
        sprintf(szBuf, "�����ļ�<%s>��������[BCC] BCCSVR=???? ����! ��ʽΪ��BCCSVR=xxx.xxx.xxx.xxx:port\n", inifile);
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

    g_iDebug = tf.ReadInt("COMMON", "DEBUG", 12);        // ȫ�����Ը��٣�������ʾ
    g_iDebug = (g_iDebug % 10) | ((g_iDebug / 10) << 4); // ת��Ϊ16���Ƹ�ʽ������WriteLog��ʱ��ʹ��

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

    // ��bqbu�������ļ�
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
        pFunc = F888000; // ����HOTS_FUND_STK_TMP_CHG
    }
    else if (g_iChoose == 1)
    {
        pFunc = F888004; // ����HOTS_XY_ENTRUST
    }
    else if (g_iChoose == 2)
    {
        pFunc = F888005; // ����HOTS_REAL_DONE
    }
    else if (g_iChoose == 3)
    {
        pFunc = F888008; // ����HOTS_OTHER_ENTRUST
    }
    else if (g_iChoose == 8)
    {
        pFunc = F888018; // ����HOTS_NOTRADE_TABLE_CHG
    }
    else
    {
        pFunc = F888000; // ȱʡΪ���� HOTS_FUND_STK_TMP_CHG
    }
    

    // ��ʼ�����ݿ����ӣ�
    SQLInitialize(&g_SQLDB); // ini �ļ��ж��� g_SQLDB
    if (SQLConnectToServer() != 0)
    {
        RAISE_RUNTIME_ERROR("���������������ݿ�����, ������ú����ݿ������!\n");
        return (-100);
    }
    // ��ʼ����BCC���ӣ�
    do
    {
        rtn = g_pSvrLink->ToLink(&g_BUnit);
        if (rtn == 1)
        {
            DEBUG_RUNTIME_MSGOUT("��ҵ���������(BCC)�����ӳɹ���\n");
            DEBUG_RUNTIME_MSGOUT(VERSION);
            break;
        }
        else if (rtn == -100)
        {
            DEBUG_RUNTIME_MSGOUT("����ҵ���������(BCC)��δ��ʽ���������߼�������е�[SERVER]�����\n");
            mysleep(g_BUnit.iHBInterval);
        }
        else
        {
            // Ӧ�����ڹ��ϣ���Է�������BCC
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
                break; // BCC exit (�����û��������ݿ����ӶϿ�����Ҫά��ϵͳ�������ֹ���BCC�˳�ͬʱҲ��ҪBU�˳�)
            if (SQLConnectToServer() != 0)
            {
                // SQL Link error:
                DEBUG_RUNTIME_MSGOUT("���������������ݿ�����, ������ú����ݿ������!\n");
            }
            else
            {
                DEBUG_RUNTIME_MSGOUT("�����ݿ����ӳɹ�!\n");
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
    DEBUG_RUNTIME_MSGOUT("ҵ����ԪBUϵͳ�����˳�!\n");
    g_LogFile.Close();
    return(0);
}
