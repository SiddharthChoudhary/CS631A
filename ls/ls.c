#include"ls.h"
#include"comparator.c"
#define BUFFERSIZE 16384
#define BUFFERSIZE20 20
#define BUFFERSIZE40 40
#define BUFFERSIZE80 80
#define BUFFERSIZE200 200
#define whiteSpaces11 11
#define whiteSpaces3 3
#define whiteSpaces2 2

struct Flags flags;
void invokingLSWithInCurrentDirectory(char* argv[],struct Flags flags,int options);
void printinLformat(FTSENT* file, struct LsComponentForLOption dimensions);
char* generateSTmodestring(struct stat* filestat,char* permission);
bool checkIfSuperUser();
struct LsComponentForLOption calculateWidthOfEachCell(FTS* directoryTree,struct Flags flags);
int main(int argc, char* argv[]){
char ch;
//initially setting the comparator by name as per the ls standards when there is no option provided
compare=compareByName;
int options = FTS_PHYSICAL|FTS_NOCHDIR;
while((ch=getopt(argc,argv,"AacdFfhiklnqRrSstuw:"))!=-1){
    switch (ch)
    {
    case 'A':
    flags.if_A_Is_True=true;
        break;
    case 'a':
    flags.if_a_Is_True=true;
    options = FTS_PHYSICAL|FTS_NOCHDIR|FTS_SEEDOT;
        break;
    case 'c':
    flags.if_c_Is_True=true;
    compare = compareByLastChagangedTime;
        break;
    case 'd':
    flags.if_d_Is_True=true;
        break;
    case 'F':
    flags.if_F_Is_True=true;
        break;
    case 'f':
    flags.if_f_Is_True=true;
    compare=(void*)compareInCaseOfNoSorting;
    options= FTS_PHYSICAL|FTS_NOCHDIR|FTS_SEEDOT;
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
    // because flag n is almost equal to l
    flags.if_l_Is_True=true;
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
    compare = compareBySize;
        break;
    case 's':
    flags.if_s_Is_True=true;
        break;
    case 't':
    flags.if_t_Is_True=true;
    compare = compareByModifiedTime;
        break;
    case 'u':
    flags.if_u_Is_True=true;
    compare = compareByLastAccessTime;
        break;
    case 'w':
    flags.if_w_Is_True=true;
        break;
    default:
        break;
    }
}
if(argv[optind]==NULL){
    argv[optind]=".";
}

/* #####
        Conditions for -A options,
        To check if the user is a superuser then it will always be executed with A,
##### */ 
if(checkIfSuperUser()){
    flags.if_A_Is_True=true;
}

/* 
Calling the main traversal for files
*/   
invokingLSWithInCurrentDirectory(argv,flags,options);

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


void invokingLSWithInCurrentDirectory(char* argv[],struct Flags flags,int options){
DIR *dir;
struct dirent *currentFile;
struct LsComponentForLOption dimensionsStructure;
char buf[BUFFERSIZE];
FTS *directoryTree;
FTSENT *file, *node;
char *argvs[]={argv[optind],NULL};
// open the directory to read the dimensions for each cell
directoryTree = fts_open(argvs,options,comparator);
dimensionsStructure=calculateWidthOfEachCell(directoryTree,flags);
// will have to reopen directory cause we close the directory
directoryTree = fts_open(argvs,options,comparator);
if(directoryTree==NULL){
    fprintf(stderr,"Directory tree itself is null\n");
}
while((file=fts_read(directoryTree))){
    switch (file->fts_info) {
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
        // displaying the total or the path name in case of multiple directories
        if(flags.if_R_Is_True && file->fts_info==FTS_D){
            printf("%s:\n",file->fts_path);
        }
        /* Main traversal of directory by children which entirely runs on children of the current file. fts_level is
        applied later to stop the traversal after one level*/
        node = fts_children(directoryTree,0);
        while(node != NULL){
            // if the directory starts with . then we need to ignore that because of the basic nature of ls.
            if(node->fts_info==FTS_D||node->fts_info==FTS_F){
                if(node->fts_name[0]=='.' && !flags.if_a_Is_True && !flags.if_A_Is_True){
                    node = node->fts_link;
                    continue;
                }
            }
            if(flags.if_l_Is_True){
                printinLformat(node,dimensionsStructure);
            }else{
                // straight away printing the file in normal cases
                if(flags.if_i_Is_True){
                    printf("%*lli ",dimensionsStructure.inodeWidth,node->fts_statp->st_ino);
                }
                if(flags.if_s_Is_True){
                    printf("%*lli ",dimensionsStructure.blocksWidth, node->fts_statp->st_blocks);
                }
                printf("%s \n",node->fts_name);
            }
            node = node->fts_link;
        }
         /* This particular section checks that whether the following is recursive or not if it is not then we 
         will stop recursion at the end of the traversal */
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

// as the name suggests it comes handy while looking out for usernames and groupnames from password file
struct UsernameAndGroupName getUsername(int uid,int gid){
    struct passwd* passwordStructure=getpwuid(uid);
    struct group* groupIdStructure = getgrgid(gid);
    struct UsernameAndGroupName userNameAndGroupName;
    char pw_nameBuffer[10];char gw_nameBuffer[10];
    // in case if the username or groupname in password file is missing, then we will throw the uid and gid itself
    if(passwordStructure!=NULL){
        if(passwordStructure->pw_name!=NULL){
            userNameAndGroupName.username=passwordStructure->pw_name;
        }else{
            sprintf(pw_nameBuffer,"%d",uid);
            userNameAndGroupName.username=pw_nameBuffer;
        }
    }
    if(groupIdStructure!=NULL){
        if(groupIdStructure->gr_name!=NULL){
            userNameAndGroupName.groupname = groupIdStructure->gr_name;
        }else{
            sprintf(gw_nameBuffer,"%d",gid);
            userNameAndGroupName.groupname=gw_nameBuffer;
        }
    }
   return userNameAndGroupName; 
};

void printinLformat(FTSENT* file,struct LsComponentForLOption dimensions){
    struct tm timestructure;
    char buf[BUFFERSIZE80], permissions[BUFFERSIZE40]="";
    char* months[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    time_t t = file->fts_statp->st_mtime;
    timestructure = *localtime(&t);
    char* permission=generateSTmodestring(file->fts_statp,permissions);
    struct UsernameAndGroupName usernameAndGroupName;
    usernameAndGroupName = getUsername(file->fts_statp->st_uid,file->fts_statp->st_gid);
    // if the flag is -n we won't convert the username and groupname id to string
    if(flags.if_n_Is_True){
        sprintf(usernameAndGroupName.username,"%d", file->fts_statp->st_uid);
        sprintf(usernameAndGroupName.groupname,"%d",file->fts_statp->st_gid);
    }
    if(flags.if_i_Is_True){
        printf("%*lli ",dimensions.inodeWidth,file->fts_statp->st_ino);
    }
    if(flags.if_s_Is_True){
        printf("%*lli ",dimensions.blocksWidth, file->fts_statp->st_blocks);
    }
    printf("%*s %*d %*s %*s %*lli %*s %*d %*d:%*d %-*s\n",whiteSpaces11,permission,
                                                    dimensions.numberOfLinks,
                                                    file->fts_statp->st_nlink,
                                                    dimensions.userNameWidth,
                                                    usernameAndGroupName.username,
                                                    dimensions.groupNameWidth,
                                                    usernameAndGroupName.groupname,
                                                    dimensions.numberOfBytesWidth,
                                                    file->fts_statp->st_size,
                                                    whiteSpaces3,
                                                    months[timestructure.tm_mon],
                                                    whiteSpaces2,
                                                    timestructure.tm_mday,
                                                    whiteSpaces2,
                                                    timestructure.tm_hour,
                                                    whiteSpaces2,
                                                    timestructure.tm_min,
                                                    dimensions.fileNameWidth,
                                                    file->fts_name);
}

// this function calculates the width for each cell and generates the maximum width which would be required
struct LsComponentForLOption calculateWidthOfEachCell(FTS* directoryTree,struct Flags flags){
    FTSENT* file,*node;
    struct LsComponentForLOption dimensions;
    char bufferForMaxInodeWidth[BUFFERSIZE20],bufferForMaximumBlocksWidth[BUFFERSIZE20], bufferForMaxNumberOfLinks[BUFFERSIZE80],bufferForMaxUserNameWidth[BUFFERSIZE80], bufferForMaGroupNameWIdth[BUFFERSIZE80], bufferForMaxBlockSizeWidth[BUFFERSIZE80], bufferForMaxFileNameWidth[BUFFERSIZE200];
    int maxInodeWidth = 0, maxUserNameIdWidth = 0,maxNumberOfLinks=0, maxblocksWidth=0, maxGroupNameWidth =0, maxBlockSizeWidth =0, maxFileNameWidth = 0;
    while((file=fts_read(directoryTree))){
        switch (file->fts_info) {
            case FTS_DP: continue;
            }
            /* Main traversal of directory by children which entirely runs on children of the current file. fts_level is
            applied later to stop the traversal after one level*/
            node = fts_children(directoryTree,0);
            while(node != NULL){
                struct UsernameAndGroupName usernameAndGroupName;
                //Running for directory, file and symnolic link, remember to consider the edge cases!
                if(node->fts_info==FTS_D||node->fts_info==FTS_F||node->fts_info==FTS_SL){
                        if(node->fts_statp!=NULL){
                            // storing the maximum length of inode cell                        
                            if(node->fts_statp->st_ino){
                                sprintf(bufferForMaxInodeWidth,"%lli",node->fts_statp->st_ino);
                                if(maxInodeWidth<strlen(bufferForMaxInodeWidth)){
                                    maxInodeWidth=strlen(bufferForMaxInodeWidth);
                                }
                            }
                            //storing maximum number of links
                            if(node->fts_statp->st_nlink){
                                sprintf(bufferForMaxNumberOfLinks,"%d",node->fts_statp->st_nlink);
                                if(maxNumberOfLinks<strlen(bufferForMaxNumberOfLinks)){
                                    maxNumberOfLinks=strlen(bufferForMaxNumberOfLinks);
                                }
                            }
                            if(node->fts_statp->st_blocks>=0){
                                sprintf(bufferForMaximumBlocksWidth,"%lli",node->fts_statp->st_blocks);
                                if(maxblocksWidth<strlen(bufferForMaximumBlocksWidth)){
                                    maxblocksWidth=strlen(bufferForMaximumBlocksWidth);
                                }
                            }
                            // storing the length of username and groupname by calling getusername function to read username and groupname from PASSWORD file        
                            if(node->fts_statp->st_uid>=0 && node->fts_statp->st_gid>=0){
                                usernameAndGroupName = getUsername(node->fts_statp->st_uid,node->fts_statp->st_gid);
                                if(maxUserNameIdWidth<(int)strlen(usernameAndGroupName.username)){
                                    maxUserNameIdWidth=strlen(usernameAndGroupName.username);
                                }
                                if(maxGroupNameWidth<strlen(usernameAndGroupName.groupname)){
                                    maxGroupNameWidth = strlen(usernameAndGroupName.groupname);
                                }
                            }
                            //  storing the max block size width for the file
                            if(node->fts_statp->st_size){
                                sprintf(bufferForMaxBlockSizeWidth,"%lli",node->fts_statp->st_size);
                                if(maxBlockSizeWidth<strlen(bufferForMaxBlockSizeWidth)){
                                    maxBlockSizeWidth=strlen(bufferForMaxBlockSizeWidth);
                                }
                            }
                            //assuming there will always be a name for a file
                            if(maxFileNameWidth<strlen(node->fts_name)){
                                maxFileNameWidth=strlen(node->fts_name);
                            }

                        }
                        node = node->fts_link;
                        continue;
                }
                node = node->fts_link;
            }
            /* This particular section checks that whether the following is recursive or not if it is not then we 
            will stop recursion at the end of the traversal */
            if(!flags.if_R_Is_True){
                if (file->fts_level > -1)
                {
                    if(fts_set(directoryTree, file, FTS_SKIP)==-1){
                        fprintf(stderr,"Error occured while using fts_set \n");
                    };
                }
        }
    }
    fts_close(directoryTree);
    dimensions.inodeWidth           =   maxInodeWidth+1;
    dimensions.userNameWidth        =   maxUserNameIdWidth+1;
    dimensions.groupNameWidth       =   maxGroupNameWidth+1;
    dimensions.numberOfBytesWidth   =   maxBlockSizeWidth+1;
    dimensions.numberOfLinks        =   maxNumberOfLinks+1;
    dimensions.blocksWidth          =   maxblocksWidth+1;
    dimensions.fileNameWidth        =   maxFileNameWidth+1;
    return dimensions;
}
