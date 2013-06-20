#! /usr/bin/env tcsh

set l = ( `ls *.misses | awk -F '.' '{ print $1 }'` )

foreach x ($l)

	printf "${x}..."
	./MakePlot.py ${x}.misses SampleRAM SampleSSD SampleHDD >& ${x}.gnuplot
	chmod u+x ${x}.gnuplot
	./${x}.gnuplot
	mv ${x}.misses.pdf ${x}.pdf
	rm -f *RAM:*
	printf "done.\n"

end

