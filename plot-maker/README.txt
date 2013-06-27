Plot Maker

>What does it do?

This utility, as you may or may not have gathered from its title, makes plots. 
Specifically, it makes three different kinds of plots for different amounts of RAM and SSD
1. Cost vs. delay
2. Cost vs. performance
3. Cost vs. performance per dollar


>How does one run it?

There are a couple of different ways you can run this utility depending on your needs 

1. If you'd like to run the program on every .misses file in the directory, and produce all three of the graphs mentioned above (in a pdf format) say:
  ./plot-all.sh
  
2. If you'd like to run it on a specific .misses file instead, say:
  ./plot-all.sh <missesfilename>

3. If you'd like to run the program MakePlot.py directly, say:
  ./MakePlot.py <missesfilename> <RAMInfo> <SSDInfo> <HDDInfo> < <redirectfile>
  ./redirectfile
  
4. If you want to start printing output onto the screen (to debug, for instance), and supress the gnuplot plotting code:
    emacs into MakePlot.py
    comment out the gnuplot method (#gnuplot_scripter)
    Run:
    ./MakePlot.py <missesfilename> <RAMInfo> <SSDInfo> <HDDInfo> 
   
     
>How does one get the argument files to run the program?

There are misses files and sample info files in the repository.

>How does one make sense of the sample files?

They are formatted like this:
Latency (in ns)
Cost (in USD, per page - one page = 4KB)

>What work is left to be done on the code?

1.Add more kinds of plots, if any come to mind.
2.The sample files are essentially informed guesstimations I made based on the data I gathered about SSD, RAM and HDD cost, latency and capacity. That data is currently sitting in the database repository on github under kVMTrace-utilities- it would be great if code were written to connect the database to this utility, and use actual data to plot these graphs. 
3.Figuring out parametrized functions to represent all the curves (misses, and the ones plotted based on the misses) is crucial. 
4. Gather more data for the database (and use the handy python utility in the database repository to add data to the database).

>Your code isn't comprehensible, even after reading this extremely handly README!

email me at sbadri16@amherst.edu if I've forgotten to explicitly include explanations somewhere in the code/ generally did something stupid


  
  
  
