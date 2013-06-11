import csv
#enter your csv filename here in place of RAM.csv
with open('RAM.csv', newline='') as csvfile:
	filereader = csv.reader(csvfile, dialect='excel')
	#assumes that there is a header row which it will ignore
	next(filereader)
	for row in filereader:
		#edit as needed depending on your fields
		s = "insert into RAM (brand, type, device, capacity, latency, price, link) values ("
		#decides which columns will be put in quotes (i.e. those that need to be entered as string values)
		stringcol = [0, 1, 2, 6] 
		i = 0;	
		for column in row:
			if stringcol.count(i) == 1:
				column = '"' + column + '"'
		#change 6 to the last column as needed
			if i ==6:
				s = s + column + ')'
			else:
				s = s + column + ','
		
			i = i + 1
			
		print(s)
			
			
	
		