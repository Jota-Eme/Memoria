import xlwt

files20 = {'20':['wen20a','wen20b','wen20c','wen20d']}
files30 = {'30':['wen30a','wen30b','wen30c','wen30d']}
files50 = {'50':['wen50a','wen50b','wen50c','wen50d']}
files200 = {'200':['mi4_1_200','mi4_2_200','mi4_3_200','mi4_4_200']}

allfiles = list(files20,files30,files50,files200)

excel = xlwt.Workbook()

for fileType in allfiles:
	sheet = excel.add_sheet(f'{fileType.keys()[0]}')
	for instance in fileType.values()[0]:
		with open(f'Final results/{instance}.txt') as file_read:

			for index_row, line in enumerate(file_read):
				token = line.split('/')
				items = token[1].split('-')
				for index_col, item in enumerate(items):
					sheet.write(index_row, index_col, item)
		file_read.close()
				


f.closed

plt.plot(times,solution,color='blue')

plt.show()
