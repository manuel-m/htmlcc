#include <stdlib.h>
#include <stdio.h>

#define HTMLCC_BUF_LEN (256)

int main(int argc, char** argv) {

    int res = EXIT_SUCCESS;

    if (argc < 4) {
        fprintf(stderr, "USAGE: %s {sym} {rsrc} {out}\n\n"
                "  Creates {out} from the contents of {rsrc}\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    const char* sym = argv[1];
    FILE* in = fopen(argv[2], "r");
    if (!in) goto err;

    FILE* out = fopen(argv[3], "w");
    if (!out) goto err;

    fprintf(out, "#include <stddef.h>\n");
    fprintf(out, "const char %s[] = {\n", sym);

    char buf[HTMLCC_BUF_LEN];
    size_t nread = 0;
    size_t linecount = 0;
    do {
        nread = fread(buf, 1, HTMLCC_BUF_LEN, in);
        int i;
        for (i = 0; i < nread; i++) {

            if (buf[i] >= 0) {
                fprintf(out, "0x%02x,", buf[i]);
                if (++linecount == 16) {
                    fprintf(out, "\n");
                    linecount = 0;
                }
            }
        }
    } while (nread > 0);
    fprintf(out, "};\n");
    fprintf(out, "const size_t %s_len = sizeof(%s);\n\n", sym, sym);


end:
    if (in)fclose(in);
    if (out)fclose(out);
    return res;

err:
    res = -1;
    goto end;

}
