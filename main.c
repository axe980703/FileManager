#include <stdio.h>
#include "os_file.h"

int main()
{
    file_manager_t fm;
    setup_file_manager(&fm);
    fm.create(24);
    //destroyFM();
    printf("%d", fm.create_dir("a"));
    printf("%d", fm.create_dir("a/b"));
    printf("%d", fm.create_file("a/b/c", 12));
    printf("%d", fm.create_dir("a/b/d"));
    printf("%d", fm.create_dir("a/b/e"));
    printf("%d", fm.create_dir("a/b/d/f"));
    printf("%d", fm.create_file("a/b/c.mp3", 2));
    printf("%d\n", fm.change_dir("/a/b/d"));
    char buf[50];
    fm.get_cur_dir(buf);
    printf("%s", buf);

    return 0;
}
