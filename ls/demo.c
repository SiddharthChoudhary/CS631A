#include<stdio.h>
#include<fts.h>
#include<dirent.h>
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

int main(int argc,char* argv[]){
DIR *dir;
struct dirent *currentFile;
char buf[80];
FTS *directoryTree;
FTSENT *node;
FTSENT *file;
char *argvs[]={argv[optind],NULL};
directoryTree = fts_open(argvs,FTS_LOGICAL|FTS_NOCHDIR,NULL);
int count=0;
while((file=fts_read(directoryTree))){
    switch (file->fts_info) {
        // preorder case
         case FTS_D: node = fts_children(directoryTree,0);
                    while(node != NULL){
                        printf("childrens are: %s\n",node->fts_name);
                        node = node->fts_link;
                    }
                    count++;
                    continue;
        // case FTS_F: count++;
        //             break;
		// case FTS_SL: printf("A symbolic link %s\n",file->fts_name);
        //             continue;
        // case FTS_SLNONE: printf("A symbolic link with non-existing target %s\n",file->fts_name);
        //             continue;
        case FTS_DNR: fprintf(stderr,"cannot read the directory %s",file->fts_name);
                    break;
        case FTS_DC: fprintf(stderr,"Directory is forming a cycle may be related to this file: %s\n",file->fts_name);
                    break;
		case FTS_ERR:	fprintf(stderr,"Error while reading the directory");
                    break;
		case FTS_NS: fprintf(stderr,"There is no stat available for this file %s",file->fts_name);/* stat() error */
                    break;
        case FTS_DP: 
                    continue;
		}
    }
fts_close(directoryTree);

}