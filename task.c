#include "os_file.h"
#include <stdio.h>

#define INIT_MEMORY 1

typedef struct Object Object;
typedef struct Manager Manager;

struct Object {
    Object *parent;
    Object* *child;
    int childCnt;
    int childMem;
    char* name;
    int size;
    int isFile;
};

struct Manager {
    Object* obj;
    int objCnt;
    int objMem;
    int capacity;
    Object *curDir;
    Object *root;
};

Manager mn;


void push_child(Object *ob, Object *pnt) {
    if(ob->childMem == 0) {
        ob->childMem = INIT_MEMORY;
        ob->child = (Object**) malloc(sizeof(Object*) * ob->childMem);
    }
    if(ob->childCnt == ob->childMem) {
        ob->childMem *= 2;
        ob->child = (Object**) realloc(ob->child, sizeof(Object*) * ob->childMem);
    }
    ob->child[ob->childCnt++] = pnt;
}

Object* push_obj(Object obj) {
    if(mn.objMem == 0) {
        mn.objMem = INIT_MEMORY;
        mn.obj = (Object*) malloc(sizeof(Object) * mn.objMem);
    }
    if(mn.objCnt == mn.objMem) {
        mn.objMem *= 2;
        mn.obj = (Object*) realloc(mn.obj, sizeof(Object) * mn.objMem);
    }
    mn.obj[mn.objCnt++] = obj;
    return &mn.obj[mn.objCnt - 1];
}

const char* getName(const char* path) {
    for(int i = strlen(path) - 1; i >= 0; i--)
        if(path[i] == '/')
            return (path + i + 1);
}

int isPathCorrect(const char* path) {
    if(!strcmp(getName(path), ".") || !strcmp(getName(path), ".."))
        return 0;
    return 1;
}

Object* chooseChild(Object* ob, const char *name) {

}

const char** getList(const char* path, int n) {
    char *buf = (char*) malloc(strlen(path));
    char* list[n];
    int p = 0, k = 0;
    for(int i = 0; i < strlen(path) + 1; i++) {
        if(path[i] == '/' || path[i] == '\0') {
            for (int j = 0; j < p; ++j) {
                printf("%c", buf[j]);
            }
            printf("\n");
            buf[p] = '\0';
            list[k++] = buf;
            p = 0;
            continue;
        }
        buf[p] = path[i];
        p++;
    }
    return (const char **) list;
}

int getDirNum(const char* path) {
    int n = 0;
    if(path[0] == '/' && path[1] == '\0')
        return 0;
    for(int i = 0; i < strlen(path); i++)
        if(path[i] == '/')
            n++;
    return n + 1;
}

Object* getDirByPath(const char* path) {
    Object *ob;
    const char **list;
    int n;
    if(path[0] == '/') {
        ob = mn.root;
        n = getDirNum(path + 1);
        list = getList(path + 1, n);

        for(int i = 0; i < n; i++) {
           printf("%s\n", list[i]);
        }
    }
    else {
        ob = mn.curDir;
        list = getList(path, n);

    }

}

int createFM(int disk_size)
{
    if(mn.curDir != NULL || disk_size < 0)
        return 0;
    Object rt;
    rt.name = "/";
    rt.size = 0;
    rt.isFile = 0;
    rt.childMem = 0;
    rt.childCnt = 0;
    mn.capacity = disk_size;
    mn.root = push_obj(rt);
    mn.curDir = mn.root;
    return 1;
}

int destroyFM()
{
    if(mn.curDir == NULL)
        return 0;
    for(int i = 0; i < mn.objCnt; i++) {
        for(int j = 0; j < mn.obj[i].childCnt; j++)
            free(mn.obj[i].child[j]);
    }
    free(mn.obj);
    mn.curDir = NULL;
    mn.objCnt = 0;
    mn.objMem = 0;
    return 1;
}

int createDir(const char* path) {
    if(mn.curDir == NULL || !isPathCorrect(path))
        return 0;
    getDirByPath(path);

}

void getCurDir(char* dst) {

    strcpy(dst, "test");
}

void setup_file_manager(file_manager_t *fm) {
    mn.objMem = 0;
    mn.objCnt = 0;
    mn.curDir = NULL;
    fm->create = createFM;
    fm->destroy = destroyFM;
    fm->get_cur_dir = getCurDir;
    fm->create_dir = createDir;
}

int main()
{
    file_manager_t fm;
    setup_file_manager(&fm);
    createFM(12);
    printf("%p\n", mn.curDir);
    //destroyFM();
    printf("%p\n", mn.curDir);
    createDir("/kek/cheburek/lol");

    return 0;
}
