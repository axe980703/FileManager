#include "os_file.h"
#include <stdio.h>

#define NULLDIR -1
#define ROOT 0
#define INIT_MEMORY 16

typedef struct Object Object;
typedef struct Manager Manager;

struct Object {
    char *name;
    int isFile;
    int size;
    int parentID;
    int *childID;
    int childCnt;
    int childMem;
};

struct Manager {
    Object *obj;
    int objCnt;
    int objMem;
    int capacity;
    int curDir;
};

Manager mn;

void init_child(Object *ob) {
    ob->childMem = INIT_MEMORY;
    ob->childCnt = 0;
    ob->childID = (int*) malloc(sizeof(int) * ob->childMem);
}

void init_obj() {
    mn.capacity = 0;
    mn.curDir = NULLDIR;
    mn.objMem = 4;
    mn.objCnt = 0;
    mn.obj = (Object*) malloc(sizeof(Object) * mn.objMem);
}

///// destroy child/obj and include them into rmv child/obj  THINK ABOUT EFFECTIVE MEMORY MANAGING

void push_child(Object *ob, int x) {
    if(ob->childCnt == ob->childMem) {
        ob->childMem *= 2;
        int *tmp = (int*) malloc(sizeof(int) * ob->childMem);
        for(int i = 0; i < ob->childMem / 2; i++)
            tmp[i] = ob->childID[i];
        free(ob->childID);
        ob->childID = tmp;
    }
    ob->childID[ob->childCnt++] = x;
}

void push_obj(Object obj) {
    if(mn.objCnt == mn.objMem) {
        mn.objMem *= 2;
        Object *tmp = (Object*) malloc(sizeof(Object) * mn.objMem);
        for(int i = 0; i < mn.objMem / 2; i++)
            tmp[i] = mn.obj[i];
        free(mn.obj);
        mn.obj = tmp;
    }
    mn.obj[mn.objCnt++] = obj;
}

void rmv_child() {

}

void rmv_obj() {

}

int createFM(int disk_size)
{
    if(mn.curDir != NULLDIR)
        return 0;

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

    fm->create = createFM;
    fm->destroy = my_destroy;
    fm->get_cur_dir = my_get_cur_dir;

}

int main()
{
    file_manager_t fm;
    setup_file_manager(&fm);

    init_obj();
    Object ob1;
    ob1.name = "ob1";
    init_child(&ob1);
    push_child(&ob1, 12);
    push_child(&ob1, 13);
    push_child(&ob1, 322);
    push_obj(ob1);
    push_obj(ob1);
    push_obj(ob1);
    printf("%d\n", mn.obj[1].childID[2]);
    ob1 = mn.obj[2];
    printf("%s", ob1.name);

    return 0;
}
