#ifndef __PASS_H

#define __PASS_H



int  getUserName(char *FileName,char *dbUser,char *dbPassword);

int  passwd_new(char *password,int len,char *crit);

void setPassLevel(int level,char *keyword);

void getDqrq(char *d);

int  passwd_old(char *password,int len,char *crit);

int  WriteSystemDatFile(char *filename,char *UserName,char *UserPasswd);

int	 Sencrypt_init();

void Sencrypt_done();

int endes(char *block);

int dedes(char *block);

void DesDeCode(char *sInputStr,char *sOut);

void DesEnCode(char *sInputStr,char *sOut);

void DesPutHex(char *sBuf,char *sOut);



#endif

