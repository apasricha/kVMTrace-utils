#! /usr/bin/env tcsh

if (${#argv} > 0) then
    set l = ${argv}
else
    set l = ( `ls *.misses | awk -F '.' '{ print $1 }'` )
endif

foreach x ($l)

	printf "${x}..."
	./MakePlot.py ${x}.misses SampleRAM SampleSSD SampleHDD >& ${x}.gnuplot
	chmod u+x ${x}.gnuplot
	./${x}.gnuplot

	mv costdelay${x}.misses.pdf ${HOME}/kVMTrace-plots/costdelay_${x}.pdf
	mv costperf${x}.misses.pdf ${HOME}/kVMTrace-plots/cost_perf_${x}.pdf
	mv costppd${x}.misses.pdf ${HOME}/kVMTrace-plots/cost_ppd_${x}.pdf
	rm -f *RAM:* 
	rm -f *.gnuplot
	printf "done.\n"

end

