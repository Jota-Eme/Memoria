import matplotlib.pyplot as plt
import random as rd
import numpy as np

instance = input("Ingrese el nombre de la instancia\n")
vehicles_graphic = int(input("Ingrese la cantidad de vehiculos a graficar:  0 = todos\n"))

crossdock_x = []
crossdock_y = []

suplier_x = []
suplier_y = []
customer_x = []
customer_y = []

with open('instances_MI/R1_4_1_200m.txt'+instance) as f:
	read_data = f.readline()
	read_data = f.readline() 

	requests_number = int(read_data)

	read_data = f.readline()	
	read_data = f.readline()
	read_data = f.readline()

	crossdocks_number = int(read_data)

	for i in range(8):
		read_data = f.readline()


	for i in range(crossdocks_number):
		read_data = f.readline()
		token = read_data.split(" ")
		token = list(filter(None, token))
		crossdock_x.append(int(token[1]))
		crossdock_y.append(int(token[2]))


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


with open('solution.txt') as f:
	read_data = f.readline()
	vehicles = int(read_data)

	route_x = []
	route_y = []

	for i in range(vehicles):
		vehicle_x = []
		vehicle_y = []

		depot_x = crossdock_x[0]
		depot_y = crossdock_y[0]

		vehicle_x.append(depot_x)
		vehicle_y.append(depot_y)

		read_data = f.readline()
		token = read_data.split('-')
		for item in token:
			token2 = item.split(',')
			token2 = list(filter(None, token2))
			vehicle_x.append(int(token2[0]))
			vehicle_y.append(int(token2[1]))

		vehicle_x.append(depot_x)
		vehicle_y.append(depot_y)

		route_x.append(vehicle_x)
		route_y.append(vehicle_y)

f.closed

randoms = []
colors = ["red","blue","green","grey","yellow","purple","orange"]

if vehicles_graphic == 0:
	vehicles_graphic = vehicles

for i in range(vehicles_graphic):

	number = rd.randint(0, vehicles - 1)
	while number in randoms:
		number = rd.randint(0, vehicles - 1)

	plt.plot(route_x[number],route_y[number],color=colors[i%len(colors)])
	randoms.append(number)


plt.show()
