
import numpy as np
import matplotlib.pyplot as plt
data = np.loadtxt('places.txt',delimiter = ",")
#print(data[:,0])
plt.scatter(data[:,0], data[:,1])
plt.show()