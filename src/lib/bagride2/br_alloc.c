#include <stdlib.h>
#include "br_alloc.h"

void on_alloc_buffer(uv_handle_t *h_, size_t suggested_sz_,  uv_buf_t* buf_) {
    (void) h_;
    buf_->base = (char*) calloc(suggested_sz_, sizeof (char));
    buf_->len = suggested_sz_;
}
