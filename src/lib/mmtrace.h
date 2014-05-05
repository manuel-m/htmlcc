
#ifndef _MMTRACE_H_
#define	_MMTRACE_H_

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
        
#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif        
   

#define clean_errno() (errno == 0 ? "None" : strerror(errno))
#define log_err(M, ...) fprintf(stderr, "[ ERR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define log_warn(M, ...) fprintf(stderr, "[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#ifdef NDEBUG    
#  define log_info(M, ...) fprintf(stderr, "[INFO] " M "\n", ##__VA_ARGS__)        
#else
#  define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif    
#define log_gerr(M, ...)  { log_err(M, ##__VA_ARGS__); errno=0; goto err; }
#define die(M, ...)  { log_err(M, ##__VA_ARGS__); exit(1); }
#define die_internal(M, ...)  { log_err("internal", ##__VA_ARGS__); exit(1); }
#define check_mem(A) check((A), "Out of memory.")



#ifdef	__cplusplus
}
#endif

#endif	/* _MMTRACE_H_ */

