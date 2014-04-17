#ifndef STRCOMPAT_H
#define	STRCOMPAT_H

#include <stddef.h>

#ifdef	__cplusplus
extern "C" {
#endif


/* strnlen() is a POSIX.2008 addition. Can't rely on it being available so
 * define it ourselves.
 */
size_t strnlen(const char *s, size_t maxlen);
size_t strlncat(char *dst, size_t len, const char *src, size_t n);
size_t strlcat(char *dst, const char *src, size_t len);
size_t strlncpy(char *dst, size_t len, const char *src, size_t n);
size_t strlcpy(char *dst, const char *src, size_t len);



#ifdef	__cplusplus
}
#endif

#endif	/* STRCOMPAT_H */

