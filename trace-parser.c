

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main (int argc, char *argv[])
{
  int o_bool, a_bool, ignore1, ignore2, line = 0;
  float quotient;
  long *misses;
  char *original_trace_filename, *altered_trace_filename;
  FILE *original_trace;
  FILE *altered_trace;

  misses = malloc(2 * sizeof(long));

  original_trace_filename = argv[1];
  altered_trace_filename = argv[2];

  original_trace = fopen(original_trace_filename, "r");
  altered_trace = fopen(altered_trace_filename, "r");

  o_bool = fscanf(original_trace, "%d%ld", &ignore1,&misses[0]);
  a_bool = fscanf(altered_trace, "%d%ld", &ignore2,&misses[1]);

  while (o_bool == 2 && a_bool == 2) {

    quotient = ((float)misses[1]-misses[0])/misses[0];
    //quotient = ((float)misses[1])/misses[0];
    printf("%f  %d\n", quotient,line++);

    o_bool = fscanf(original_trace, "%d%ld", &ignore1, &misses[0]);
    a_bool = fscanf(altered_trace, "%d%ld", &ignore2, &misses[1]);
  }

  free(misses);


}
