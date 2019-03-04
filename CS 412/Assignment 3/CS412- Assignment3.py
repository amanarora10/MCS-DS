import numpy as np
import matplotlib.pyplot as plt
import random
import sys
import pandas as pd
import pandas as pd
def initalize_centroids(data, k):
    centroids = []
    while k>0:
        n = random.randint(0,len(data))
        centroid1 = data[n]
        centroids.append(centroid1)
        k = k-1
    return centroids

def assign_centroids(new_centroids, data, k):
    data['dist0'] = math.hypot(data['x'] - new_centroids[0] , data['y']- new_centroid[0])
    data['dist1'] = math.hypot(data['x'] - new_centroids[1] , data['y']- new_centroid[1])
    data['dist2'] = math.hypot(data['x'] - new_centroids[2] , data['y']- new_centroid[2])
   
#def compute_new_centroids(data):
    


def run_k_means(new_centroids,data,k):
    centroids = []
    while (set(centroids) != set(new_centroids)):
       assign_centroids(new_centroids,data,k)
       centroids = newinit_centroids
       new_centroids = compute_new_centroids(data)
    return new_centroids 
                            
data= pd.read_csv(r'CS 412\Assignment 3\places.txt', sep=",", names = ["x", "y","dist0","dist1","dist2","cluster"])

print(data.head())

#print(data[:,0])
#plt.scatter(data[:,0], data[:,1])
#plt.show()
k=3
init_centroids = initalize_centroids(data,k )
results = run_k_means(init_centroids, data, k) 


