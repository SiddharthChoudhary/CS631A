#include<dirent.h>
#include"flag.c"
#include<fts.h>
#include<grp.h>
#include<pwd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<time.h>
#include<unistd.h>
#include<zconf.h>
#define BUFFERSIZE 16384
struct Flags flags;
void invokingLSWithAInCurrentDirectory(char* argv[],struct Flags flags);
void printinLformat(FTSENT* file);
char* generateSTmodestring(struct stat* filestat,char* permission);
bool checkIfSuperUser();
int main(int argc, char* argv[]){
char ch;
while((ch=getopt(argc,argv,"AacdFfhiklnqRrSstuw:"))!=-1){
    switch (ch)
    {
    case 'A':
    flags.if_A_Is_True=true;
        break;
    case 'a':
    flags.if_a_Is_True=true;
        break;
    case 'c':
    flags.if_c_Is_True=true;
        break;
    case 'd':
    flags.if_d_Is_True=true;
        break;
    case 'F':
    flags.if_F_Is_True=true;
        break;
    case 'f':
    flags.if_f_Is_True=true;
        break;
    case 'h':
    flags.if_h_Is_True=true;
        break;
    case 'i':
    flags.if_i_Is_True=true;
        break;
    case 'k':
    flags.if_k_Is_True=true;
        break;
    case 'l':
    flags.if_l_Is_True=true;
        break;
    case 'n':
    flags.if_n_Is_True=true;
        break;
    case 'q':
    flags.if_q_Is_True=true;
        break;
    case 'R':
    flags.if_R_Is_True=true;
        break;
    case 'r':
    flags.if_r_Is_True=true;
        break;
    case 'S':
    flags.if_S_Is_True=true;
        break;
    case 's':
    flags.if_s_Is_True=true;
        break;
    case 't':
    flags.if_t_Is_True=true;
        break;
    case 'u':
    flags.if_u_Is_True=true;
        break;
    case 'w':
    flags.if_w_Is_True=true;
        break;
    default:
        break;
    }
}
if(argv[optind]==NULL){
    argv[optind]="./";
}
invokingLSWithAInCurrentDirectory(argv,flags);
return 0;
}
bool checkIfSuperUser(){
    if(getuid()==0){
        return true;
    }else{
        return false;
    }
}

struct stat getStatInformation(char* filename){
    struct stat buffer;
    return buffer;
}


int count=0;
void invokingLSWithAInCurrentDirectory(char* argv[],struct Flags flags){
DIR *dir;
struct dirent *currentFile;
char buf[BUFFERSIZE];
FTS *directoryTree;
FTSENT *file, *node;
char *argvs[]={argv[optind],NULL};
directoryTree = fts_open(argvs,FTS_LOGICAL|FTS_NOCHDIR,NULL);
if(directoryTree==NULL){
    fprintf(stderr,"Directory tree itself is null\n");
}
while((file=fts_read(directoryTree))){
    switch (file->fts_info) {
        // preorder case
        case FTS_F: printf("A regular file %s\n",file->fts_name);
                    break;
		// case FTS_SL: printf("A symbolic link %s\n",file->fts_name);
        //             continue;
        // case FTS_SLNONE: printf("A symbolic link with non-existing target %s\n",file->fts_name);
        //             continue;
        case FTS_DNR: fprintf(stderr,"cannot read the directory %s",file->fts_name);
                    break;
        case FTS_DC: fprintf(stderr,"Directory is forming a cycle may be related to this file: %s\n",file->fts_name);
                    break;
		case FTS_ERR: fprintf(stderr,"Error while reading the directory");
                    break;
		case FTS_NS: fprintf(stderr,"There is no stat available for this file %s",file->fts_name);/* stat() error */
                    break;
			/* Show error, then continue to next files. */
			fprintf(stderr,"%s Error at ", file->fts_path);
			continue;
        case FTS_DP: continue;
		}
        node = fts_children(directoryTree,0);
        while(node != NULL){
            if(flags.if_l_Is_True){
                printinLformat(node);
            }else{
                printf("%s \n",node->fts_name);
            }
            node = node->fts_link;
        }
        // This particular section checks that whether the following is recursive or not if it is not then we 
        // will stop recursion at the end of the traversal
        if(!flags.if_R_Is_True){
            if (file->fts_level > -1)
            {
                if(fts_set(directoryTree, file, FTS_SKIP)==-1){
                    fprintf(stderr,"Error occured while using fts_set \n");
                };
            }
        }
}
    printf("\n");
fts_close(directoryTree);
};
//generating permissions based on fstat->stmode and passing permission variable to avoid block level allocation error
char* generateSTmodestring(struct stat* fstat,char* permissions){
    if(S_ISDIR(fstat->st_mode)){
        strcat(permissions,"d");
    }else{
        strcat(permissions,"-");
    }
    if(fstat->st_mode & S_IRUSR){
        strcat(permissions,"r");
    }else{
        strcat(permissions,"-");
    }
    if(fstat->st_mode & S_IWUSR){
        strcat(permissions,"w");
    }else{
        strcat(permissions,"-");
    }
    if(fstat->st_mode & S_IXUSR){
        strcat(permissions,"x");
    }else{
        strcat(permissions,"-");
    }
    if(fstat->st_mode & S_IRGRP){
        strcat(permissions,"r");
    }else{
        strcat(permissions,"-");
    }
    if(fstat->st_mode & S_IWGRP){
        strcat(permissions,"w");
    }else{
        strcat(permissions,"-");
    }
    if(fstat->st_mode & S_IXGRP){
        strcat(permissions,"x");
    }else{
        strcat(permissions,"-");
    }
    if(fstat->st_mode & S_IROTH){
        strcat(permissions,"r");
    }else{
        strcat(permissions,"-");
    }
    if(fstat->st_mode & S_IWOTH){
        strcat(permissions,"w");
    }else{
        strcat(permissions,"-");
    }
    if(fstat->st_mode & S_IXOTH){
        strcat(permissions,"x");
    }else{
        strcat(permissions,"-");
    }
    return permissions;
};

void printinLformat(FTSENT* file){
    struct tm timestructure;
    char buf[80], permissions[40]="";
    char* months[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    char pw_name[5];char gid_name[5];
    time_t t = file->fts_statp->st_mtime;
    timestructure = *localtime(&t);
    char* permission=generateSTmodestring(file->fts_statp,permissions);
    struct passwd* passwordStructure=getpwuid(file->fts_statp->st_uid);
    if(passwordStructure==NULL){
        sprintf(pw_name,"%d",file->fts_statp->st_uid);
        passwordStructure->pw_name=pw_name;
    }
    struct group* groupId = getgrgid(file->fts_statp->st_gid);
    if(groupId==NULL){
        sprintf(gid_name,"%d",file->fts_statp->st_gid);
        groupId->gr_name=gid_name;
    }
    printf("%s  %d   %s    %s     %lli    %s %d %d:%d   %s\n",permission,
                                                    file->fts_statp->st_nlink,
                                                    passwordStructure->pw_name,
                                                    groupId->gr_name,
                                                    file->fts_statp->st_size,
                                                    months[timestructure.tm_mon],
                                                    timestructure.tm_mday,
                                                    timestructure.tm_hour,
                                                    timestructure.tm_min,
                                                    file->fts_name);

}
