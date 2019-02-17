#include "os_file.h"
#include <stdio.h>

#define NULLDIR -1
#define ROOT 0

typedef struct Object Object;
typedef struct Manager Manager;

struct Object {
    char* name;
    int isFile;
    int size;
    int parentID;
    int *childID;
    int childCnt;
};

struct Manager {
    Object *obj;
    int objCnt;
    int capacity;
    int curDir;
};

Manager mn;


int createFM(int disk_size)
{

    return 1;
}

int my_destroy()
{
    return 1;
}


void my_get_cur_dir(char* dst) {
    strcpy(dst, "test");
}

void setup_file_manager(file_manager_t *fm) {
    mn.capacity = 0;
    mn.curDir = NULLDIR;
    mn.objCnt = 0;
    fm->create = createFM;
    fm->destroy = my_destroy;
    fm->get_cur_dir = my_get_cur_dir;

}

