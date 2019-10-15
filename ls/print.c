#include"print.h"
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