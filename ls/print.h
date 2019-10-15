#include<stdio.h>
struct UsernameAndGroupName getUsername(int uid,int gid);
char* generateSTmodestring(struct stat* filestat,char* permission);
char* hPrintableFormat(off_t st_blocks,int blocksize);
void printinLformat(FTSENT* file, struct LsComponentForLOption dimensions,struct Flags flags);
void printInNormalForm(FTSENT *node, struct LsComponentForLOption dimensions,struct Flags flags);
char* printSign(FTSENT* file,struct Flags flags);
char* calculateDiffTime(FTSENT* file1);