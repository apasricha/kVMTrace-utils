#! /usr/bin/env python3



# ==================================================================================================================================
# IMPORTS

import sys
# ==================================================================================================================================



# ==================================================================================================================================
# The entry point.

def main (argv):

    # Check usage.
    if len(argv) != 1:
        print("USAGE: {0} <NO ARGUMENTS>".format(argv[0]));
        print("       stdin: space-delimited, decimal text, hit histogram");
        print("       stdout: space-delimited, decimal text, miss histogram");
        exit(1)

    # Read the hit histogram.
    histogram = read_hits()

    # Sum, right to left, to create the miss histogram.
    sum_histogram(histogram)

    # Emit the newly summed miss histogram.
    write_misses(histogram)
# ==================================================================================================================================



# ==================================================================================================================================
def read_hits ():

    # Initialize the histogram.
    histogram = [0]

    # Read from stdin until there is no more.
    for line in sys.stdin:

        # Parse the current entry.
        parsed_line = line.split()
        qpos = int(parsed_line[0])
        hits = int(parsed_line[1])

        # Add it to the histogram list.
        add_histogram_entry(histogram, qpos, hits)

    return histogram
# ==================================================================================================================================



# ==================================================================================================================================
def add_histogram_entry (histogram, qpos, value):

    # Extend the histogram to account for this new entry at its queue position.  +1 for the unused index 0.
    extension = qpos - len(histogram) + 1
    histogram.extend(extension * [0])

    # Assign this new entry.
    histogram[qpos] = value    
# ==================================================================================================================================



# ==================================================================================================================================
def sum_histogram (histogram):

    running_sum = 0
    for i in reversed(range(1, len(histogram))):
        running_sum = running_sum + histogram[i]
        histogram[i] = running_sum

    return histogram
# ==================================================================================================================================



# ==================================================================================================================================
def write_misses (histogram):

    for i in range(1, len(histogram)):
        print("{0} {1}".format(i, histogram[i]))
# ==================================================================================================================================



# ==================================================================================================================================
# Trigger to call main() when invoked from the command line, passing the command-line arguments.

if __name__ == "__main__":
   main(sys.argv)
# ==================================================================================================================================
