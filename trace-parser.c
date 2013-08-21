

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main (int argc, char *argv[])
{
  int o_bool, a_bool;
  float quotient, *misses;
  char *original_trace_filename, *altered_trace_filename;
  FILE *original_trace;
  FILE *altered_trace;

  misses = malloc(2 * sizeof(float));

  original_trace_filename = argv[1];
  altered_trace_filename = argv[2];

  original_trace = fopen(original_trace_filename, "r");
  altered_trace = fopen(altered_trace_filename, "r");

  o_bool = fscanf(original_trace, "%f", &misses[0]);
  a_bool = fscanf(altered_trace, "%f", &misses[1]);

  while (o_bool != '\0' && a_bool != '0') {

    quotient = misses[1]/misses[0];
    printf("%f\n", quotient);

    o_bool = fscanf(original_trace, "%f", &misses[0]);
    a_bool = fscanf(altered_trace, "%f", &misses[1]);
  }

  free(misses);

}
