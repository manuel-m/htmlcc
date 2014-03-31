#include <stdio.h>
#include "diru.h"

void print_dir_cb(const char *path, int level, void* user_data_) {
    (void) user_data_;
    (void) level;
    printf("%s\n", path);
}

void print_file_cb(const char *path, const char *basename, int level, void* user_data_) {
    (void) user_data_;
    (void) level;
    printf("%s/%s\n", path, basename);
}



int main(void) {
    diru_parse(".", 0, print_dir_cb, print_file_cb, NULL);
    return 0;
}