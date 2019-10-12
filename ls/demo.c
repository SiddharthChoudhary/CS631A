#include <stdio.h>
#include<string.h>
#include<math.h>
#include<dirent.h>
#include<fts.h>
#define BUFFERSIZE1024 1024
void traverseIncaseofD(FTS* fi);
void hPrintableFormat(off_t st_blocks);
int main()
{
   long a = 1227;
   char* argvs[]= {"/","/etc","/lib","testDIr",NULL};
   traverseIncaseofD(fts_open(argvs,FTS_PHYSICAL|FTS_NOCHDIR,NULL));
   return 0;
}
void hPrintableFormat(off_t st_blocks){
    float divisor = st_blocks,count=0;
    char ch='B';
    if(st_blocks<999){
       printf("%li%c",st_blocks,ch);
    }else{
      while(round(divisor/BUFFERSIZE1024)>0.0)
      {
         divisor=(float)((float)divisor/(float)BUFFERSIZE1024);
         count++;
      }
      if(divisor<10){
         if(count==1){
            printf("%1.1fK",divisor);
         }else if(count==2){
            printf("%1.1fM",divisor);
         }else if(count==3){
            printf("%1.1fG",divisor);
         }
      }else{
         if(count==1){
            printf("%3dK",(int)round(divisor));
         }else if(count==2)
         {
            printf("%3dM",(int)round(divisor));
         }
         else if(count==3)
         {
            printf("%3dG",(int)round(divisor));
         }
      }
    }
};

void traverseIncaseofD(FTS* directoryTree){
   FTSENT   *file, *node;
   while ((file=fts_read(directoryTree))!=NULL)
   {
      if(file->fts_info==FTS_D){
         printf("%s\n",file->fts_name);
      }
      // if(file->fts_info==FTS_DP){
      //    continue;
      // }
      // while((node=fts_children(directoryTree,0))!=NULL){
      //    node=node->fts_parent;
         
      //    printf("%s\n",node->fts_name);
      
      //    break;
      // };
      fts_set(directoryTree,file,FTS_SKIP);
   }
   
}