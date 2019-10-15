#include"ls.h"
#include"comparator.c"
#include"print.c"
int main(int argc, char* argv[]){
    char ch;
    int options = FTS_PHYSICAL|FTS_NOCHDIR;
    /* 
        initially setting the comparator by name as per the ls standards when there is no option provided 
        */
    compare=compareByName;
    /* 
        to check if the output is to the terminal in case of options like q and w, isatty(1) means output is to the terminal
     */
    if(isatty(STDOUT_FILENO)){
        flags.if_q_Is_True=true;
    }

    while((ch=getopt(argc,argv,"AacdFfhiklnqRrSstuw"))!=-1){
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
        flags.if_S_Is_True=false;
        flags.if_u_Is_True=false;
        compare = compareByLastChangedTime;
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
        /* 
            because flag n is almost equal to l 
        */
        flags.if_l_Is_True=true;
            break;
        case 'q':
        flags.if_q_Is_True=true;
        flags.if_w_Is_True=false;
            break;
        case 'R':
        flags.if_R_Is_True=true;
            break;
        case 'r':
        flags.if_r_Is_True=true;
            break;
        case 'S':
        flags.if_S_Is_True=true;
        flags.if_c_Is_True=false;
        flags.if_u_Is_True=false;
        flags.if_t_Is_True=false;
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
        flags.if_S_Is_True=false;
        flags.if_c_Is_True=false;
        compare = compareByLastAccessTime;
            break;
        case 'w':
        flags.if_w_Is_True=true;
        flags.if_q_Is_True=false;
            break;
        default:
            break;
        }
    }
    if(argv[optind]==NULL){
        argv[optind]=".";
    }

    /*
            Conditions for -A options,
            To check if the user is a superuser then it will always be executed with A,
    */ 
    if(checkIfSuperUser()){
        flags.if_A_Is_True=true;
    }

    

    if(flags.if_t_Is_True){
        compare=compareByModifiedTime;
            if(flags.if_c_Is_True){
                compare=compareByLastChangedTime;
            }else if(flags.if_u_Is_True){
                compare=compareByLastAccessTime;
            }
    }else if(!flags.if_t_Is_True){
        compare=compareByName;
    }


    /* 
        setting sorting functions based on the order they are being called, basically to override each other 
    */
    if(flags.if_r_Is_True){
        compare=compareByNameReverse;
        if(flags.if_t_Is_True){
            compare=compareByModifiedTimeReverse;
            if(flags.if_u_Is_True){
                compare=compareByLastAccessTimeReverse;
            }
            if(flags.if_c_Is_True){
                compare=compareByLastChagangedTimeReverse;
            }
        }
    }
    /* 
    Calling the main traversal for files
    */  

    invokingLSWithInCurrentDirectory(argv,flags,options,argc);

    return 0;
}
bool checkIfSuperUser(){
    if(getuid()==0){
        return true;
    }else{
        return false;
    }
}
void checkForNonPrintableCharacters(char* filename){
    int i = 0;
    while (filename[i] != '\0'){
       if(!isprint(filename[i])){
           if(flags.if_q_Is_True){
            filename[i]='?';
           }else if(flags.if_w_Is_True){
               /* do nothing and let it pass */
           }
       }
    i++;
   }
}
/* 
    malloc assigner when we don't know the size or length of the string, Remember to free it while invoking wherever 
*/
void* mallocAssignerCharPointer(){
   int n = 100*(sizeof(char));
   while(((char*)malloc(n))!=NULL){
      n*=10;
   }
   return (char*)malloc(n);
}
char* printSign(FTSENT* file,struct Flags flagsPrint){
    char *filename = malloc(strlen(flagsPrint.if_d_Is_True?file->fts_path:file->fts_name) + 1);
    strcpy(filename,flagsPrint.if_d_Is_True?file->fts_path:file->fts_name);
    if(flagsPrint.if_F_Is_True){
        if(S_ISDIR(file->fts_statp->st_mode)){
            if(file->fts_info!=FTS_DOT){
                strcat(filename,"/");
            }
        }
        else if(S_ISLNK(file->fts_statp->st_mode)){
            strcat(filename,"@");
        }
        else if(S_ISWHT(file->fts_statp->st_mode)){
            strcat(filename,"%");
        }
        else if(S_ISSOCK(file->fts_statp->st_mode)){
            strcat(filename,"=");
        }
        else if(S_ISFIFO(file->fts_statp->st_mode)){
            strcat(filename,"|");
        }
        else if((S_IXUSR |S_IXGRP|S_IXOTH) & file->fts_statp->st_mode){
            strcat(filename,"*");
        }
    }
    return filename;
}
/* 
    basic checking whether the blocksize is valid else blocksize would be by default 1024 
*/
long blocksizeCalculator(int blocks){
    char buffer[BUFFERSIZE20];
    char choice;
    long int blocksizeBufferRange=0;
    char bufferForBlocksizeBreaking[BUFFERSIZE40]="";
    long blocksize=0;
    char sizeInKiloBytesOrBytesInEnvBlockSize=' ';
    sprintf(buffer,"%s",getenv("BLOCKSIZE"));
    /* you will have to set everythingIsFine variable as true to execute calculation */
    bool unknownBlocksize=false,characterEncountered=false,minimumBlockSizeIs=false,maximumBlockSizeIs=false;
    unsigned int i=0;
    for(i=0;i<strlen(buffer);i++){
        int ch = buffer[i];
        if((ch>47&&ch<58)||(ch==75||ch==77||ch==71)||(ch==103||ch==107||ch==109)){
            /* in case of if it is B or K or  M or  G or b or k or m or g */
            if((ch==75||ch==77||ch==71)||(ch==103||ch==107||ch==109)){
                /* case where we already encountered a character in the string and now we are seeing an another one, So break */
                if(characterEncountered && strlen(buffer)>1){
                    unknownBlocksize=true;
                    break;
                }
                sizeInKiloBytesOrBytesInEnvBlockSize=buffer[i];
                characterEncountered=true;
            }else{
                /* concating the string  */
               sprintf(bufferForBlocksizeBreaking+strlen(bufferForBlocksizeBreaking),"%c",buffer[i]);
            }
        }else{
            unknownBlocksize = true;
        }
    }   
    if(strlen(buffer)==1){
        if(buffer[0]=='k'||buffer[0]=='m'||buffer[0]=='g'||buffer[0]=='K'||buffer[0]=='M'||buffer[0]=='G'){
            sprintf(bufferForBlocksizeBreaking+strlen(bufferForBlocksizeBreaking),"%d",1);
        }
    }
        /* unknown block size error */
        if(unknownBlocksize){
            /* assigning by default block size as 1024 */
            blocksize=BLOCKSIZE512;
            if(errorInBlockSize){
                printf("ls: %s: unknown blocksize\n",getenv("BLOCKSIZE"));
                printf("ls: maximum blocksize is 1G\n");
                printf("ls: %s minimum blocksize is 512\n",getenv("BLOCKSIZE"));
            }
            return (long)ceil((float)(blocks/(float)(blocksize/BLOCKSIZE512)));
        }
        /* to check if environment blocksize is in the range
        if it's plain numbers and no k, m or g */
        if(sizeInKiloBytesOrBytesInEnvBlockSize==' '){
            choice = ' ';
        }else if((int)(sizeInKiloBytesOrBytesInEnvBlockSize)==75||(int)(sizeInKiloBytesOrBytesInEnvBlockSize)==107){
            choice = 'k';
        }else if((int)(sizeInKiloBytesOrBytesInEnvBlockSize)==77||(int)(sizeInKiloBytesOrBytesInEnvBlockSize)==109){
            choice = 'm';
        }else if((int)(sizeInKiloBytesOrBytesInEnvBlockSize)==71||(int)(sizeInKiloBytesOrBytesInEnvBlockSize)==103){
            choice = 'g';
        }
       switch(choice){
           case ' ':blocksizeBufferRange=atoi(bufferForBlocksizeBreaking); 
                    break;
           case 'k':blocksizeBufferRange=atoi(bufferForBlocksizeBreaking)*BLOCKSIZE1024;
                    break;
           case 'm':blocksizeBufferRange=atoi(bufferForBlocksizeBreaking)*BLOCKSIZE1024*BLOCKSIZE1024;
                    break;
           case 'g':blocksizeBufferRange=atoi(bufferForBlocksizeBreaking)*BLOCKSIZE1024*BLOCKSIZE1024*BLOCKSIZE1024;
           break;
       }
        if(blocksizeBufferRange<BLOCKSIZE512){
            minimumBlockSizeIs = true;
        }
        else if((blocksizeBufferRange>BLOCKSIZE1024X1024X1024)){
            maximumBlockSizeIs = true;
        }
        /* because if the block size is unknown then there will be error thrown anyway before all this */
        if(maximumBlockSizeIs){
            blocksize=BLOCKSIZE1024X1024X1024;
            if(errorInBlockSize){
                printf("ls: maximum blocksize is 1G\n");
            }
        }
        else if(minimumBlockSizeIs){
            blocksize = BLOCKSIZE512;
            if(errorInBlockSize){
                printf("ls: %s: minimum blocksize is 512\n",getenv("BLOCKSIZE"));
            }
        }
        /* main calculation despite all the errors and if it's not in the range */
        return mainCalculationForBlockSize(blocks,blocksizeBufferRange);
}
long mainCalculationForBlockSize(int blocks,long int blocksizeBufferRange){
            long result = (long)ceil((float)(((float)blocks/blocksizeBufferRange)*BLOCKSIZE512));
            return result;
}
/* core invoking function for our program */
void invokingLSWithInCurrentDirectory(char* argv[],struct Flags flagsLs,int options,int argc){
    int row=0;
    char* argvs[argc-optind+1];
    FTS *directoryTree;
    FTSENT *file, *node;
    struct LsComponentForLOption dimensionsStructure;
    for(row=optind;row<=argc;row++){
        argvs[row-optind]=argv[row];
    }
    argvs[row-optind]=NULL;
    /* open the directory to read the dimensions for each cell */
    directoryTree = fts_open(argvs,options,comparator);
    dimensionsStructure=calculateWidthOfEachCell(directoryTree,flagsLs);
    /* will have to reopen directory cause we closed the directory */
    directoryTree = fts_open(argvs,options,comparator);
    if(directoryTree==NULL){
        fprintf(stderr,"Directory tree itself is null\n");
    }
    if(!flagsLs.if_d_Is_True){
        int i=0;
        /* 
            reading the directory again for printing and for the core part 
        */
        while((file=fts_read(directoryTree))){
            switch (file->fts_info) {
               /*  case FTS_SL: printf("A symbolic link %s\n",file->fts_name);
                            continue;
                case FTS_SLNONE: printf("A symbolic link with non-existing target %s\n",file->fts_name);
                            continue; */
                case FTS_DP: 
                        continue;
                case FTS_ERR:
                        fprintf(stderr,"Error : %s", strerror(errno));
                        continue;
                        break;
                case FTS_DNR:
                        fprintf(stderr,"Error : %s", strerror(errno));
                        continue;
                        break;
                default: break;
                }
                /* displaying the total or the path name in case of multiple directories */
                if((flagsLs.if_R_Is_True && file->fts_info==FTS_D) || file->fts_info==FTS_D){
                    if(i){
                    printf("%s:\n",file->fts_path);
                    }
                }
                /* Main traversal of directory by children which entirely runs on children of the current file. fts_level is
                applied later to stop the traversal after one level*/
                node = fts_children(directoryTree,0);
                /* 
                    In case if the arguements itself is a file having no children 
                */ 
               if(flagsLs.if_s_Is_True||flagsLs.if_l_Is_True){
                if(file->fts_info==FTS_D && fts_children(directoryTree,0)!=NULL){
                        if(flagsLs.if_h_Is_True){
                            char* total = hPrintableFormat(dimensionsStructure.total[i]*512,BLOCKSIZE1024);
                            printf("total %s\n",hPrintableFormat(dimensionsStructure.total[i]*512,BLOCKSIZE1024));
                            free(total);
                        }else{
                            long value = blocksizeCalculator(dimensionsStructure.total[i]);
                            printf("total %lu\n", value);
                        }
                        i++;
                }
               }
                /* checking here, because in case of recursive option we are getting duplicate prints. */
                if(node==NULL && !flagsLs.if_R_Is_True){
                    if(file->fts_info!=FTS_D||file->fts_info!=FTS_DP){
                        printInNormalForm(file,dimensionsStructure,flagsLs);
                        continue;
                    }
                }

                /* Main iteration of the loop */
                while(node != NULL){
                     if(node->fts_info==FTS_D||node->fts_info==FTS_F){
                        if(node->fts_name[0]=='.' && !flagsLs.if_a_Is_True && !flagsLs.if_A_Is_True){
                            node = node->fts_link;
                            continue;
                        }
                    }
                    printInNormalForm(node,dimensionsStructure,flagsLs);

                    node = node->fts_link;
                }
                if((flagsLs.if_R_Is_True && file->fts_info==FTS_D) || file->fts_info==FTS_D){
                    printf("\n");
                }
                /* This particular section checks that whether the following is recursive or not if it is not then we 
                will stop recursion at the end of the traversal */
                if(!flagsLs.if_R_Is_True){
                    if (file->fts_level > -1)
                    {
                        if(fts_set(directoryTree, file, FTS_SKIP)==-1){
                            fprintf(stderr,"Error occured while using fts_set \n");
                        };
                    }
                }
        }
    }else{
        traverseInCaseOfDFlag(directoryTree,dimensionsStructure,flagsLs);
    }
    fts_close(directoryTree);
}

