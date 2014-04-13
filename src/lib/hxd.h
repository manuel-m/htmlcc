#ifndef EMBED_H
#define	EMBED_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
  const size_t sz;
  const uint8_t* data;
  const char key[];
} mmembed_s;


#ifdef	__cplusplus
}
#endif

#endif	/* EMBED_H */

