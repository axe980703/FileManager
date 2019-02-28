#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include "os_mem.h"

using namespace std;

class Page
{
public:
    int pageNum;
    Page() : pageNum(-1) {};
};


class PageManager
{
    private:
        static bool initialized;
        static int pageSize;
        static int pageCount;
        static vector<Page*> *pages;

    public:
        static int create(int size, int num_pages);
        static int destroy();
        static int page_num(mem_handle_t block);
        static int read(int addr, int size);
        static int write(int addr, int size);
};

bool PageManager::initialized = false;
int PageManager::pageSize = 0;
int PageManager::pageCount = 0;
vector<Page *> *PageManager::pages = nullptr;

int PageManager::create(int size, int num_pages)
{
    if (PageManager::initialized || size <= 0 || num_pages <= 0) return 0;

    PageManager::initialized = true;
    PageManager::pageSize = size;
    PageManager::pageCount = num_pages;
    PageManager::pages = new vector<Page*>();

    return 1;
}

int PageManager::destroy()
{
    if (!PageManager::initialized) return 0;

    for (auto &page : *PageManager::pages)
        delete page;
    delete PageManager::pages;

    PageManager::initialized = false;

    return 1;
}

int PageManager::page_num(mem_handle_t block)
{
    if (!PageManager::initialized) return -1;

    int page = block.addr / PageManager::pageSize;
    if (page != (block.addr + block.size - 1) / PageManager::pageSize)
        return -1;

    return page;
}

int PageManager::read(int addr, int size)
{
    if (!PageManager::initialized) return -1;

    int page = addr / PageManager::pageSize;
    if (page != (addr + size - 1) / PageManager::pageSize)
        return -1;

    bool pageFound = false;
    for (auto i_page = PageManager::pages->begin(); !pageFound && i_page != PageManager::pages->end(); i_page++)
        if ((*i_page)->pageNum == page)
            pageFound = true;

    if (!pageFound)
    {
        if (PageManager::pages->size() == PageManager::pageCount)
            PageManager::pages->erase(pages->begin());

        Page *_page = new Page();
        _page->pageNum = page;
        PageManager::pages->push_back(_page);
    }

    cout << "read " << addr << " " << size << endl;

    return 1;
}

int PageManager::write(int addr, int size)
{
    if (!PageManager::initialized) return -1;

    int page = addr / PageManager::pageSize;
    if (page != (addr + size - 1) / PageManager::pageSize)
        return -1;

    bool pageFound = false;
    for (auto i_page = PageManager::pages->begin(); !pageFound && i_page != PageManager::pages->end(); i_page++)
        if ((*i_page)->pageNum == page)
            pageFound = true;

    if (!pageFound)
    {
        if (PageManager::pages->size() == PageManager::pageCount)
            PageManager::pages->erase(pages->begin());

        Page *_page = new Page();
        _page->pageNum = page;
        PageManager::pages->push_back(_page);
    }

    cout << "write " << addr << " " << size << endl;

    return 1;
}

void setup_memory_manager(memory_manager_t* mm)
{
    mm->create = PageManager::create;
    mm->destroy = PageManager::destroy;
    mm->page_num = PageManager::page_num;
    mm->read = PageManager::read;
    mm->write = PageManager::write;
}
