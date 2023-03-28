/*******************************************************************************
 * 程序名称：BDFunc.inl
 * 创建日期: 2020-09-29
 * 创建人员: 周伟
 * 程序功能: HOTS组件交易编码功能声明
 * 备注信息: SECJZJY-52250 HOTS组件开发
 * 版本信息: 3.21.5.2-p2
 * 专题信息: SECREQ-11868HTOS支持红利补扣税批量冻结
 * --------------------------------------------------------------
 * 修改日期: 2020-12-24
 * 修改人员: 李向东
 * 修改描述: 1、增加非交易相关新申请代码 888008，888009，421403
 *           2、增加交易后台交易编码423135，423156，供业务使用
 * 备注信息: SECREQ-12733 【海通】【融资融券】HOSTBU支持两融深圳非交易业务
 * 版本信息: 3.22.1.0
 * 专题信息: SECREQ-12733 【海通】【融资融券】HOSTBU支持两融深圳非交易业务
 * -------------------------------------------------------------------
 * 修改日期：2021-03-04
 * 修改人员: 李向东
 * 修改描述: 增加非交易设置推送相关新申请代码 888018，888019，421408
 * 备注信息: SECRZRQ-9174
 * 版本信息: 3.22.1.0
 * 专题信息: 【20210303】SECREQ-13212 HOTSBU支持推送非交易设置信息
 * -------------------------------------------------------------------
 * 修改日期：2021-12-14
 * 修改人员: 邓炜
 * 修改描述: 增加非交易设置推送相关新申请代码 121140
 * 备注信息: SECRZRQ-10764
 * 版本信息: 3.22.1.0
 * 专题信息: SECREQ-14966【海通】【融资融券】HOTSBU支持新股中签预冻结资金解冻
 * -------------------------------------------------------------------
 * 修改日期：2022-02-24
 * 修改人员: 邓炜
 * 修改描述: 增加两地保证金调拨交易编码 421409
 * 备注信息: SECRZRQ-11121
 * 版本信息: 3.22.1.0
 * 专题信息: SECREQ-15255【海通】【融资融券】支持两地客户调拨保证金  
 * -------------------------------------------------------------- */

#ifndef BDFUNC_INC
#define BDFUNC_INC

#include "mytypes.h"
#include "cpack.h"
#define ALL_FUNC
/*  在每个业务处理过程中必须有如下这样一行：详细各项表示的含义,见BDefine.h中的描述：
   {1000     ,FTestProc   ,"Demo for SQC 业务处理函数编写样板"             ,"CYH Demo"          ,1     ,false,0,0,0,0,0},
   {功能号   ,过程函数>    ,"功能描述"                                     ,"编程人员姓名"       ,优先级,false,0,0,0,0,0},
*/
// 下面列出所有合法的业务处理过程函数列表：
#ifdef __cplusplus
extern "C" {
#endif
/////////////////////////////////////////////////////////////////////////////////////////////   
// 下面为HOTS组件的交易编码功能：
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
// 下面为CHENYH为了演示KSBU功能实现的演示功能：
int FTestIReply(void *handle,int iRequest,ST_PACK *rPack,int *iRetCode,char *szMsg);
int FTestBCCShm(void *handle,int iRequest, ST_PACK *rpack, int *iRetCode, char *szMsg);
int FBCCShmMultiRows(void *handle,int iRequest, ST_PACK *rpack, int *iRetCode, char *szMsg);

#ifdef __cplusplus
}
#endif

// 下面数据表为本业务处理单元能处理的所有合法业务处理过程及其相关参数
TBDefine g_PushXBDefines[]=
{
    //////////////////////////////////////////////////////////////////////////////////////
    INSERT_FUNCTION(0,NULL,"END BPFunctions List","CYH Marked",0)  // the last mark line,Don't remove this line
};

// 下面数据表为本业务处理单元能处理的所有合法业务处理过程及其相关参数
TBDefine g_XBDefines[]=
{
    INSERT_FUNCTION(888001, F888001, "HOTS业务请求类主体", "HOTS", 1)
    INSERT_FUNCTION(888002, F888002, "HOTS业务应答类请求主体", "HOTS", 1)
    INSERT_FUNCTION(888003, F888003, "资券冻结解冻流水查询", "HOTS", 1)
    INSERT_FUNCTION(888006, F888006, "HOTS_XY_ENTRUST查询", "HOTS", 1)
    INSERT_FUNCTION(888007, F888007, "HOTS_REAL_DONE查询", "HOTS", 1)
    INSERT_FUNCTION(888009, F888009, "HOTS_OTHER_ENTRUST查询", "HOTS", 1)
    INSERT_FUNCTION(888019, F888019, "HOTS推送非交易设置查询", "HOTS", 1)
    //////////////////////////////////////////////////////////////////////////////////////
    INSERT_FUNCTION(0,NULL,"END BPFunctions List","CYH Marked",0)  // the last mark line,Don't remove this line
};

TBDefine g_InXBDefines[]=
{
    INSERT_FUNCTION(421400, F421400, "资券变动流水应答处理", "HOTS", 1)
    INSERT_FUNCTION(421401, F421401, "协议委托应答", "HOTS", 1)
    INSERT_FUNCTION(421402, F421402, "协议成交应答处理", "HOTS", 1)
    INSERT_FUNCTION(421403, F421403, "非交易委托应答处理", "HOTS", 1)
    INSERT_FUNCTION(421408, F421408, "HOTSBU处理推送的非交易设置流水应答请求", "HOTS", 1)
    INSERT_FUNCTION(421409, F421409, "HOTSBU处理推送的保证金调拨应答请求", "HOTS", 1)
    //////////////////////////////////////////////////////////////////////////////////////
    INSERT_FUNCTION(121140, F121140, "IPO新股中签预冻结资金解冻", "HOTS", 1)
    INSERT_FUNCTION(423135, F423135, "非交易委托", "HOTS", 1)
    INSERT_FUNCTION(423156, F423156, "非交易委托撤单", "HOTS", 1)
    //////////////////////////////////////////////////////////////////////////////////////
    INSERT_FUNCTION(0,NULL,"END BPFunctions List","CYH Marked",0)  // the last mark line,Don't remove this line
};
#endif
