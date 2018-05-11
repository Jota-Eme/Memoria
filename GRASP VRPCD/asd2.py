import matplotlib.pyplot as plt
import random as rd
import numpy as np

instance = input("Ingrese el nombre de la instancia\n")

times = []
solution = []

with open('Results/'+instance) as f:
	for line in f:
		token = line.split('-')
		times.append(float(token[0]))
		solution.append(float(token[1]))

f.closed

plt.plot(times,solution,color='blue')

plt.show()
