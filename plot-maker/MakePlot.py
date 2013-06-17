import sys

def main():

    if len(sys.argv) != 3:
        sys.stderr.write("Usage: MakePlot.py <traceName> <File of RamInfo> < File of SSDInfo> <File of HDDInfo> ")
        sys.exit()
    
    footprint = footprint_finder(sys.argv[1])
    read_file (sys.argv[2], sys.argv[3], sys.argv[4])
    make_plot(footprint)
    
def footprint_finder(fname): #returns footprint
    with open(fname) as f:
        global misses
        misses = {0}
        i = 1
        for line in f:
            i = i + 1
            misses.append(line.split()[1])
            
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
    for x in range(1, footprint):
        name = "RAM:" + x
        with open(name, 'w') as f:
            for y in range (x, footprint):
                delay = ssd_accesstime*misses[x] + hdd_accesstime*misses[y] #is there a conversion i'm missing here?
                cost = ram_costcap*x + ssd_costcap*y + hdd_costcap*footprint
                filestring = cost + " " + delay + '\n' 
                f.write(filestring) 
                y = y + 50 #preferred increment?
                
        x = x + 50
