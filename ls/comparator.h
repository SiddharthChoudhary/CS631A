#include<fts.h>
#include<string.h>
#include<sys/stat.h>
static int (*compare)(const FTSENT *file1, const FTSENT *file2);
int comparator(const FTSENT **file1, const FTSENT **file2);

void* compareInCaseOfNoSorting();

int compareByName(const FTSENT *file1, const FTSENT *file2);
int compareByNameReverse(const FTSENT *file1, const FTSENT *file2);

int compareByLastAccessTime(const FTSENT *file1, const FTSENT *file2);
int compareByLastAccessTimeReverse(const FTSENT *file1, const FTSENT *file2);

int compareByLastChangedTime(const FTSENT *file1, const FTSENT *file2);
int compareByLastChagangedTimeReverse(const FTSENT *file1, const FTSENT *file2);

int compareByModifiedTime(const FTSENT *file1, const FTSENT *file2);
int compareByModifiedTimeReverse(const FTSENT *file1, const FTSENT *file2);

int compareBySize(const FTSENT *file1, const FTSENT *file2);
int compareBySizeReverse(const FTSENT *file1, const FTSENT *file2);