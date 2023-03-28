/* ------------------------------------------------------------
 * �޸�����: 2022-01-19
 * �޸���Ա: ����
 * �޸�����: �����߰汾gcc�²����ַ��������ڴ��ص�����
 * ��ע��Ϣ: SECRZRQ-10964
 * �汾��Ϣ: 3.22.3.3
 * ר����Ϣ: SECREQ-15050 ����ͨ����������ȯ��������ȯ֧��linux7.6��̨����
 * --------------------------------------------------------------*/
#include "stdafx.h"
#include "mypub.h"

#include <sys/stat.h>

#ifdef WIN32
#include <direct.h>
#define PATHMARK  '\\'
#define MAKEPATH(a) mkdir(a)
#else
#include <sys/types.h>
#include <dirent.h>
#define PATHMARK  '/'
#define MAKEPATH(a) mkdir(a,0777)
#endif

/* ȥ�����ҿո�   */
char *mytrim(char *s)
{
	int i,len;

	len=strlen(s);
	for (i=len-1;i>=0;i--)
	{
		if ((s[i] != ' ') && (s[i] != '\t'))
			break;
		else
			s[i]=0;
	}
	for (i=0; i<len; i++)
	{
		if ((s[i] != ' ') && (s[i] != '\t'))
			break;
	}
	if (i != 0)
	{
        //strncpy(s,s+i,len-i);
        memmove(s,s+i,len-i);
		s[len-i]=0;
	}
	return s;
}

// ������: *Strncpy
// ���  : ������ 2004-2-21 13:39:08
// ����  : ��source����ิ��count�ַ���dest��������dest[count-1]����ֵΪ'\0'�ַ���������
// ����  : char 
// ����  : char *dest
// ����  : char *source
// ����  : size_t count
char *Strncpy(char *dest,char *source,unsigned int count)
{
   strncpy(dest,source,count);
   dest[count-1]='\0';
   return(dest);
}


// ������: *Strncpy_t
// ���  : ������ 2004-2-21 13:39:12
// ����  : ��source����ิ��count�ַ���dest��������dest[count-1]����ֵΪ'\0'�ַ���������
//        ͬʱȥ�����ҿո�
// ����  : char 
// ����  : char *dest
// ����  : char *source
// ����  : size_t count
char *Strncpy_t(char *dest,char *source,unsigned int count)
{
   mytrim(Strncpy(dest,source,count));
   return(dest);
}


// ������: mytrim_r
// ���  : ������ 2004-2-21 13:45:29
// ����  : ����ַ����ұߵ�' '��'\t'
// ����  : char *
// ����  : char *dest
char *mytrim_r(char *dest)
{
   int i;
   i = strlen(dest)-1;
   for (;i>=0;i--)
   {
		if ((dest[i] != ' ') && (dest[i] != '\t'))
			break;
		else
			dest[i]=0;
   }
   return(dest);
}


// ������: Strncpy_rt
// ���  : ������ 2004-2-21 13:46:22
// ����  : ���Ʋ����dest�ұߵ�' '��'\t'
// ����  : char * ����dest��ָ��
// ����  : char *dest
// ����  : char *source
// ����  : size_t count
char *Strncpy_rt(char *dest,char *source,unsigned int count)
{
   Strncpy(dest,source,count);
   return(mytrim_r(dest));
}

// ����  : ���÷ָ��separator�ָ�Ĵ��ַ���pc�У���ȡ��pid�����ַ�����para��
// ����  : int ��true(!=0)-Ϊ�ɹ���false(0)��Ϊʧ��
// ����  : const char *pc [IN]: ���ַ��� �� "000,1,22, 333,4444"
// ����  : char separator [IN]: �ָ�� ��','
// ����  : int pid [IN]: Ҫ��ȡ�����ַ�������ţ���0��ʼ
// ����  : char *para [OUT]: ����ȡ���������ַ�������������pid=3�����ַ�������Ϊ" 333"
// ����  : int psize [IN]: para����ĳ��ȣ����ֽ�����ʾ
// ����  : char *rtnmsg [OUT]: ���ش�����Ϣ 
int GetSubString(const char *pc, char separator ,int pid, char *para, int psize, char *rtnmsg)
{
   int ip;
   int n=pid;
   // ����ǰ��������ֵ
   for (ip=0; n>0; ip++)
   {
      if (pc[ip]=='\0')
      {
         sprintf(rtnmsg,"���ַ���<%s>�У�û�е�(%d)������",pc,pid);
         return(0);
      }
      else if (pc[ip]==separator)
      {
         n--;
      }
   }
   // �����ָ��
   if (pid>0 && pc[ip]==separator)
      ip++;
   // ȡ�ַ���������
   for (n=0;pc[ip]!=separator && pc[ip]!='\0';ip++)
   {
      para[n++]=pc[ip];
      if (n>=psize)
      {
         sprintf(rtnmsg,"���ַ���<%s>�У�����[%d]���峬������������%d",pc,pid,psize);
         return(0);
      }
   }
   
   para[n]='\0';
   return(1);
}


