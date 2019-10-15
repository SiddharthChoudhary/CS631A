#include"comparator.h"

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
void* compareInCaseOfNoSorting(){
    return NULL;
}
int compareByName(const FTSENT *file1, const FTSENT *file2){
    return strcmp(file1->fts_name, file2->fts_name);
}
int compareByLastChangedTime(const FTSENT *file1, const FTSENT *file2){
    if(file2->fts_statp->st_ctimespec.tv_sec ==  file1->fts_statp->st_ctimespec.tv_sec){
        if(file2->fts_statp->st_ctimespec.tv_nsec==file1->fts_statp->st_ctimespec.tv_nsec){
            return strcmp(file1->fts_name,file2->fts_name);
        }
        if(file2->fts_statp->st_ctimespec.tv_nsec>file1->fts_statp->st_ctimespec.tv_nsec){
            return 1;
        }else{
            return -1;
        }
        return strcmp(file1->fts_name,file2->fts_name);
    }
    if(file2->fts_statp->st_ctimespec.tv_nsec > file1->fts_statp->st_ctimespec.tv_nsec){
        return 1;
    }else{
        return -1;
    };
}
int compareByLastAccessTime(const FTSENT *file1, const FTSENT *file2){
    if(file2->fts_statp->st_atimespec.tv_sec == file1->fts_statp->st_atimespec.tv_sec){
         if(file2->fts_statp->st_atimespec.tv_nsec==file1->fts_statp->st_atimespec.tv_nsec){
            return strcmp(file1->fts_name,file2->fts_name);
        }
        if(file2->fts_statp->st_atimespec.tv_nsec>file1->fts_statp->st_atimespec.tv_nsec){
            return 1;
        }else{
            return -1;
        }
        return strcmp(file1->fts_name,file2->fts_name);
    }
    if(file2->fts_statp->st_atimespec.tv_sec > file1->fts_statp->st_atimespec.tv_sec){
        return 1;
    }else{
        return -1;
    };
}
int compareByModifiedTime(const FTSENT *file1, const FTSENT *file2){
    if(file2->fts_statp->st_mtimespec.tv_sec == file1->fts_statp->st_mtimespec.tv_sec){
        if(file2->fts_statp->st_mtimespec.tv_nsec==file1->fts_statp->st_mtimespec.tv_nsec){
            return strcmp(file1->fts_name,file2->fts_name);
        }
        if(file2->fts_statp->st_mtimespec.tv_nsec>file1->fts_statp->st_mtimespec.tv_nsec){
            return 1;
        }else{
            return -1;
        }
        return strcmp(file1->fts_name,file2->fts_name);
    }
    if(file2->fts_statp->st_mtimespec.tv_sec > file1->fts_statp->st_mtimespec.tv_sec){
        return 1;
    }else{
        return -1;
    };
}
int compareBySize(const FTSENT *file1, const FTSENT *file2){
    if(file2->fts_statp->st_size==file1->fts_statp->st_size){
        return compareByName(file1,file2); 
    }
    if(file2->fts_statp->st_size > file1->fts_statp->st_size){
        return 1;
    }else{
        return -1;
    }
}

/* 
    Reversal
 */

int compareByNameReverse(const FTSENT *file1, const FTSENT *file2){
    return strcmp(file2->fts_name, file1->fts_name);
}
int compareBySizeReverse(const FTSENT *file1, const FTSENT *file2){
    if(file2->fts_statp->st_size == file1->fts_statp->st_size){
        return strcmp(file2->fts_name,file1->fts_name);
    }
    if(file1->fts_statp->st_size > file2->fts_statp->st_size){
        return 1;
    }else{
        return -1;
    };
}
int compareByModifiedTimeReverse(const FTSENT *file1, const FTSENT *file2){
    if(file1->fts_statp->st_mtimespec.tv_sec == file2->fts_statp->st_mtimespec.tv_sec){
        if(file1->fts_statp->st_mtimespec.tv_nsec==file2->fts_statp->st_mtimespec.tv_nsec){
            return strcmp(file2->fts_name,file1->fts_name);
        }
        if(file1->fts_statp->st_mtimespec.tv_nsec > file2->fts_statp->st_mtimespec.tv_nsec){
            return 1;
        }else{
            return -1;
        }
    }
    if(file1->fts_statp->st_mtimespec.tv_sec > file2->fts_statp->st_mtimespec.tv_sec){
        return 1;
    }else{
        return -1;
    };
}
int compareByLastAccessTimeReverse(const FTSENT *file1, const FTSENT *file2){
    if(file2->fts_statp->st_atimespec.tv_sec == file1->fts_statp->st_atimespec.tv_sec){
        if(file1->fts_statp->st_atimespec.tv_nsec==file2->fts_statp->st_atimespec.tv_nsec){
            return strcmp(file2->fts_name,file1->fts_name);
        }
        if(file1->fts_statp->st_atimespec.tv_nsec > file2->fts_statp->st_atimespec.tv_nsec){
            return 1;
        }else{
            return -1;
        }
    }
    if(file1->fts_statp->st_atimespec.tv_sec > file2->fts_statp->st_atimespec.tv_sec){
        return 1;
    }else
    {
        return -1;
    }
    ;
}
int compareByLastChagangedTimeReverse(const FTSENT *file1, const FTSENT *file2){
    if(file2->fts_statp->st_ctimespec.tv_sec == file1->fts_statp->st_ctimespec.tv_sec){
        if(file1->fts_statp->st_ctimespec.tv_nsec == file2->fts_statp->st_ctimespec.tv_nsec){
            return strcmp(file2->fts_name,file1->fts_name);
        }
        if(file1->fts_statp->st_ctimespec.tv_nsec > file2->fts_statp->st_ctimespec.tv_nsec){
            return 1;
        }else{
            return -1;
        }
    }
    if(file1->fts_statp->st_ctimespec.tv_sec > file2->fts_statp->st_ctimespec.tv_sec){
        return 1;
    }else{
        return -1;
    };
}