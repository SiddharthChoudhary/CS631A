#include<ctype.h>
#include<dirent.h>
#include<errno.h>
#include"flag.c"
#include<fts.h>
#include<grp.h>
#include<limits.h>
#include<math.h>
#include<pwd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<time.h>
#include<unistd.h>
#include<zconf.h>

#define BUFFERSIZE 16384
#define BUFFERSIZE20 20
#define BUFFERSIZE40 40
#define BUFFERSIZE80 80
#define BUFFERSIZE200 200
#define BLOCKSIZE1024 1024
#define BLOCKSIZE1024X1024X1024 1073741824
#define BLOCKSIZE512 512
#define whiteSpaces3 3
#define whiteSpaces11 11
#define whiteSpaces2 2

struct Flags flags;
bool errorInBlockSize=true;

void invokingLSWithInCurrentDirectory(char* argv[],struct Flags flags,int options,int argc);
void printinLformat(FTSENT* file, struct LsComponentForLOption dimensions,struct Flags flags);
char* generateSTmodestring(struct stat* filestat,char* permission);
bool checkIfSuperUser();
long long blocksizeCalculator(int blocks);
char* printSign(FTSENT* file,struct Flags flags);
struct LsComponentForLOption calculateWidthOfEachCell(FTS* file,struct Flags flags);
void checkForNonPrintableCharacters(char* filename1);
void traverseInCaseOfDFlag(FTS* directory,struct LsComponentForLOption dimensions,struct Flags flags);
void printInNormalForm(FTSENT *node, struct LsComponentForLOption dimensions,struct Flags flags);
long long mainCalculationForBlockSize(int blocks, long long blocksize, long int blocksizeBuffer);