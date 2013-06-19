#! /usr/bin/env python3
import sys

def main (argv):

    if len(argv) != 5: 
        sys.stderr.write("Usage: MakePlot.py <traceName> <File of RamInfo> < File of SSDInfo> <File of HDDInfo> ")
        sys.exit()
    
    footprint = footprint_finder(argv[1])
    read_file (argv[2], argv[3], argv[4])
    filelist = make_plot(footprint)
    gnuplot_scripter(filelist)
    
def footprint_finder(fname): #returns footprint
    with open(fname) as f:
        global misses
        misses = [0]
        i = 1
        for line in f:
            i = i + 1
            misses.append(float(line.split()[1]))
            
    return i*1000            

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
    
    filelist = ['"RAMHDD"', '"RAMSSD"']
    x = 10000

    with open ("RAMHDD", 'w') as file1, open ("RAMSSD", 'w') as file2:
        
        while x <footprint:
            name = "RAM:" + str(x)
            filelist.append('"' + name + '"')
            RAMdelay = hdd_accesstime*misses[int(x/1000)]
            RAMcost = ram_costcap*x + hdd_costcap*footprint
            RAMstring =  str(RAMcost) + " " + str(RAMdelay) + '\n' 
            file1.write(RAMstring)

            SSDdelay = ssd_accesstime*misses[int(x/1000)]
            SSDcost = ram_costcap*x + ssd_costcap*footprint
            SSDstring =  str(SSDcost) + " " + str(SSDdelay) + '\n' 
            file2.write(SSDstring)

            

            with open(name, 'w') as f:
                y = x
                while y < footprint:
                    delay = ssd_accesstime*misses[int(x /1000)] + hdd_accesstime*misses[int(y/1000)]
                    cost = ram_costcap*x + ssd_costcap*y + hdd_costcap*footprint
                    filestring = str(cost) + " " + str(delay) + '\n' 
                    f.write(filestring) 
                    y = y +int(footprint/500) 


            x = x +int(footprint/ 20)
    
    return filelist

    
def gnuplot_scripter(filelist):
    print('#! /usr/bin/env gnuplot\n')
    print('set style data linespoints\n')
   # print('set style data dots\n')
    print('set logscale y\n')
    print('set nokey')
   # print('set terminal postscript enhanced color')
    print ("set output '| ps2pdf - output.pdf'")
    
    RAMlist = ",".join(filelist)
    print ('plot ' + RAMlist +'\n')
    print ("pause -1\n")

   

if __name__ == "__main__":
	main(sys.argv)
