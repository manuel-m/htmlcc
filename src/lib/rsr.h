#ifndef EMBED_H
#define	EMBED_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
  const size_t m_sz;
  const uint8_t* m_data;
  const char* m_type;
  const char m_key[];
} rsr_t;


#ifdef	__cplusplus
}
#endif

#endif	/* EMBED_H */

