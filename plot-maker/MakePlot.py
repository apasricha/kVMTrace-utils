#! /usr/bin/env python3

import sys

def main (argv): #Hint: Run hits file through hits-to-misses utility to generate misses file.

    if len(argv) != 5:
        sys.stderr.write("Usage: MakePlot.py <Misses File> <File of RamInfo> < File of SSDInfo> <File of HDDInfo>\n")
        sys.exit()
  
    footprint = footprint_finder(argv[1])
    read_file (argv[2], argv[3], argv[4])
    filelist = make_plots(footprint)
    gnuplot_scripter(filelist, argv[1])
    
def footprint_finder(fname): #returns footprint and creates misses list that will be accessed by make_plot()
    with open(fname) as f:
        global misses
        misses = [0]
        i = 0
        for line in f:
            i = i + 1
            misses.append(float(line.split()[1]))
            
    return i

def read_file(f1, f2, f3): #README file has details on format of input file
    with open(f1) as f:
        global ram_accesstime
        ram_accesstime = float(f.readline())
        global ram_costcap
        ram_costcap = float(f.readline())
    with open(f2) as f:
        global ssd_accesstime
        ssd_accesstime = float(f.readline())
        global ssd_costcap
        ssd_costcap = float(f.readline())
    with open(f3) as f:
        global hdd_accesstime
        hdd_accesstime = float(f.readline())
        global hdd_costcap
        hdd_costcap = float(f.readline())
        
def make_plots(footprint): #produces data for three kinds of plots: cost-delay, cost-performance and cost-performance per delay
    #for the filelists below, case 1: no SSD, case 2: no HDD. Remaining cases vary SSD and RAM with HDD = footprint.
    costdelay_filelist = ['"costdelayRAM:NoSSD", "costdelayRAM:NoHDD"'] 
    costperformance_filelist = ['"costperfRAM:NoSSD", "costperfRAM:NoHDD"']
    costppd_filelist = ['"costppdRAM:NoSSD", "costppdRAM:NoHDD"'] # ppd = performace per dollar 
    x = 1000 #10000 pages was chosen as the realistic minimum for how small RAM could be. This can be changed as per discretion. 
    filelist = [costdelay_filelist, costperformance_filelist, costppd_filelist] 
    
    with open ("costdelayRAM:NoSSD", 'w') as cdNoSSD, open ("costdelayRAM:NoHDD", 'w') as cdNoHDD, open ("costperfRAM:NoSSD", 'w') as cpNoSSD, open ("costperfRAM:NoHDD", 'w') as cpNoHDD, open ("costppdRAM:NoSSD", 'w') as cppdNoSSD, open ("costppdRAM:NoHDD", 'w') as cppdNoHDD:
        curvecount = 20
        counterx = 1;
        while counterx <= curvecount+1: 
            cdname = "costdelayRAM:" + str(x)
            cpname = "costperfRAM:" + str(x)
            cppdname = "costppdRAM:" + str(x)
            
            if counterx <= curvecount:
                costdelay_filelist.append('"' + cdname + '"')
                costperformance_filelist.append('"' + cpname + '"')
                costppd_filelist.append('"' + cppdname + '"')
                        
            #No SSD case: 
            NoSSDdelay = hdd_accesstime*misses[int(x)]
       
            NoSSDcost = ram_costcap*x + hdd_costcap*footprint
            
            NoSSD_cdstring = str(NoSSDcost) + " " + str(NoSSDdelay) + '\n'
            cdNoSSD.write(NoSSD_cdstring)

            if NoSSDdelay >0:
                NoSSD_cpstring = str(NoSSDcost) + " " + str(1/NoSSDdelay) + '\n'
                cpNoSSD.write(NoSSD_cpstring)
            
                NoSSD_cppdstring = str(NoSSDcost) + " " + str(1/(NoSSDdelay*NoSSDcost)) + '\n'
                cppdNoSSD.write(NoSSD_cppdstring)
            
            #No HDD case: 
            NoHDDdelay = ssd_accesstime*misses[int(x)]
            NoHDDcost = ram_costcap*x + ssd_costcap*footprint
            
            NoHDD_cdstring = str(NoHDDcost) + " " + str(NoHDDdelay) + '\n'
            cdNoHDD.write(NoHDD_cdstring)
            
            if NoHDDdelay>0:
                NoHDD_cpstring = str(NoHDDcost) + " " + str(1/NoHDDdelay) + '\n'
                cpNoHDD.write(NoHDD_cpstring)
                NoHDD_cppdstring = str(NoHDDcost) + " " + str(1/(NoHDDdelay*NoHDDcost)) + '\n'
                cppdNoHDD.write(NoHDD_cppdstring)
           
            with open(cdname, 'w') as cdfile,open (cpname, 'w') as cpfile, open (cppdname, 'w') as cppdfile:
                y = x #SSD cannot sensibly be smaller than RAM                
                pointcount = 100
                countery = 1
       
                while countery <= pointcount:
                    delay = ssd_accesstime*misses[int(x )] + hdd_accesstime*misses[int(y)]
                    cost = ram_costcap*x + ssd_costcap*y + hdd_costcap*footprint
              
                  
                    
                    cdstring = str(cost) + " " + str(delay) + '\n'
                    cdfile.write(cdstring)
                    if delay>0:
                        cpstring = str(cost) + " " + str(1/delay) + '\n'              
                        cpfile.write(cpstring)
                        cppdstring = str(cost) + " " + str(1/(delay*cost))+ '\n'
                        cppdfile.write(cppdstring)

                    y = ((footprint-x)*countery)/pointcount + x
                    countery = countery + 1
                    
                    
            x = (counterx/curvecount)* (footprint-1000) + 1000        
            counterx = counterx + 1
            

        
           #x = x + int(footprint/20)
         
    return filelist
  
def gnuplot_scripter(filelist, misses_path):#creates gnuplot script to produce plots from make_plots data
    

    print('#!/usr/bin/env gnuplot\n')
    print('set style data linespoints\n')
    print('set logscale y\n')
    print('set terminal postscript enhanced color\n')
    print('set nokey')
  

   
    #Creates the cost-delay pdf   
    print('set title "costdelay' + misses_path +'"' )
    print('set xlabel "Cost (dollars)"')
    print('set ylabel "Delay (nanoseconds)"')
    print('set yrange [1:]')
    print ("set output '| ps2pdf - costdelay" + misses_path + ".pdf'")
    cdlist = ",".join(filelist[0])
    print ('plot ' + cdlist + '\n')

    #Creates the cost-performance pdf
    print('set title "costperformance' + misses_path + '"' )
    print('set xlabel "Cost (dollars)"')
    print('set ylabel "Performance (gigahertz)"')
    print('set autoscale y')
    print ("set output '| ps2pdf - costperf" + misses_path + ".pdf'") 
    #print('set yrange [1:]')
    cplist = ",".join(filelist[1])
    print ('plot ' + cplist + '\n')
    
    #Creates the cost-performance per delay pdf    
    print('set title "costppd' + misses_path + '"' )
    print('set xlabel "Cost (dollars)"')
    print('set ylabel "Performance per dollar (giaghertz/dollar)"')
    print ("set output '| ps2pdf - costppd" + misses_path + ".pdf'")
    # print('set yrange [1:]')
    cppdlist = ",".join(filelist[2])
    print ('plot ' + cppdlist + '\n')

 
if __name__ == "__main__":
    main(sys.argv)
