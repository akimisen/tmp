/* --------------------------------------------------------------
 * �������ƣ�hotsparam.h
 * �������ڣ�2020-09-29
 * �������ߣ���ΰ
 * �����ܣ�HOTS������bu�����ļ�
 * ��ע��Ϣ: SECJZJY-52250 HOTS�������
 * �汾��Ϣ��3.21.5.2-p2
 * ר����Ϣ: SECREQ-11868HTOS֧�ֺ�������˰��������
 * --------------------------------------------------------------*/
#ifndef __HOTSPARAM_H
#define __HOTSPARAM_H

typedef struct {
    char  sbegintime[9]                        ; // ��ʼ����ʱ��
    char  sendtime[9]                          ; // ��������ʱ��
    int   ipushInterval                        ; // ����ʱ����
    int   inumPerPush                          ; // ÿ��������Ŀ
    int   itryTimes                            ; // ����ʧ�ܳ��Դ���
} ST_PUSHBU_PARAM;

extern ST_PUSHBU_PARAM g_pushbu_param;
#endif
