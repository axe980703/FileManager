#include "os_file.h"
#include <stdio.h>
#include <cstring>
#include <cstdlib>

#define INIT_MEMORY 200

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
    int size;
    Object *curDir;
    Object *root;
    int *holes;
    int holCnt;
};

Manager mn;


void push_hole(int x) {
    if(mn.holCnt == -1)
        mn.holCnt = 0;
    else if(mn.holCnt == 0)
        mn.holes = (int*) malloc(sizeof(int) * INIT_MEMORY);
    mn.holes[mn.holCnt++] = x;
}

int get_hole() {
    int x = mn.holCnt - 1;
    mn.holCnt--;
    if(mn.holCnt == 0)
        mn.holCnt = -1;
    return mn.holes[x];
}

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
        auto *tmp = (Object*) malloc(sizeof(Object) * mn.objMem);
        for(int i = 0; i < mn.objCnt; i++)
            tmp[i] = mn.obj[i];
        free(mn.obj);
        mn.obj = tmp;
    }
    if(mn.holCnt > 0) {
        int n = get_hole();
        mn.obj[n] = obj;
        return &mn.obj[n];
    }
    mn.obj[mn.objCnt++] = obj;
    return &mn.obj[mn.objCnt - 1];
}

const char* getName(const char* path) {
    for(int i = static_cast<int>(strlen(path) - 1); i >= 0; i--)
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
        for(int i = static_cast<int>(strlen(path) - 1); i >= 0; i--) {
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
        if(ob->child[i] != nullptr && !strcmp(ob->child[i]->name, name))
            return 1;
    return 0;
}

int getChildCount(Object* ob) {
    int n = 0;
    for(int i = 0; i < ob->childCnt; i++)
        if(ob->child[i] != nullptr)
            n++;
    return n;
}


int getIndex(Object* ob, Object* c) {
    for(int i = 0; i < ob->childCnt; i++)
        if(ob->child[i] == c)
            return i;
}

int getObjIndex(Object* ob) {
    for(int i = 0; i < mn.objCnt; i++)
        if(&mn.obj[i] == ob)
            return i;
}

Object* nextChild(Object* ob, const char *name) {
    if(!strcmp(name, ".")) return ob;
    if(!strcmp(name, "..")) {
        if(ob == mn.root)
            return nullptr;
        return ob->parent;
    }
    for(int i = 0; i < ob->childCnt; i++)
        if(ob->child[i] != nullptr && !strcmp(ob->child[i]->name, name) && !ob->child[i]->isFile)
            return ob->child[i];
    return nullptr;
}

Object* nextChild1(Object* ob, const char *name, int isLast) {
    if(!strcmp(name, ".")) return ob;
    if(!strcmp(name, "..")) {
        if(ob == mn.root)
            return NULL;
        return ob->parent;
    }
    for(int i = 0; i < ob->childCnt; i++)
        if(ob->child[i] != NULL && !strcmp(ob->child[i]->name, name) && (!ob->child[i]->isFile || isLast))
            return ob->child[i];
    return NULL;
}

char** getList(const char* path, int n) {
    char **list = (char**) malloc(sizeof(char*) * n);
    int lng = static_cast<int>(strlen(path));
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

void updateSizeInfo(Object* ob, int size) {
    while(ob != mn.root) {
        ob->size += size;
        ob = ob->parent;
    }
    if(ob == mn.root) {
        mn.root->size += size;
        return;
    }
}

Object* getDirByPath(const char* path, int onlyDir) {
    Object *ob;
    char **list;
    int n = 0;
    if(path[0] == '/') {
        ob = mn.root;
        if(strcmp(path, "/")) {
            n = getDirNum(path + 1);
            list = getList(path + 1, n);
            for(int i = 0; i < n; i++) {
                if(onlyDir)
                    ob = nextChild(ob, list[i]);
                else
                    ob = nextChild1(ob, list[i], (i == n - 1? 1 : 0));
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
                if(onlyDir)
                    ob = nextChild(ob, list[i]);
                else
                    ob = nextChild1(ob, list[i], (i == n - 1? 1 : 0));
                if(ob == NULL)
                    return NULL;
            }
        }
    }
    for(int i = 0; i < n; i ++)
        free(list[i]);
    return ob;
}

int createFM(int disk_size)
{
    if(mn.curDir != nullptr || disk_size < 0)
        return 0;
    Object rt;
    rt.name = "/";
    rt.size = 0;
    rt.isFile = 0;
    rt.childMem = 0;
    rt.childCnt = 0;
    mn.size = disk_size;
    mn.root = push_obj(rt);
    mn.curDir = mn.root;
    return 1;
}

int destroyFM()
{
    if(mn.curDir == nullptr)
        return 0;
    free(mn.obj);
    free(mn.holes);
    mn.holCnt = 0;
    mn.curDir = nullptr;
    mn.objCnt = 0;
    mn.objMem = 0;
    return 1;
}

int createDir(const char* path) {
    if(mn.curDir == nullptr || !isPathCorrect(path))
        return 0;
    const char *pth = pathToObj(path);
    Object *parent = getDirByPath(pth, 1);
    if(parent == nullptr)
        return 0;
    if(isAlreadyExists(parent, getName(path)))
        return 0;
    Object folder;
    folder.parent = parent;
    folder.name = getName(path);
    folder.childMem = 0;
    folder.childCnt = 0;
    folder.isFile = 0;
    folder.size = 0;
    push_child(parent, push_obj(folder));
    return 1;
}

int createFile(const char* path, int fileSize) {
    if(mn.curDir == nullptr || !isPathCorrect(path))
        return 0;
    const char *pth = pathToObj(path);
    Object *parent = getDirByPath(pth, 1);
    if(parent == nullptr)
        return 0;
    if(isAlreadyExists(parent, getName(path)))
        return 0;
    if(mn.root->size + fileSize > mn.size)
        return 0;
    updateSizeInfo(parent, fileSize);
    Object file;
    file.parent = parent;
    file.name = getName(path);
    file.childMem = 0;
    file.childCnt = 0;
    file.isFile = 1;
    file.size = fileSize;
    push_child(parent, push_obj(file));
    return 1;
}

void deleteThis(Object *ob) {
    if(ob == mn.curDir)
        mn.curDir = mn.root;
    if(ob->isFile) {
        push_hole(getObjIndex(ob));
        return;
    }
    if(!getChildCount(ob)) {
        push_hole(getObjIndex(ob));
        return;
    }
    for(int i = 0; i < ob->childCnt; i++) {
        if(ob->child[i] != nullptr) {
            deleteThis(ob->child[i]);
        }
    }
    push_hole(getObjIndex(ob));
}

int removeObj(const char* path, int recursive) {
    if(mn.curDir == nullptr || !isPathCorrect(path))
        return 0;
    Object *obj = getDirByPath(path, 0);
    if(obj == nullptr)
        return 0;
    if(getChildCount(obj) && !recursive)
        return 0;
    updateSizeInfo(obj, -obj->size);
    obj->parent->child[getIndex(obj->parent, obj)] = nullptr;
    deleteThis(obj);
    return 1;
}

int changeDir(const char* path) {
    if(mn.curDir == nullptr)
        return 0;
    Object *obj = getDirByPath(path, 1);
    if(obj == nullptr)
        return 0;
    if(obj->isFile)
        return 0;
    mn.curDir = obj;
    return 1;
}

void reverseString(char* str) {
    int len = static_cast<int>(strlen(str) - 1);
    for(int i = 0; i < len / 2 ; i++) {
        char t = str[i];
        str[i] = str[len - i];
        str[len - i] = t;
    }
}

void getCurDir(char* dst) {
    Object *ob = mn.curDir;
    if(ob == mn.root) {
        strcpy(dst, "/");
        return;
    }
    strcpy(dst, "/");
    while(ob != mn.root) {
        strcat(dst, ob->name);
        strcat(dst, "/");
        ob = ob->parent;
    }
    dst[strlen(dst) - 1] = '\0';
}

void copyOnce(Object* parent, Object* ob, const char* name, int k, int *cnt) {
    Object file;
    file.parent = parent;
    if(!k)
        file.name = name;
    else
        file.name = ob->name;
    file.childMem = 0;
    file.childCnt = 0;
    file.isFile = ob->isFile;
    *cnt += file.isFile;
    file.size = ob->size;
    Object *tmp = push_obj(file);
    push_child(parent, tmp);
    if(ob->isFile || !getChildCount(ob))
        return;
    for(int i = 0; i < ob->childCnt; i++) {
        if(ob->child[i] != nullptr) {
            copyOnce(tmp, ob->child[i], "", k + 1, cnt);
        }
    }
}

int copyObj(const char *path, const char *toPath) {
    if(mn.curDir == nullptr || !isPathCorrect(path) || !isPathCorrect(toPath))
        return -1;
    const char *pth = pathToObj(toPath);
    const char *name = getName(toPath);
    Object *ob = getDirByPath(path, 0);
    Object *to = getDirByPath(pth, 1);
    if(ob == nullptr || to == nullptr)
        return -1;
    if(mn.root->size + ob->size > mn.size)
        return -1;
    if(ob->parent == to || to->isFile)
        return -1;
    int count = 0;
    copyOnce(to, ob, name, 0, &count);
    updateSizeInfo(to, ob->size);
    return count;
}

void setup_file_manager(file_manager_t *fm) {
    mn.objMem = 0;
    mn.objCnt = 0;
    mn.curDir = nullptr;
    fm->create = createFM;
    fm->destroy = destroyFM;
    fm->create_dir = createDir;
    fm->create_file = createFile;
    fm->remove = removeObj;
    fm->change_dir = changeDir;
    fm->get_cur_dir = getCurDir;
    fm->copy = copyObj;
}
