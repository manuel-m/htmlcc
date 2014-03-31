#ifndef DIRU_H
#define	DIRU_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef void (*on_dir_cb)(const char *path, int level, void* user_data_);

typedef void (*on_file_cb)(const char *path, const char *basename, int level, 
        void* user_data_);    
    
void diru_parse(const char *name, int level, on_dir_cb dir_cb_, 
        on_file_cb file_cb_, void* user_data_);

#ifdef	__cplusplus
}
#endif

#endif	/* DIRU_H */

