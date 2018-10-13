import os
import matplotlib.pyplot as plt
import matplotlib as mpl

from pylab import plot, show, savefig, xlim, figure, ylim, legend, boxplot, setp, axes


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




#folder_static = input("Ingrese el nombre de la carpeta estatica\n")
#folder_static = "boxplot_results/"+folder_static
folder_static = 'boxplot_results/wen20'
listdir_static = os.listdir(folder_static)
listdir_static.sort()

#folder_adaptive = input("Ingrese el nombre de la carpeta adaptiva\n")
#folder_adaptive = "boxplot_results/"+folder_adaptive
folder_adaptive = 'boxplot_results/wen20_adaptive'
listdir_adaptive = os.listdir(folder_adaptive)
listdir_adaptive.sort()

all_static_data = []
all_adaptive_data = []



for filename in listdir_static:
	fullpath = os.path.join(folder_static, filename)
	with open(fullpath) as file:
		content = file.readlines()
		data = [float(read_data.split("-")[8]) for read_data in content]
		all_static_data.append(data)

for filename in listdir_adaptive:
	fullpath = os.path.join(folder_adaptive, filename)
	with open(fullpath) as file:
		content = file.readlines()
		data = [float(read_data.split("-")[8]) for read_data in content]
		all_adaptive_data.append(data)
		
instance1 = [all_static_data[0],all_adaptive_data[0]]
instance2 = [all_static_data[1],all_adaptive_data[1]]
instance3 = [all_static_data[2],all_adaptive_data[2]]
instance4 = [all_static_data[3],all_adaptive_data[3]]
instance5 = [all_static_data[4],all_adaptive_data[4]]

all_data = [instance1,instance2,instance3,instance4,instance5]

fig = figure()
ax = axes()

# first boxplot pair
bp = boxplot(instance1, positions = [1, 2], widths = 0.6)
setBoxColors(bp)

# second boxplot pair
bp = boxplot(instance2, positions = [4, 5], widths = 0.6)
setBoxColors(bp)

# thrid boxplot pair
bp = boxplot(instance3, positions = [7, 8], widths = 0.6)
setBoxColors(bp)

xlim(0,9)
ylim(2500,4000)

ax.set_xticklabels(['20a', '20b', '20c'])
ax.set_xticks([1.5, 4.5, 7.5])

hB, = plot([1,1],'b-')
hR, = plot([1,1],'r-')
legend((hB, hR),('Estatico', 'Adaptivo'))
hB.set_visible(False)
hR.set_visible(False)

savefig(os.path.join('boxplot_images', '20.png'))
show()


# fig = plt.figure()
# ax = fig.add_subplot(111)
# boxplot = ax.boxplot(all_data)
# fig.savefig('boxplot_images/'+folder_static+'.png', bbox_inches='tight')

