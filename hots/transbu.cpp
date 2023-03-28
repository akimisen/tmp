
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

ST_SQLDB g_SQLDB;

ST_BUNIT g_BUnit;              // ����ֻ�е�һҵ����Ԫ
CSvrLink *g_pSvrLink;          // ��ҵ��������ĵ�����
CLogFile g_LogFile;            // ���Ƶ�SvrLink.cpp�У���Ϊϵͳ��Ҫ������
T_PUBCTRL *g_pPubCtrl = NULL; // SSMT�ڴ���ƿ�ָ��
char g_ssmtpath[256];
extern TBDefine g_XBDefines[]; // �ھ���ĺ������岿�֣���BDFTest.cpp -> BDFunc.inl��

// ���н�����ר�õ�ȫ������
int g_iDebug; // ����DEBUG������
int DoProcessPack(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);

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
        PrintMsg("��֧�ֵĽ��ױ���:%d.", iRequest);
        sprintf(szMsg, "��֧�ֵĽ��ױ���:%d.", iRequest);
        *iRetCode = 0x100;
        return 0;
    }
}

// CYH + 2018/2/28 ������ 18:53:11
//extern int g_mrPackNo; // ��g_bMultiReqPackʱ�򣬵�ǰ���ڴ��������¼����ţ���0��ʼ��
extern bool g_bMultiReqPack;
int g_mrPackNo = 0;
void BeginMultiRPProcess();
void EndMultiRPProcess(TRUSERID *handle, int *iRetCode, char *szMsg);
/* ****** Updated by CHENYH at 2005-11-5 14:50:13 ****** 
����������bdefine.cpp����ȡ��������Ϊ���ø���ϵͳ���е����������ĵ��÷���
*******************************************************/
int CallBDFunc(int fno, TRUSERID *handle, ST_CPACK *rpack, ST_PACK *pArrays, int *iRetCode, char *szMsg)
{
    int rtn;
    PrintMsg("UserID[%02x],hostname[%s],queuename[%s],queuetype[%02x]", rpack->head.hook.UserID, rpack->head.hook.hostname, rpack->head.hook.queuename, rpack->head.hook.queuetype);

    /* Removed by CHENYH @ 2018/2/28 ������ 19:10:12
   rtn = CallFunctions(fno,handle,rpack->head.RequestType,&(rpack->pack),iRetCode,szMsg);
   ��Ϊ�������ַ�ʽ�� */
    // 2018/2/28 ������ 18:12:41 Ϊ���ܹ�֧�ֶ��¼������ by CYH  -- ����ҪΪ���¼���������д����
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
                // ��ʾ���ױ��봦���쳣
                g_bMultiReqPack = false; // ��ֹ EndMultiRPProcess �ڲ� PutRow����������ı����ػᱻ�ͻ��˶���
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
    // ���н����е�������
    if (rtn != 0 || *iRetCode == -999 || *iRetCode == NEED_CHECK)
    {
        Commit();

        // +2015/10/12 11:32:02 CYH Ϊ�˳��׼���appsvr���μ�svr5.sqc�е�
        szMsg[0] = '\0';
        //////////////////////////////////////////////////////////////////////////
    }
    else
    {
        RollBack();
        PrintMsg("DoProcessPack:%u ErrCode=%d %s", rpack->head.RequestType, *iRetCode, szMsg);
        g_pSvrLink->Err_Deal(szMsg, *iRetCode); //���Ӵ�����Ϣvsmess����
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

    g_LogFile.Open("transbulog");      // �ó���Ľ��̺���Ϊ����BU��־�ļ����Ŀ¼, �ý��̺�������־�е���Ϣ
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
    //g_BUnit.iBUnitGourpID = tf.ReadInt("COMMON","GID",1);
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

        /******* Updated by CHENYH at 2007-11-23 9:46:43 ******/
        // Ϊ���ܹ�֧��MQģʽ
#ifndef NOIPCS
    tf.ReadTString("BCC", "MQNAME", "", szBuf, sizeof(szBuf));
    BUSetMQName(szBuf);
#endif
    /******************************************************/

    g_iDebug = tf.ReadInt("COMMON", "DEBUG", 12);        // ȫ�����Ը��٣�������ʾ
    g_iDebug = (g_iDebug % 10) | ((g_iDebug / 10) << 4); // ת��Ϊ16���Ƹ�ʽ������WriteLog��ʱ��ʹ��

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
            DynamicCheckSSMT();
            g_pSvrLink->Processing(&g_BUnit);
        }
    }

    g_pSvrLink->Close();
    SQLExit();
    
    DEBUG_RUNTIME_MSGOUT("ҵ����ԪBUϵͳ�����˳�!\n");
    g_LogFile.Close();
    return (0);
}
