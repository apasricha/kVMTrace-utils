#! /usr/bin/env python3

import sys

def main (argv):

    if len(argv) != 5: 
        sys.stderr.write("Usage: MakePlot.py <traceName> <File of RamInfo> < File of SSDInfo> <File of HDDInfo> ")
        sys.exit()
    
    footprint = footprint_finder(argv[1])
    read_file (argv[2], argv[3], argv[4])
    make_plot(footprint)
    
def footprint_finder(fname): #returns footprint
    with open(fname) as f:
        global misses
        misses = [0]
        i = 1
        for line in f:
            i = i + 1
            misses.append(float(line.split()[1]))
            
    return i            

def read_file(f1, f2, f3):	# format of the input files: two lines. first....******   
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
    x = 1
    while x <footprint:
        name = "RAM:" + str(x)
        with open(name, 'w') as f:
            y = x
            while y < footprint:
                delay = ssd_accesstime*misses[x] + hdd_accesstime*misses[y]
                cost = ram_costcap*x + ssd_costcap*y + hdd_costcap*footprint
                filestring = str(cost) + " " + str(delay) + '\n' 
                f.write(filestring) 
                y = y +int(footprint/50) #preferred increment?
                
        x = x +int(footprint/ 6)
        print(x)

def gnuplot_scripter():
    print('gnuplot\n')
    print('set style data linespoints\n')
    print('set logscale y\n')
    RAMlist = ",".join(filelist)
    print ('plot ' + RAMlist)
    
if __name__ == "__main__":
	main(sys.argv)
