import csv
with open('HDD.csv', newline='') as csvfile:
    filereader = csv.reader(csvfile, dialect='excel')
    next(filereader)
    for row in filereader:
        s = "insert into RAM (brand, device, capacity, latency, price, link) values ("
        stringcol = [0, 1, 5] # here put anything you need in quotes
        i = 0;    
        for column in row:
            if stringcol.count(i) == 1:
                column = '"' + column + '"'
            if i ==3:
                j = float(column)
                j = j*1000000
                s = s + str(j) + ','
                         
            if i ==5:
                s = s + column + ')'
            else:
                s = s + column + ','    
            i = i + 1
            
        print(s)