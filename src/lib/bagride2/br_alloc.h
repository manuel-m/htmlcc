#ifndef BR_ALLOC_H
#define	BR_ALLOC_H

#include "uv.h"

#ifdef	__cplusplus
extern "C" {
#endif

void on_alloc_buffer(uv_handle_t *h_, size_t suggested_sz_,  uv_buf_t* buf_);


#ifdef	__cplusplus
}
#endif

#endif	/* BR_ALLOC_H */

