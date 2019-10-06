#include<fts.h>
#include<string.h>
#include<sys/stat.h>
static int (*compare)(const FTSENT *file1, const FTSENT *file2);
int comparator(const FTSENT **file1, const FTSENT **file2);
/* 
        This one is for the function pointer for comparison in fts which we will be using as
        a middleware. since fts_open takes comparator of pointer to pointer, and string comparison deals 
        with pointers we will need to have a middle function which can help casting pointer to pointer function into 
        pointer function.
*/
int comparator(const FTSENT **file1, const FTSENT **file2){
    FTSENT *file1ActualPointer = *(FTSENT**)file1;
    FTSENT *file2ActualPointer = *(FTSENT**)file2;
    /* calling the middle handle function which take care of the correct function to be called,
    based on what flags are put when */
    return compare(file1ActualPointer,file2ActualPointer);
}
int compareByName(const FTSENT *file1, const FTSENT *file2){
    return strcmp(file1->fts_name, file2->fts_name);
}
int compareByLastChagangedTime(const FTSENT *file1, const FTSENT *file2){
    return file2->fts_statp->st_ctime - file1->fts_statp->st_ctime;
}
int compareByLastAccessTime(const FTSENT *file1, const FTSENT *file2){
    return file2->fts_statp->st_atime - file1->fts_statp->st_atime;
}
int compareByModifiedTime(const FTSENT *file1, const FTSENT *file2){
    return file2->fts_statp->st_mtime - file1->fts_statp->st_mtime;
}
int compareBySize(const FTSENT *file1, const FTSENT *file2){
    return file2->fts_statp->st_size - file1->fts_statp->st_size;
}