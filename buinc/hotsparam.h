/* --------------------------------------------------------------
 * 程序名称：hotsparam.h
 * 创建日期：2020-09-29
 * 程序作者：周伟
 * 程序功能：HOTS组件相关bu参数文件
 * 备注信息: SECJZJY-52250 HOTS组件开发
 * 版本信息：3.21.5.2-p2
 * 专题信息: SECREQ-11868HTOS支持红利补扣税批量冻结
 * --------------------------------------------------------------*/
#ifndef __HOTSPARAM_H
#define __HOTSPARAM_H

typedef struct {
    char  sbegintime[9]                        ; // 开始工作时间
    char  sendtime[9]                          ; // 结束工作时间
    int   ipushInterval                        ; // 推送时间间隔
    int   inumPerPush                          ; // 每次推送条目
    int   itryTimes                            ; // 推送失败尝试次数
} ST_PUSHBU_PARAM;

extern ST_PUSHBU_PARAM g_pushbu_param;
#endif
