/*******************************************************************************
 * �������ƣ�BDFunc.inl
 * ��������: 2020-09-29
 * ������Ա: ��ΰ
 * ������: HOTS������ױ��빦������
 * ��ע��Ϣ: SECJZJY-52250 HOTS�������
 * �汾��Ϣ: 3.21.5.2-p2
 * ר����Ϣ: SECREQ-11868HTOS֧�ֺ�������˰��������
 * --------------------------------------------------------------
 * �޸�����: 2020-12-24
 * �޸���Ա: ����
 * �޸�����: 1�����ӷǽ��������������� 888008��888009��421403
 *           2�����ӽ��׺�̨���ױ���423135��423156����ҵ��ʹ��
 * ��ע��Ϣ: SECREQ-12733 ����ͨ����������ȯ��HOSTBU֧���������ڷǽ���ҵ��
 * �汾��Ϣ: 3.22.1.0
 * ר����Ϣ: SECREQ-12733 ����ͨ����������ȯ��HOSTBU֧���������ڷǽ���ҵ��
 * -------------------------------------------------------------------
 * �޸����ڣ�2021-03-04
 * �޸���Ա: ����
 * �޸�����: ���ӷǽ����������������������� 888018��888019��421408
 * ��ע��Ϣ: SECRZRQ-9174
 * �汾��Ϣ: 3.22.1.0
 * ר����Ϣ: ��20210303��SECREQ-13212 HOTSBU֧�����ͷǽ���������Ϣ
 * -------------------------------------------------------------------
 * �޸����ڣ�2021-12-14
 * �޸���Ա: ���
 * �޸�����: ���ӷǽ����������������������� 121140
 * ��ע��Ϣ: SECRZRQ-10764
 * �汾��Ϣ: 3.22.1.0
 * ר����Ϣ: SECREQ-14966����ͨ����������ȯ��HOTSBU֧���¹���ǩԤ�����ʽ�ⶳ
 * -------------------------------------------------------------------
 * �޸����ڣ�2022-02-24
 * �޸���Ա: ���
 * �޸�����: �������ر�֤��������ױ��� 421409
 * ��ע��Ϣ: SECRZRQ-11121
 * �汾��Ϣ: 3.22.1.0
 * ר����Ϣ: SECREQ-15255����ͨ����������ȯ��֧�����ؿͻ�������֤��  
 * -------------------------------------------------------------- */

#ifndef BDFUNC_INC
#define BDFUNC_INC

#include "mytypes.h"
#include "cpack.h"
#define ALL_FUNC
/*  ��ÿ��ҵ��������б�������������һ�У���ϸ�����ʾ�ĺ���,��BDefine.h�е�������
   {1000     ,FTestProc   ,"Demo for SQC ҵ��������д����"             ,"CYH Demo"          ,1     ,false,0,0,0,0,0},
   {���ܺ�   ,���̺���>    ,"��������"                                     ,"�����Ա����"       ,���ȼ�,false,0,0,0,0,0},
*/
// �����г����кϷ���ҵ������̺����б�
#ifdef __cplusplus
extern "C" {
#endif
/////////////////////////////////////////////////////////////////////////////////////////////   
// ����ΪHOTS����Ľ��ױ��빦�ܣ�
BOOL F888000(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
BOOL F888001(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
BOOL F888002(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
BOOL F888003(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
BOOL F888004(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
BOOL F888005(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
BOOL F888006(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
BOOL F888007(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
BOOL F888008(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
BOOL F888009(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
BOOL F888018(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
BOOL F888019(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
BOOL F421400(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
BOOL F421401(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
BOOL F421402(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
BOOL F421403(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
BOOL F421408(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
BOOL F421409(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL F121140(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
BOOL F423135(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
BOOL F423156(void *handle, int iRequest, ST_PACK *rPack, int *iRetCode, char *szMsg);
/////////////////////////////////////////////////////////////////////////////////////////////   
// ����ΪCHENYHΪ����ʾKSBU����ʵ�ֵ���ʾ���ܣ�
int FTestIReply(void *handle,int iRequest,ST_PACK *rPack,int *iRetCode,char *szMsg);
int FTestBCCShm(void *handle,int iRequest, ST_PACK *rpack, int *iRetCode, char *szMsg);
int FBCCShmMultiRows(void *handle,int iRequest, ST_PACK *rpack, int *iRetCode, char *szMsg);

#ifdef __cplusplus
}
#endif

// �������ݱ�Ϊ��ҵ����Ԫ�ܴ�������кϷ�ҵ������̼�����ز���
TBDefine g_PushXBDefines[]=
{
    //////////////////////////////////////////////////////////////////////////////////////
    INSERT_FUNCTION(0,NULL,"END BPFunctions List","CYH Marked",0)  // the last mark line,Don't remove this line
};

// �������ݱ�Ϊ��ҵ����Ԫ�ܴ�������кϷ�ҵ������̼�����ز���
TBDefine g_XBDefines[]=
{
    INSERT_FUNCTION(888001, F888001, "HOTSҵ������������", "HOTS", 1)
    INSERT_FUNCTION(888002, F888002, "HOTSҵ��Ӧ������������", "HOTS", 1)
    INSERT_FUNCTION(888003, F888003, "��ȯ����ⶳ��ˮ��ѯ", "HOTS", 1)
    INSERT_FUNCTION(888006, F888006, "HOTS_XY_ENTRUST��ѯ", "HOTS", 1)
    INSERT_FUNCTION(888007, F888007, "HOTS_REAL_DONE��ѯ", "HOTS", 1)
    INSERT_FUNCTION(888009, F888009, "HOTS_OTHER_ENTRUST��ѯ", "HOTS", 1)
    INSERT_FUNCTION(888019, F888019, "HOTS���ͷǽ������ò�ѯ", "HOTS", 1)
    //////////////////////////////////////////////////////////////////////////////////////
    INSERT_FUNCTION(0,NULL,"END BPFunctions List","CYH Marked",0)  // the last mark line,Don't remove this line
};

TBDefine g_InXBDefines[]=
{
    INSERT_FUNCTION(421400, F421400, "��ȯ�䶯��ˮӦ����", "HOTS", 1)
    INSERT_FUNCTION(421401, F421401, "Э��ί��Ӧ��", "HOTS", 1)
    INSERT_FUNCTION(421402, F421402, "Э��ɽ�Ӧ����", "HOTS", 1)
    INSERT_FUNCTION(421403, F421403, "�ǽ���ί��Ӧ����", "HOTS", 1)
    INSERT_FUNCTION(421408, F421408, "HOTSBU�������͵ķǽ���������ˮӦ������", "HOTS", 1)
    INSERT_FUNCTION(421409, F421409, "HOTSBU�������͵ı�֤�����Ӧ������", "HOTS", 1)
    //////////////////////////////////////////////////////////////////////////////////////
    INSERT_FUNCTION(121140, F121140, "IPO�¹���ǩԤ�����ʽ�ⶳ", "HOTS", 1)
    INSERT_FUNCTION(423135, F423135, "�ǽ���ί��", "HOTS", 1)
    INSERT_FUNCTION(423156, F423156, "�ǽ���ί�г���", "HOTS", 1)
    //////////////////////////////////////////////////////////////////////////////////////
    INSERT_FUNCTION(0,NULL,"END BPFunctions List","CYH Marked",0)  // the last mark line,Don't remove this line
};
#endif
