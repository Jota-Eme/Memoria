import plotly.plotly as py
import plotly.tools as tls

import matplotlib.pyplot as plt
import numpy as np

# Generating sample data
N = 1000
data_set1 = np.random.normal(1, 1, N)
data_set2 = np.random.lognormal(1, 1, N)
data_set3 = np.random.exponential(1, N)
data_set4 = np.random.gumbel(6, 4, N)
data_set5 = np.random.triangular(2, 9, 11, N)

data = [data_set1, data_set2, data_set3, data_set4, data_set5]

mpl_fig = plt.figure()
ax = mpl_fig.add_subplot(111)

ax.boxplot(data)

plotly_fig = tls.mpl_to_plotly( mpl_fig )
py.iplot(plotly_fig, filename='mpl-multiple-boxplot')