// ������: percolate
// ���  : ������ 2003-9-24 13:38:37
// ����  : ���ַ����н��Ƿ��ַ����˵���ͬʱ����//��Ϊ����Ϊע������Ҳͬ//һ����˵�
// ����  : int �������ĳ���
// ����  : char *str In��Out
int percolate(char *str)
{
   int l,i;
   l = strlen(str)-1;
   for (;l>=0;l--)
   {
      if ((unsigned char)(str[l])<0x20)
         str[l]='\0';
      else break;
   }
   for (i=0;i<=l;i++)
   {
      if (str[i]=='/' && str[i+1]=='/') 
      {
         str[i]='\0';
         break;
      }
   }
   return(i);
}

#define MAXUSLEEPUNIT 500000
// ������: mysleep
// ���  : ������ 2004-2-11 12:35:51
// ����  : ���߳̽���˯��״̬milliseconds����
// ����  : void 
// ����  : unsigned int milliseconds
void mysleep(unsigned int milliseconds)
{
#ifdef WIN32
   Sleep(milliseconds);
#else
//#ifdef AIX
//   struct timespec delay;
//   delay.tv_sec =0;
//   delay.tv_nsec = milliseconds*1000;
//   pthread_delay_np(&delay);
//#else
   /******* Updated by CHENYH at 2005-10-29 0:39:25 ******/
   // ����HP-UX����ʱ�򣬷���usleep��������Ĳ���ֵΪ1000000,
   // ��˲���ֱ����usleep(milliseconds*10000)������
   unsigned int i;
   milliseconds *= 1000;
   do {
   	if (milliseconds>MAXUSLEEPUNIT)
      {
         i = MAXUSLEEPUNIT;
      }
      else
      {
         i = milliseconds;
      }
      milliseconds -= i;
      usleep(i);
   } while(milliseconds>100);
   /*******************************************************/
   //usleep(milliseconds*1000);
//#endif
#endif
}

// ������: *getfmttime
// ���  : ������ 2003-10-14 11:23:47
// ����  : ��tm��ʱ��ת��Ϊ����ʱ��ĸ�ʽ���ַ���������ϵͳ��non-thread-safe
// ����  : char * ��ʽ�����ַ���ָ��, NULL ��ʹ���ڲ���̬�ռ��Ÿ�ʽ�����ַ�����
// ����  : time_t *ptm ����Ҫת����ʱ�� ==NULL ��Ϊת��ʱʱ���
// ����  : int fmttype 
/*
            0 : hh:mm:ss
            1 : MM/DD-hh:mm:ss
            2 : YYYY/MM/DD-hh:mm:ss
            3 : YYYYMMDD
            4 : hhmmss
            ���� : YYYYMMDDhhmmss
*/
// ����  : char *outstr ����ַ����Ļ��棬=NULL��ʹ���ڲ��ľ�̬���棬�䳤����Ҫ��fmttype����
char *getfmttime(void *ptm,int fmttype,char *outstr)
{
   static char tstr[80];
   struct tm tmloc;
   time_t timet;
   
   if (ptm==NULL)
   {
      ptm = &timet;
      time(&timet);
   }
   else
   {
      memcpy(&timet,ptm,sizeof(timet));
   }
   if (outstr==NULL)
   {
      outstr = tstr;
   }
   //localtime_r(&tm,&tmloc);
   memcpy(&tmloc,localtime(&timet),sizeof(tmloc));
   if (fmttype==0)
   {
      // %H:%M:%S:
      strftime(outstr,79,"%H:%M:%S",&tmloc);
   }
   else if (fmttype==1)
   {
      // %m/%d-%H:%M:%S
      strftime(outstr,79,"%m/%d %H:%M:%S",&tmloc);
   }
   else if (fmttype==2)
   {
      strftime(outstr,79,"%Y/%m/%d %H:%M:%S",&tmloc);
   }
   else if (fmttype==3)
   {
      strftime(outstr,79,"%Y%m%d",&tmloc);
   }
   else if (fmttype==4)
   {
      strftime(outstr,79,"%H%M%S",&tmloc);
   }
   else 
   {
      strftime(outstr,79,"%Y%m%d%H%M%S",&tmloc);
   }
   return(outstr);
}


void DataDumpFile(char *filename,char *data,int datalen)
{
   FILE *fp;
   fp = fopen(filename,"a+b");
   if (fp!=NULL)
   {
      fwrite(data,1,datalen,fp);
      fclose(fp);
   }
}

