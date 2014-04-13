#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "mmtrace.h"
#include "diru.h"

#define QR_BUFFER_SIZE (1<<10)
#define QR_NB_COLS (1<<4)


typedef struct {
    FILE* fout;
    size_t nbelem;
    const char* prefix;
} ctx_s;


static int hxd_file(FILE *out_, const char* symname_, const char* filename_) {
    int res = 0;
    uint8_t icol = 0;
    size_t sz = 0;

    if (!out_) MM_GERR("internal out_ NULL");
    if (!symname_) MM_GERR("internal symname_ NULL");
    if (!filename_) MM_GERR("internal filename_ NULL");

    FILE *f = fopen(filename_, "rb");
    if (!f) MM_GERR("read file %s", filename_);

    uint8_t buffer[QR_BUFFER_SIZE];
    memset(buffer, 0, sizeof (buffer));


    /* --> data */
    fprintf(out_, "static const uint8_t %s_data[] = {\n", symname_);

    /* --> values */
    do {
        const size_t nbelem = fread(buffer, 1, sizeof (buffer), f);
        size_t i;

        for (i = 0; i < nbelem; i++) {
            fprintf(out_, "0x%02x,", buffer[i]);
            ++icol;
            ++sz;
            icol &= ~QR_NB_COLS;
            if (!icol)puts("");
        }
        if (sizeof (buffer) != nbelem) {
            fprintf(out_, "\n");
            break;
        }
    } while (1);

    /* --> size */
    fprintf(out_, "};\n");

    /* --> full struct */
    fprintf(out_, "static const mmembed_s %s = {\n", symname_);
    fprintf(out_, "  .size = %zu;\n", sz);
    fprintf(out_, "  .data = %s_data;\n", symname_);
    fprintf(out_, "  .key = \"%s\";\n", filename_);
    fprintf(out_, "};\n\n\n\n");


end:
    if (f)fclose(f);
    return res;

err:
    res = -1;
    goto end;

}

static int hxd_header(ctx_s* pctx_) {
    int res = 0;
    if (!pctx_ || !pctx_->fout) MM_GERR("internal");
    fprintf(pctx_->fout, "#include \"hxd.h\"\n\n");

end:
    return res;

err:
    res = -1;
    goto end;

}

static void hxd_onfile(const char *path_, const char *basename_, int level_, void* user_data_) {
    (void) level_;
    ctx_s* pctx = user_data_;

    char* fullname = NULL;
    if (0 > asprintf(&fullname, "%s/%s", path_, basename_)) MM_ERR("internal error");

    char* symname = NULL;
    if (0 > asprintf(&symname, "%s_%zu", pctx->prefix, pctx->nbelem)) MM_ERR("internal error");

    hxd_file(pctx->fout, symname, fullname);

    ++(pctx->nbelem);

    if (symname)free(symname);
    if (fullname)free(fullname);
}

static int hxd_footer(ctx_s* pctx_) {
    int res = 0;
    
    if (!pctx_ || !pctx_->fout || !pctx_->nbelem ) MM_GERR("internal");
    
    fprintf(pctx_->fout, "static const mmembed_s* hxds_%s[] = {\n",pctx_->prefix);
    
    size_t i;
    for(i=0;i<pctx_->nbelem;i++){
        fprintf(pctx_->fout, "  &%s_%zu,\n",pctx_->prefix,i);
    }
    fprintf(pctx_->fout, "};\n");
end:
    return res;

err:
    res = -1;
    goto end;

}

int main(int argc, char** argv) {

    int res = 0;
    if (3 != argc) goto bad_cmd;

    ctx_s ctx = {
        .fout = stdout,
        .nbelem = 0,
        .prefix = argv[2]
    };

    hxd_header(&ctx);

    diru_parse(argv[1], 0, NULL, hxd_onfile, &ctx);
    
    hxd_footer(&ctx);

end:
    return res;

err:
    res = -1;
    goto end;

bad_cmd:
    puts("bad invocation");
    goto err;
}