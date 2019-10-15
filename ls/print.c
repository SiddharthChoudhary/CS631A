#include"print.h"
#include<time.h>
#include<math.h>
char* hPrintableFormat(off_t st_blocks,int blocksize){
    float divisor = st_blocks;
    char* size = (char*)malloc(10*sizeof(char));
    if(size==NULL){
        fprintf(stderr,"error while assigning malloc block of 10 bytes \n");
    }
    int count=0;
    char ch='B';
    if(st_blocks<999){
       sprintf(size,"%li%c",st_blocks,ch);
    }else{
      while(round(divisor/blocksize)>0.0)
      {
         divisor=(float)((float)divisor/(float)blocksize);
         count++;
      }
      if(divisor<10){
         if(count==1){
            sprintf(size,"%1.1fK",divisor);
         }else if(count==2){
            sprintf(size,"%1.1fM",divisor);
         }else if(count==3){
            sprintf(size,"%1.1fG",divisor);
         }
      }else{
         if(count==1){
            sprintf(size,"%3dK",(int)round(divisor));
         }else if(count==2)
         {
            sprintf(size,"%3dM",(int)round(divisor));
         }
         else if(count==3)
         {
            sprintf(size,"%3dG",(int)round(divisor));
         }
      }
    }
    return size;
}
/* as the name suggests it comes handy while looking out for usernames and groupnames from password file */
struct UsernameAndGroupName getUsername(int uid,int gid){
    struct passwd* passwordStructure=getpwuid(uid);
    struct group* groupIdStructure = getgrgid(gid);
    struct UsernameAndGroupName userNameAndGroupName;
    /* in case if the username or groupname in password file is missing, then we will throw the uid and gid itself */
    if(passwordStructure!=NULL){
        if(passwordStructure->pw_name!=NULL){
            sprintf(userNameAndGroupName.username,"%s",passwordStructure->pw_name);
        }
    }else{
            sprintf(userNameAndGroupName.username,"%d",uid);
        }
    if(groupIdStructure!=NULL){
        if(groupIdStructure->gr_name!=NULL){
            sprintf(userNameAndGroupName.groupname,"%s", groupIdStructure->gr_name);
    }
    }else{
            sprintf(userNameAndGroupName.groupname,"%d",gid);
        }
   return userNameAndGroupName; 
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
void printinLformat(FTSENT* file,struct LsComponentForLOption dimensions, struct Flags flagsInL){
    struct tm timestructure;
    char* printableSignString;
    struct UsernameAndGroupName usernameAndGroupName;
    long blocksize;
    char fileNameInCaseofSymbolicLnk[PATH_MAX]="";
    char pathInCaseOfSymbolicLink[PATH_MAX]="";
    char permissions[BUFFERSIZE40]="";
    char *filesize= (char*)malloc(100*sizeof(char));
    if(filesize==NULL){
        fprintf(stdout,"Error: filesize is too long");
    }
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
        /* 
            this is where blocksize can vary based on if it's a block or character special file 
        */
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
    /* if the file is not executable and set userid mode is set */
    if(fstat->st_mode & S_IXUSR){
        if(fstat->st_mode & S_ISUID){
            strcat(permissions,"s");
        }else{
            strcat(permissions,"x");
        }
    }else{
        if(S_ISUID & fstat->st_mode){
            strcat(permissions,"S");
        }else{
            strcat(permissions,"-");
        }
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
        if(fstat->st_mode & S_ISGID){
            strcat(permissions,"s");
        }else{
        strcat(permissions,"x");
        }
    }else{
        if(fstat->st_mode & S_ISGID){
            strcat(permissions,"S");
        }else{
            strcat(permissions,"-");
        }
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