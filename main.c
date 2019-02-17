#include "os_file.h"
#include <stdio.h>

int main()
{

    file_manager_t fm;
    setup_file_manager(&fm);
    int result;
    result = fm.create(10);
    result = fm.destroy();


    return 0;
}
