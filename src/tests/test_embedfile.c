#include <stdio.h>


extern const char mini_html[];
extern const size_t mini_html_len;

int main(int argc, char** argv){
   (void)argc;
   (void)argv;
   
   fprintf(stdout,"%s",mini_html);
  
   return 0;
}