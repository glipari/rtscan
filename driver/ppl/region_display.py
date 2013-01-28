import matplotlib.pyplot as plt
import numpy as np


plt.title('The feasibility region for C11 and C21')
plt.ylabel('C21')
plt.xlabel('C11')

#x = np.arange(0, 8000, 1000)
#y = np.arange(0, 2000, 500)
#X, Y = np.meshgrid(x, y)
plt.axis([0, 8000, 0, 1500])

#plt.plot([0, 3850, 7000], [1050, 1050, 0], 'r--', label='rtscan')

# Now, to display the offset_based region
xs = []; ys = [];
xx = []
for i in range(7200+1) :
	xx.append(0)
for l in open('ob_region.txt').readlines() :
	x, y = [int(s) for s in l.split()]
	if xx[x] < y : 
		xx[x]= y
for i in range(7200+1) :
	xs.append(i)
	ys.append(xx[i])
plt.plot(xs, ys, linewidth=5) #, alpha=0.5)

# to display the holistic region
xs[:] = []; ys[:] = [];
xx[:] = [];
for i in range(7200+1) :
	xx.append(0)
for l in open('h_region.txt').readlines() :
	x, y = [int(s) for s in l.split()]
	if xx[x] < y : xx[x]=y
for i in range(7200+1) :
	xs.append(i)
	ys.append(xx[i])
plt.plot(xs, ys, 'c--', linewidth=2.5);

# to display the offset_based_optimized region
#xs[:] = []; ys[:] = [];
#xx[:] = [];
#for i in range(7200+1) :
#	xx.append(0)
#for l in open('obp_region.txt').readlines() :
#	x, y = [int(s) for s in l.split()]
#	if xx[x] < y : xx[x]=y
#for i in range(7200+1) :
#	xs.append(i)
#	ys.append(xx[i])
#plt.plot(xs, ys);

# to display the rtscan region
plt.plot([0, 3850, 7000], [1050, 1050, 0], 'r--', linewidth=5)

plt.legend(('offset_based', 'holistic', 'rtscan',))

plt.show()
plt.savefig('region.eps')
