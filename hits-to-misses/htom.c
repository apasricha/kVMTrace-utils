#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


int
main (int argc, char** argv)
{

  // Check arguments.
  if (argc != 2) {
    fprintf(stderr, "USAGE: %s <footprint (pages)>\n", argv[0]);
    exit(1);
  }

  // Grab the footprint.
  errno = 0;
  int footprint = strtol(argv[1], NULL, 10);
  if (errno != 0) {
    fprintf(stderr, "ERROR: Unable to parse %s as an integer.\n", argv[1]);
    exit(1);
  }
  if (footprint <= 0) {
    fprintf(stderr, "ERROR: The footprint must be a positive number of pages (not %d).\n", footprint);
    exit(1);
  }

  // Create space for the histogram(s).  Recall that entry 0 is unused, so footprint+1 entries are needed.
  unsigned long long int* x =
    (unsigned long long int*)malloc(sizeof(unsigned long long int) * (footprint + 1));
  if (x == NULL) {
    fprintf(stderr, "ERROR: Unable to create histogram space for a footprint of %d.\n", footprint);
    exit(1);
  }

  // Initialize all histogram entries to 0, since the input may not contain values for all entries.
  int i;
  for (i = 0; i < footprint + 1; i++) {
    x[i] = 0;
  }

  // Assign the histogram of hits based on the input.
  i = 0;
  while (!feof(stdin)) {
    int memsize;
    unsigned long long int hits;
    i++;
    int read_result = fscanf(stdin, "%d %qd\n", &memsize, &hits);
    if (read_result != 2) {
      fprintf(stderr, "ERROR: Failed to parse hit histogram input at line %d\n", i);
      exit(1);
    }
    if ((memsize <= 0) || (footprint < memsize)) {
      fprintf(stderr,
	      "ERROR: On line %d, queue position (%d) out of range for footprint (%d)\n",
	      i,
	      memsize,
	      footprint);
      exit(1);
    }
    x[memsize] = hits; // All is well, so assign it.
  }

  // Convert to a miss histogram by summing from the right.
  unsigned long long int misses_so_far = 0;
  for (i = footprint; i > 0; i--) {
    misses_so_far += x[i];
    x[i] = misses_so_far;
  }

  // Print the miss histogram in order.
  for (i = 0; i < footprint + 1; i++) {
    printf("%d %qd\n", i, x[i]);
  }

  return 0;

}
