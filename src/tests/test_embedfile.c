#include <stdio.h>


extern const char sym_test[];
extern const size_t sym_test_len;

int main(int argc, char** argv){
   (void)argc;
   (void)argv;
   
   fprintf(stdout,"%s",sym_test);
  
   return 0;
}