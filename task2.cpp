#include <iostream>
#include "os_mem.h"

using namespace std;


int create(int size, int num_pages) {


}

int destroy() {

}

int page_num(mem_handle_t block) {

}

int read(int addr, int size) {

}

int write(int addr, int size) {

}

void setup_memory_manager(memory_manager_t* mm) {
    mm->create = create;
    mm->destroy = destroy;
    mm->page_num = page_num;
    mm->read = read;
    mm->write = write;
}

int main() {
    memory_manager_t fm;
    setup_memory_manager(&fm);


}