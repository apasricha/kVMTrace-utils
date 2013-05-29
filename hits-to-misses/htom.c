#include <stdio.h>
#include <stdlib.h>

int
main (int argc, char** argv)
{

  int footprint = strtol(argv[1], NULL, 10);
  unsigned long long int* x = (unsigned long long int*)malloc(sizeof(unsigned long long int) * (footprint + 1));
  int i;
  unsigned long long int misses_so_far = 0;

  for (i = 0; i < footprint + 1; i++) {
    x[i] = 0;
  }

  while (!feof(stdin)) {
    int memsize;
    unsigned long long int hits;
    fscanf(stdin, "%ld %qd\n", &memsize, &hits);
    if ((0 <= memsize) && (memsize < footprint + 1)) {
      x[memsize] = hits;
    } else {
      fprintf(stderr, "ERROR: index = %ld, footprint = %ld\n", memsize, footprint);
    }
  }

  for (i = footprint; i > 0; i--) {
    misses_so_far += x[i];
    x[i] = misses_so_far;
  }

  for (i = 0; i < footprint + 1; i++) {
    printf("%ld %qd\n", i, x[i]);
  }

  return 0;

}
