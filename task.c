#include "os_file.h"
#include <stdio.h>

#define INIT_MEMORY 100

typedef struct Object Object;
typedef struct Manager Manager;

struct Object {
    Object *parent;
    Object* *child;
    int childCnt;
    int childMem;
    const char* name;
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
        printf("%s", mn.obj[0].name);
        Object *tmp = (Object*) malloc(sizeof(Object) * mn.objMem);
        for(int i = 0; i < mn.objCnt; i++)
            tmp[i] = mn.obj[i];
        free(mn.obj);
        mn.obj = tmp;
    }
    mn.obj[mn.objCnt++] = obj;
    return &mn.obj[mn.objCnt - 1];
}

const char* getName(const char* path) {
    for(int i = strlen(path) - 1; i >= 0; i--)
        if(path[i] == '/')
            return (path + i + 1);
    return path;
}

int getDirNum(const char* path) {
    int n = 0;
    for(int i = 0; i < strlen(path); i++)
        if(path[i] == '/')
            n++;
    return n + 1;
}

const char* pathToObj(const char* path) {
    if(path[0] != '/' && getDirNum(path) == 1)
        return "*";
    int pos = 0;
    for(int i = strlen(path) - 1; i >= 0; i--) {
        if(path[i] == '/') {
            pos = i;
            break;
        }
    }
    if(!pos) pos = 1;
    char *buf = (char*) malloc(sizeof(char) * (pos + 1));
    for(int i = 0; i < pos; i++)
        buf[i] = path[i];
    buf[pos] = '\0';
    return buf;
}

int isPathCorrect(const char* path) {
    if(!strcmp(getName(path), ".") || !strcmp(getName(path), "..") || !strcmp(path, "/"))
        return 0;
    return 1;
}

int isAlreadyExists(Object* ob, const char* name) {
    for(int i = 0; i < ob->childCnt; i++)
        if(!strcmp(ob->child[i]->name, name))
            return 1;
    return 0;
}

Object* nextChild(Object* ob, const char *name) {
    if(!strcmp(name, ".")) return ob;
    if(!strcmp(name, "..")) {
        if(ob == mn.root)
            return NULL;
        return ob->parent;
    }
    for(int i = 0; i < ob->childCnt; i++) {
        if(!strcmp(ob->child[i]->name, name) && !ob->isFile) {
            return ob->child[i];
        }
    }
    return NULL;
}

char** getList(const char* path, int n) {
    char **list = (char**) malloc(sizeof(char*) * n);
    int lng = strlen(path);
    int k = 0, t = 0;
    char buf[lng];
    for(int i = 0; i < lng + 1; i++) {
        if(path[i] == '/' || path[i] == '\0') {
            list[k] = (char*) malloc(sizeof(char) * (t + 1));
            list[k][t] = '\0';
            strncpy(list[k++], buf, (size_t) t);
            t = 0;
            continue;
        }
        buf[t] = path[i];
        t++;
    }
    return list;
}


Object* getDirByPath(const char* path) {
    Object *ob;
    char **list;
    int n = 0;
    if(path[0] == '/') {
        ob = mn.root;
        if(strcmp(path, "/")) {
            n = getDirNum(path + 1);
            list = getList(path + 1, n);
            for(int i = 0; i < n; i++) {
                ob = nextChild(ob, list[i]);
                if(ob == NULL)
                    return NULL;
            }
        }
    }
    else {
        ob = mn.curDir;
        if(strcmp(path, "*")) {
            n = getDirNum(path);
            list = getList(path, n);
            for(int i = 0; i < n; i++) {
                ob = nextChild(ob, list[i]);
                if(ob == NULL)
                    return NULL;
            }
        }
    }
    for(int i = 0; i < n; i ++)
        free(list[i]);
    free(list);
    return ob;
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
    const char *pth = pathToObj(path);
    Object *ob = getDirByPath(pth);
    if(ob == NULL)
        return 0;
    if(isAlreadyExists(ob, getName(path)))
        return 0;
    Object folder;
    folder.parent = ob;
    folder.name = getName(path);
    folder.childMem = 0;
    folder.childCnt = 0;
    folder.isFile = 0;
    push_child(ob, push_obj(folder));
    free((void *) pth);
    return 1;
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
    printf("%p\n", mn.root);
    printf("%d", createDir("/cheburek"));
    printf("%d", createDir("/cheburek/kek"));
    printf("%d", createDir("/cheburek/kek"));
    printf("%d", createDir("/cheburek/kek."));

    return 0;
}