void printInNormalForm(FTSENT* node,struct LsComponentForLOption dimensionsStructure, struct Flags flagsInNormal){
    long blocksize;
    char *printableSignString;
    /* if the directory starts with . then we need to ignore that because of the basic nature of ls. */
                    if(flagsInNormal.if_l_Is_True){
                        printinLformat(node,dimensionsStructure,flagsInNormal);
                    }else{
                            /* straight away printing the file in normal cases */
                            if(flagsInNormal.if_i_Is_True){
                                printf("%*lu ",dimensionsStructure.inodeWidth,node->fts_statp->st_ino);
                            }
                            /* earlier was using fts_statp's blocks but later got to know that env's blocksize can vary */
                            if(flagsInNormal.if_s_Is_True && !flagsInNormal.if_h_Is_True){
                                blocksize=blocksizeCalculator(node->fts_statp->st_blocks);
                                if(errorInBlockSize){
                                    errorInBlockSize=false;
                                }
                                printf("%*li ",dimensionsStructure.blocksWidth,blocksize);
                            }else if(flagsInNormal.if_s_Is_True && flagsInNormal.if_h_Is_True){
                                char* sizeInHumanReadable=hPrintableFormat(node->fts_statp->st_size,BLOCKSIZE1024);
                                printf("%*s ",dimensionsStructure.blocksWidth,sizeInHumanReadable);
                                free(sizeInHumanReadable);
                            }
                            checkForNonPrintableCharacters(node->fts_name);
                            printableSignString=printSign(node,flagsInNormal);
                            printf("%s \n",printableSignString);
                            free(printableSignString);
                        }
}
/* generating permissions based on fstat->stmode and passing permission variable to avoid block level allocation error */
char* generateSTmodestring(struct stat* fstat,char* permissions){
    if(S_ISDIR(fstat->st_mode)){
        strcat(permissions,"d");
    }else if((S_IFWHT&(fstat->st_mode))==0160000){
        strcat(permissions,"w");
    }else if(S_ISSOCK(fstat->st_mode)){
        strcat(permissions,"s");
    }else if(S_ISLNK(fstat->st_mode)){
        strcat(permissions,"l");
    }else if(S_ISCHR(fstat->st_mode)){
        strcat(permissions,"c");
    }else if(S_ISBLK(fstat->st_mode)){
        strcat(permissions,"b");
    }else if(S_ISFIFO(fstat->st_mode)){
        strcat(permissions,"p");
    }
    else{
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
}

/* as the name suggests it comes handy while looking out for usernames and groupnames from password file */
struct UsernameAndGroupName getUsername(int uid,int gid){
    struct passwd* passwordStructure=getpwuid(uid);
    struct group* groupIdStructure = getgrgid(gid);
    struct UsernameAndGroupName userNameAndGroupName;
    char pw_nameBuffer[10];char gw_nameBuffer[10];
    /* in case if the username or groupname in password file is missing, then we will throw the uid and gid itself */
    if(passwordStructure!=NULL){
        if(passwordStructure->pw_name!=NULL){
            userNameAndGroupName.username=passwordStructure->pw_name;
        }
    }else{
            sprintf(pw_nameBuffer,"%d",uid);
            userNameAndGroupName.username=pw_nameBuffer;
        }
    if(groupIdStructure!=NULL){
        if(groupIdStructure->gr_name!=NULL){
            userNameAndGroupName.groupname = groupIdStructure->gr_name;
    }
    }else{
            sprintf(gw_nameBuffer,"%d",gid);
            userNameAndGroupName.groupname=gw_nameBuffer;
        }
   return userNameAndGroupName; 
}

void printinLformat(FTSENT* file,struct LsComponentForLOption dimensions, struct Flags flagsInL){
    struct tm timestructure;
    char* printableSignString;
    struct UsernameAndGroupName usernameAndGroupName;
    long blocksize;
    char fileNameInCaseofSymbolicLnk[PATH_MAX]="";
    char pathInCaseOfSymbolicLink[PATH_MAX]="";
    char permissions[BUFFERSIZE40]="";
    char *filesize= (char*)malloc(100*sizeof(char));
    char* months[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    time_t t = file->fts_statp->st_mtime;
    if(flagsInL.if_t_Is_True){
        if(flagsInL.if_c_Is_True){
                t = file->fts_statp->st_ctime;
        }else if(flagsInL.if_u_Is_True){
                t = file->fts_statp->st_atime;
        }
    }
    timestructure = *localtime(&t);
    char* permission=generateSTmodestring(file->fts_statp,permissions);
    usernameAndGroupName = getUsername(file->fts_statp->st_uid,file->fts_statp->st_gid);
    /* if the flag is -n we won't convert the username and groupname id to string */
    if(file->fts_statp){
        if(flagsInL.if_n_Is_True){
            sprintf(usernameAndGroupName.username,"%d", file->fts_statp->st_uid);
            sprintf(usernameAndGroupName.groupname,"%d",file->fts_statp->st_gid);
        }
        if(flagsInL.if_i_Is_True){
            printf("%*lu ",dimensions.inodeWidth,file->fts_statp->st_ino);
        }
        if(flagsInL.if_s_Is_True){
            if(flagsInL.if_h_Is_True){
            char* sizeInHumanReadable=hPrintableFormat(file->fts_statp->st_blocks*BLOCKSIZE512,BLOCKSIZE1024);
            printf("%*s",dimensions.blocksWidth,sizeInHumanReadable);
            free(sizeInHumanReadable);
            }else{
            blocksize=blocksizeCalculator(file->fts_statp->st_blocks);
                        if(errorInBlockSize){
                            errorInBlockSize=false;
                        }
            printf("%*li ",dimensions.blocksWidth, blocksize);
            }
        }
        if(S_ISBLK(file->fts_statp->st_mode)||S_ISCHR(file->fts_statp->st_mode)){
            sprintf(filesize,"%d, %d",major(file->fts_statp->st_rdev),minor(file->fts_statp->st_rdev));
        }else
        {
            if(flagsInL.if_h_Is_True){
                filesize=hPrintableFormat(file->fts_statp->st_size,BLOCKSIZE1024);
            }else{
                sprintf(filesize,"%lu",file->fts_statp->st_size);
            }
        }
        
        checkForNonPrintableCharacters(file->fts_name);
        printableSignString=printSign(file,flagsInL);
        if(file->fts_info==FTS_SL||file->fts_info==FTS_SLNONE){
             /* case when your arguement is a symlink itself */
             if(file->fts_level == 0)
                sprintf(fileNameInCaseofSymbolicLnk, "%s", file->fts_path);
            else 
                sprintf(fileNameInCaseofSymbolicLnk, "%s/%s", file->fts_accpath,file->fts_name);
            if(readlink(fileNameInCaseofSymbolicLnk,pathInCaseOfSymbolicLink,sizeof(pathInCaseOfSymbolicLink)-1)==-1){
                fprintf(stderr,"Error: %s\n",strerror(errno));
                return;
            }
            strcat(printableSignString," -> ");
            strcat(printableSignString,pathInCaseOfSymbolicLink);
        }
        printf("%*s %*d %*s %*s %*s %*s %*d %02d:%02d %-s\n",whiteSpaces11,permission,
                                                        dimensions.numberOfLinks,
                                                        file->fts_statp->st_nlink,
                                                        dimensions.userNameWidth,
                                                        usernameAndGroupName.username,
                                                        dimensions.groupNameWidth,
                                                        usernameAndGroupName.groupname,
                                                        dimensions.numberOfBytesWidth,
                                                        filesize,
                                                        whiteSpaces3,
                                                        months[timestructure.tm_mon],
                                                        whiteSpaces2,
                                                        timestructure.tm_mday,
                                                        timestructure.tm_hour,
                                                        timestructure.tm_min,
                                                        printableSignString);
        free(printableSignString);
        free(filesize);
    }
}
void traverseInCaseOfDFlag(FTS* directoryTree,struct LsComponentForLOption dimensions,struct Flags flagsInD){
    FTSENT* file;
    while((file=fts_read(directoryTree))!=NULL){
        if(file->fts_info==FTS_DP){
            continue;
        }
        if(file->fts_info==FTS_D||file->fts_info==FTS_F){
                        if(file->fts_name[0]=='.' && !flagsInD.if_a_Is_True && !flagsInD.if_A_Is_True){
                            file = file->fts_link;
                            continue;
                        }
                    }
        /* calling the printing function  */
        printInNormalForm(file,dimensions,flagsInD);
        
        if(flagsInD.if_d_Is_True){
            fts_set(directoryTree,file,FTS_SKIP);
        }
    }
}
/* this function calculates the width for each cell and generates the maximum width which would be required */
struct LsComponentForLOption calculateWidthOfEachCell(FTS* directoryTree,struct Flags flagsWidth){
    FTSENT* file,*node;
    unsigned int maxInodeWidth = 0, maxUserNameIdWidth = 0,maxNumberOfLinks=0, maxblocksWidth=0, maxGroupNameWidth =0, maxBlockSizeWidth =0, maxFileNameWidth = 0;
    char bufferForMaxInodeWidth[BUFFERSIZE20],bufferForMaximumBlocksWidth[BUFFERSIZE20], bufferForMaxNumberOfLinks[BUFFERSIZE80], bufferForMaxBlockSizeWidth[BUFFERSIZE80];
    struct LsComponentForLOption dimensions;
    int i=0;
    long blocksize,total=0;
        while((file=fts_read(directoryTree))){
            total=0;
            switch (file->fts_info) {
                case FTS_DNR: fprintf(stderr,"cannot read the directory %s",file->fts_name);
                        break;
                case FTS_DC: fprintf(stderr,"Directory is forming a cycle may be related to this file: %s\n",file->fts_name);
                            break;
                case FTS_ERR: fprintf(stderr,"Error while reading the directory");
                            break;
                case FTS_NS: fprintf(stderr,"%s: No such file or directory \n",file->fts_name);/* stat() error */
                            break;
                case FTS_DP: continue;
                }
                /* Main traversal of directory by children which entirely runs on children of the current file. fts_level is
                applied later to stop the traversal after one level*/
                node = fts_children(directoryTree,0);
                while(node != NULL){
                    if(flagsWidth.if_d_Is_True){
                        node=node->fts_parent;
                    }

                    struct UsernameAndGroupName usernameAndGroupNameStruct;
                    /* Running for directory, file and symnolic link, remember to consider the edge cases! */
                            if(node->fts_statp!=NULL){
                                /* storing the maximum length of inode cell                         */
                                if(node->fts_statp->st_ino){
                                    sprintf(bufferForMaxInodeWidth,"%lu",node->fts_statp->st_ino);
                                    if(maxInodeWidth<strlen(bufferForMaxInodeWidth)){
                                        maxInodeWidth=strlen(bufferForMaxInodeWidth);
                                    }
                                }
                                /* storing maximum number of links */
                                if(node->fts_statp->st_nlink){
                                    sprintf(bufferForMaxNumberOfLinks,"%d",node->fts_statp->st_nlink);
                                    if(maxNumberOfLinks<strlen(bufferForMaxNumberOfLinks)){
                                        maxNumberOfLinks=strlen(bufferForMaxNumberOfLinks);
                                    }
                                }
                                if(node->fts_statp->st_blocks>=0){
                                    if(!flagsWidth.if_h_Is_True){
                                        blocksize=blocksizeCalculator(node->fts_statp->st_blocks);
                                        if(errorInBlockSize){
                                            errorInBlockSize=false;
                                        }
                                        sprintf(bufferForMaximumBlocksWidth,"%li",blocksize);
                                    }else if(flagsWidth.if_h_Is_True){
                                        blocksize=blocksizeCalculator(node->fts_statp->st_blocks*BLOCKSIZE512);
                                        if(errorInBlockSize){
                                            errorInBlockSize=false;
                                        }
                                        char* sizeInHumanReadable=hPrintableFormat(node->fts_statp->st_blocks*BLOCKSIZE512,BLOCKSIZE1024);
                                        sprintf(bufferForMaximumBlocksWidth,"%s",sizeInHumanReadable);
                                        free(sizeInHumanReadable);
                                    }
                                        total+=node->fts_statp->st_blocks;
                                        if(maxblocksWidth<strlen(bufferForMaximumBlocksWidth)){
                                            maxblocksWidth=strlen(bufferForMaximumBlocksWidth);
                                        }
                                }
                                /* storing the length of username and groupname by calling getusername function to read username and groupname from PASSWORD file         */
                                if((signed)node->fts_statp->st_uid>=0 && (signed)node->fts_statp->st_gid>=0){
                                    usernameAndGroupNameStruct = getUsername(node->fts_statp->st_uid,node->fts_statp->st_gid);
                                    if(usernameAndGroupNameStruct.username!=NULL){

                                        if(maxUserNameIdWidth<(unsigned)strlen(usernameAndGroupNameStruct.username)){
                                            maxUserNameIdWidth=strlen(usernameAndGroupNameStruct.username);
                                        }
                                    }
                                    if(usernameAndGroupNameStruct.groupname!=NULL){
                                        if(maxGroupNameWidth<strlen(usernameAndGroupNameStruct.groupname)){
                                            maxGroupNameWidth = strlen(usernameAndGroupNameStruct.groupname);
                                        }
                                    }
                                }
                                 /* storing the max block size width for the file */
                                if(node->fts_statp->st_size || !S_ISBLK(node->fts_statp->st_mode)|| !S_ISCHR(node->fts_statp->st_mode)){
                                    char* sizeInHumanReadable;
                                    bool ifBLKORCHR = false;
                                    unsigned int blocksizeLength=0;
                                    /* if h is not true then calculate normal else call hprintableblocksize width */
                                    if(S_ISBLK(node->fts_statp->st_mode)||S_ISCHR(node->fts_statp->st_mode)){
                                        ifBLKORCHR=true;
                                        sprintf(bufferForMaxBlockSizeWidth,"%d, %d",major(node->fts_statp->st_rdev),minor(node->fts_statp->st_rdev));
                                    }else{
                                        if(!flagsWidth.if_h_Is_True){
                                            sprintf(bufferForMaxBlockSizeWidth,"%lu",node->fts_statp->st_size);
                                        }else if(flagsWidth.if_h_Is_True){
                                            sizeInHumanReadable=hPrintableFormat(node->fts_statp->st_size,BLOCKSIZE1024);
                                            sprintf(bufferForMaxBlockSizeWidth,"%s",sizeInHumanReadable);
                                            free(sizeInHumanReadable);
                                        }
                                    }
                                    if(ifBLKORCHR){
                                        blocksizeLength  = strlen(bufferForMaxBlockSizeWidth)+2;
                                    }else{
                                        blocksizeLength  = strlen(bufferForMaxBlockSizeWidth);
                                    }
                                    if(maxBlockSizeWidth<blocksizeLength){
                                        maxBlockSizeWidth=blocksizeLength;
                                    };
                                }
                                /* assuming there will always be a name for a file */
                                if(maxFileNameWidth<strlen(node->fts_name)){
                                    checkForNonPrintableCharacters(node->fts_name);
                                    maxFileNameWidth=strlen(node->fts_name);
                                }

                            }
                            if(flagsWidth.if_d_Is_True){
                                break;
                            }
                            node = node->fts_link;
                            continue;
                    if(flagsWidth.if_d_Is_True){
                        break;
                    }
                    node = node->fts_link;
                }

                dimensions.total[i]=total;
                i++;
                
                if(flagsWidth.if_d_Is_True){
                    fts_set(directoryTree,file,FTS_SKIP);
                }

                /* This particular section checks that whether the following is recursive or not if it is not then we 
                will stop recursion at the end of the traversal */
                if(!flagsWidth.if_R_Is_True){
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
