import matplotlib.pyplot as plt

instance = input("Ingrese el nombre de la instancia\n")

with open('instances_MI/'+instance) as f:
	read_data = f.readline()
	read_data = f.readline() 

	requests_number = int(read_data)

	read_data = f.readline()	
	read_data = f.readline()
	read_data = f.readline()

	crossdocks_number = int(read_data)

	for i in range(8):
		read_data = f.readline()

	crossdock_x = []
	crossdock_y = []

	for i in range(crossdocks_number):
		read_data = f.readline()
		token = read_data.split(" ")
		token = list(filter(None, token))
		crossdock_x.append(int(token[1]))
		crossdock_y.append(int(token[2]))

	suplier_x = []
	suplier_y = []
	customer_x = []
	customer_y = []

	for i in range(requests_number):
		read_data = f.readline()
		token = read_data.split(" ")
		token = list(filter(None, token))
		suplier_x.append(int(token[1]))
		suplier_y.append(int(token[2]))
		customer_x.append(int(token[5]))
		customer_y.append(int(token[6]))

f.closed

plt.scatter(crossdock_x,crossdock_y,color='green')
plt.scatter(suplier_x,suplier_y,color='red')
plt.scatter(customer_x,customer_y,color='blue')
plt.show()




'''
year=[1950,1970,1990,2010]
pop=[2.519,3.692,5.263,6.972]
plt.scatter(year,pop)
plt.show()'''

