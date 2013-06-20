#! /usr/bin/env python3

import sys

def main (argv): #Run hits file through hits-to-misses utility to generate misses file. 

    if len(argv) != 5: 
        sys.stderr.write("Usage: MakePlot.py <Misses File> <File of RamInfo> < File of SSDInfo> <File of HDDInfo>\n")
        sys.exit()
    
    footprint = footprint_finder(argv[1])
    read_file (argv[2], argv[3], argv[4])
    filelist = make_plot(footprint)
    gnuplot_scripter(filelist, argv[1])
    
def footprint_finder(fname): #returns footprint and creates misses list that will be accessed by make_plot()
    with open(fname) as f:
        global misses
        misses = [0]
        i = 1
        for line in f:
            i = i + 1
            misses.append(float(line.split()[1]))
            
    return i          

def read_file(f1, f2, f3):	# format of the input files: two lines. first latency in ns, second cost per page in dollars
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
        
def make_plot(footprint):
    
    filelist = ['"RAM:NoSSD"', '"RAM:NoHDD"'] #case 1: no SSD, case 2: no HDD. Remaining cases vary SSD and RAM with HDD = footprint.
    x = 10000

    with open ("RAM:NoSSD", 'w') as file1, open ("RAM:NoHDD", 'w') as file2:
        
        while x <footprint:
            name = "RAM:" + str(x)
            performance_dollar_name = "PPD-" + name
            filelist.append('"' + name + '"')
            filelist.append('"' + performance_dollar_name + '"')
            NoSSDdelay = hdd_accesstime*misses[int(x)]
            NoSSDcost = ram_costcap*x + hdd_costcap*footprint
            NoSSDstring =  str(NoSSDcost) + " " + str(NoSSDdelay) + '\n' 
            file1.write(NoSSDstring)

            NoHDDdelay = ssd_accesstime*misses[int(x)]
            NoHDDcost = ram_costcap*x + ssd_costcap*footprint
            NoHDDstring =  str(NoHDDcost) + " " + str(NoHDDdelay) + '\n' 
            file2.write(NoHDDstring)

            

            with open(name, 'w') as f, open (performance_dollar_name, 'w') as f1:
                y = x
                while y < footprint:
                    delay = ssd_accesstime*misses[int(x )] + hdd_accesstime*misses[int(y)]
                    cost = ram_costcap*x + ssd_costcap*y + hdd_costcap*footprint
                    filestring = str(cost) + " " + str(delay) + '\n' 
                    ppdstring = str(cost*delay) + " " + str(y) + '\n'
                    f.write(filestring) 
                    f1.write(ppdstring)
                    y = y +int(footprint/500) 


            x = x +int(footprint/ 20)
    
    return filelist

    
def gnuplot_scripter(filelist, misses_path):
    print('#! /usr/bin/env gnuplot\n')
    print('set style data linespoints\n')
   # print('set style data dots\n')
    print('set logscale y\n')
   # print('set nokey')
    print('set terminal postscript enhanced color')
    print ("set output '| ps2pdf - " + misses_path + ".pdf'")
    
    RAMlist = ",".join(filelist)
    print ('plot ' + RAMlist +'\n')
   # print ("pause -1\n")

   

if __name__ == "__main__":
	main(sys.argv)
