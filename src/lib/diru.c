#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include "diru.h"

void diru_parse(const char *name, int level, on_dir_cb dir_cb_, on_file_cb file_cb_, void* user_data_) {
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;
    if (dir_cb_)dir_cb_(name, level, user_data_);

    if (!(entry = readdir(dir)))
        return;

    do {
        if (entry->d_type == DT_DIR) {
            char path[2048];
            int len = snprintf(path, sizeof (path) - 1, "%s/%s", name, entry->d_name);
            path[len] = 0;
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            diru_parse(path, level + 1, dir_cb_, file_cb_, user_data_);
        } else
            if (file_cb_)file_cb_(name, entry->d_name, level, user_data_);

    } while ((entry = readdir(dir)));
    closedir(dir);
}
