import os
## numpy is used for creating fake data
import numpy as np 
import matplotlib as mpl 

## agg backend is used to create plot as a .png file
mpl.use('agg')

import matplotlib.pyplot as plt 


def setBoxColors(bp):
    setp(bp['boxes'][0], color='blue')
    setp(bp['caps'][0], color='blue')
    setp(bp['caps'][1], color='blue')
    setp(bp['whiskers'][0], color='blue')
    setp(bp['whiskers'][1], color='blue')
    setp(bp['fliers'][0], color='blue')
    setp(bp['medians'][0], color='blue')

    setp(bp['boxes'][1], color='red')
    setp(bp['caps'][2], color='red')
    setp(bp['caps'][3], color='red')
    setp(bp['whiskers'][2], color='red')
    setp(bp['whiskers'][3], color='red')
    setp(bp['fliers'][1], color='red')
    setp(bp['medians'][1], color='red')




instance_group = input("Ingrese el nombre de la carpeta estatica\n")
folder_static = 'boxplot_results/'+instance_group+'_estandar'
listdir_static = os.listdir(folder_static)
listdir_static.sort()

folder_adaptive = 'boxplot_results/'+instance_group+'_adaptive'
listdir_adaptive = os.listdir(folder_adaptive)
listdir_adaptive.sort()

all_static_data = []
all_adaptive_data = []



for filename in listdir_static:
	fullpath = os.path.join(folder_static, filename)
	with open(fullpath) as file:
		content = file.readlines()
		data = [float(read_data.split("-")[9]) for read_data in content]
		all_static_data.append(data)

for filename in listdir_adaptive:
	fullpath = os.path.join(folder_adaptive, filename)
	with open(fullpath) as file:
		content = file.readlines()
		data = [float(read_data.split("-")[8]) for read_data in content]
		all_adaptive_data.append(data)


for i in range(0,len(all_static_data)):
	instance = [all_static_data[i], all_adaptive_data[i]]

	# Create a figure instance
	fig = plt.figure(figsize=(9, 6))

	# Create an axes instance
	ax = fig.add_subplot(111)

	# Create the boxplot
	bp = ax.boxplot(instance, patch_artist=True)
	ax.set_xticklabels(['Estandar', 'Adaptivo'])

	for box in bp['boxes']:
	    # change outline color
	    box.set( color='#7570b3', linewidth=2)
	    # change fill color
	    box.set( facecolor = '#1b9e77' )

	for whisker in bp['whiskers']:
		whisker.set(color='#7570b3', linewidth=2)

	for cap in bp['caps']:
		cap.set(color='#7570b3', linewidth=2)

	for median in bp['medians']:
		median.set(color='#b2df8a', linewidth=2)

	for flier in bp['fliers']:
		flier.set(marker='o', color='#e7298a', alpha=0.5)

	# Save the figure
	fig.savefig('boxplot_images/'+instance_group+'/'+instance_group+str(i)+'.png', bbox_inches='tight')
	# first boxplot pair
	# bp = boxplot(instance, positions = [0, 1], widths = 0.6)
	# setBoxColors(bp)

	# xlim(0,9)
	# ylim(ymin_lim, ymax_lim)

	# # ax.set_xticklabels(['20a', '20b', '20c'])
	# # ax.set_xticks([1.5, 4.5, 7.5])

	# hB, = plot([1,1],'b-')
	# hR, = plot([1,1],'r-')
	# legend((hB, hR),('Estandar', 'Adaptivo'))
	# hB.set_visible(False)
	# hR.set_visible(False)

	# savefig(os.path.join('boxplot_images', instance_group+'.png'))
	# show()


# fig = plt.figure()
# ax = fig.add_subplot(111)
# boxplot = ax.boxplot(all_data)
# fig.savefig('boxplot_images/'+folder_static+'.png', bbox_inches='tight')