void OutErrorFile(char *efilename,char *msg,char *sourcefile,int lineno)
{
   FILE *fp;
   fp = fopen(efilename,"a+t");
   if (fp!=NULL)
   {
      fprintf(fp,
         "%s FILE:%s LINENO:%d -- [%s]\n",
         getfmttime(NULL,1,NULL),
         sourcefile,
         lineno,
         msg
         );      
      fclose(fp);
   }
}

void DebugFileOut(char *dfilename,char *msg,char *sourcefile,int lineno)
{
   char *pmsg;

/* ****** Updated by CHENYH at 2004-4-28 9:58:39 ****** 
   char szDRMMsg[1025];
   if (strlen(msg)<900) 
   {
      sprintf(szDRMMsg,"BU%d:%s",iThisProcID,msg);
      pmsg = szDRMMsg;
   }
   else
*/
   {
      pmsg = msg;
   }
   OutErrorFile(dfilename,pmsg,sourcefile,lineno);
   if (pmsg[strlen(pmsg)-2]=='\n')
   {
      printf("%s",pmsg);
   }
   else
   {
      printf("%s\n",pmsg);
   }
}

#ifdef WIN32
unsigned int tc2_tc1(unsigned int tc2,unsigned int tc1)
{
   return(tc2-tc1);
}
#else
unsigned int tc2_tc1(unsigned int tc2,unsigned int tc1)
{
   if (tc2<tc1)
   {
      return(86400000-tc1+tc2);
   }
   else
   {
      return(tc2-tc1);
   }
}
unsigned int GetTickCount()
{
   struct timeval CurrentTime;
   gettimeofday(&CurrentTime, NULL);
   return ((CurrentTime.tv_sec * 1000) + (CurrentTime.tv_usec / 1000));   
}

#endif

void OnRemoveOverdueFile(char *path_mark, char *ext, int overdue, char *file)
{
   FILE *fp;
   fp = fopen("RMFile.lst","a+t");
   if (fp!=NULL)
   {
      fprintf(fp,
         "%s: rm <%s> where path=<%s> ext=<%s> overdue=%d\n",
         getfmttime(NULL,1,NULL),
         file, path_mark, ext, overdue
         );
      fclose(fp);
   }
}


// ������: RemoveOverdueFileInSecond
// ���  : ������ 2008-10-7 13:41:20
// ����  : �����ָ����Ŀ¼path_mark�µ��޸�ʱ������overdue���������ļ�
// ����  : int ɾ�����ļ���
// ����  : char *path_mark  ָ����Ŀ¼�������ҪӦ�ð���Ŀ¼�ָ�������MS����ϵͳ��'\\' ��������'/'��
// ����  : char *ext �ļ���չ�������� '.'
// ����  : int overdue  ָ�����ڵ�����
int RemoveOverdueFileInSecond(char *path_mark,char *ext,int overdue)
{
   int nfs=0;
   char szfile[2048]; 
   time_t odtime;
   if (overdue<1)
      return(0);
   if (ext[0]=='\0'||memcmp(ext,".*",2)==0)
      return(0);
   odtime = time(NULL)-overdue;   
   sprintf(szfile,"%s*%s",path_mark,ext);
#ifdef WIN32
   struct _finddata_t fd;
   int dh;
   dh = _findfirst(szfile,&fd);
   if (dh==-1) 
      return(0);
   do
   {
      if ((odtime>fd.time_write) && strstr(fd.name,ext)!=NULL)
      {
         sprintf(szfile,"%s%s",path_mark,fd.name);
         OnRemoveOverdueFile(path_mark,ext,overdue,szfile);
         remove(szfile);
         nfs++;
      }
   } while (_findnext(dh,&fd)!=-1);
   _findclose(dh);
#else
   DIR *dp;
   struct dirent *dirp;
   struct stat fst;
   dp = opendir(path_mark);
   if (dp==NULL) 
      return(0);
   while ((dirp=readdir(dp))!=NULL)
   {
      sprintf(szfile,"%s%s",path_mark,dirp->d_name);
      if (strstr(dirp->d_name,ext)!=NULL 
         && stat(szfile,&fst)==0 
         && odtime>fst.st_mtime)
      {
         remove(szfile);
         OnRemoveOverdueFile(path_mark,ext,overdue,szfile);
         nfs++;
      }
   }
   closedir(dp);
#endif
   return(nfs);
}


// ������: RemoveOverdueFile
// ���  : ������ 2004-4-6 9:52:41
// ����  : �����ָ����Ŀ¼path_mark�µ��޸���������overdue���������ļ�
// ����  : int ɾ�����ļ���
// ����  : char *path_mark ָ����Ŀ¼�������ҪӦ�ð���Ŀ¼�ָ�������MS����ϵͳ��'\\' ��������'/'��
// ����  : char *ext �ļ���չ�������� '.'
// ����  : int overdue  ָ�����ڵ�����
int RemoveOverdueFile(char *path_mark,char *ext,int overdue)
{
   return(RemoveOverdueFileInSecond(path_mark,ext,overdue*86400));
}