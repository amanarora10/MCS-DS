import numpy as np
import matplotlib.pyplot as plt
import random
import math
import pandas as pd


def initalize_centroids(data, k):
    centroids = []
    while k>0:
        n = random.randint(0,len(data))
        centroid1 = [data.iloc[n].x, data.iloc[n].y]
        centroids.append(centroid1)
        k = k-1
    return centroids

def assign_centroids(new_centroids, data, k):
    data['dist0'] = ((data['x'] - new_centroids[0][0])**2 + (data['y']- new_centroids[0][1])**2)**0.5
    data['dist1'] = ((data['x'] - new_centroids[1][0])**2 + (data['y']- new_centroids[1][1])**2)**0.5
    data['dist2'] = ((data['x'] - new_centroids[2][0])**2 + (data['y']- new_centroids[2][1])**2)**0.5

def compute_new_centroids(data,k):
    new_centroids= []
    no_change = False
    last_cluster = data['cluster']
    data['cluster'] = 0
    data.loc[((data['dist1']<data['dist0']) &  (data['dist1']<data['dist2'])),'cluster'] = 1 
    data.loc[((data['dist2']<data['dist0']) & (data['dist2']<data['dist1'])),'cluster'] = 2
    if(last_cluster.equals(data['cluster'])):
       no_change = True
    while k>0:
        data_centroid_x = data.loc[data['cluster']==k-1,'x']
        data_centroid_y = data.loc[data['cluster']==k-1,'y']
        new_centroids.append([data_centroid_x.mean(), data_centroid_y.mean()])
        k = k-1
    return new_centroids, no_change


def run_k_means(new_centroids,data,k):
    centroids = []
    j = 0 
    no_change = False
    while (no_change == False):
       j = j+1
       print("Run:",j," Centroids:" , new_centroids,centroids)
       assign_centroids(new_centroids,data,k)
       centroids = new_centroids
       new_centroids, no_change = compute_new_centroids(data,k)
    return new_centroids 
                            
data= pd.read_csv(r'places.txt', sep=",", names = ["x", "y","dist0","dist1","dist2","cluster"])

print(data.head())

#print(data[:,0])
#plt.scatter(data[:,0], data[:,1])
#plt.show()
k=3
init_centroids = initalize_centroids(data,k)
results = run_k_means(init_centroids, data, k) 
pd.options.display.max_rows = 400
text_file = open(r'clusters.txt', "w")
print(data['cluster'], file=text_file, end="",sep='')
text_file.close()        





