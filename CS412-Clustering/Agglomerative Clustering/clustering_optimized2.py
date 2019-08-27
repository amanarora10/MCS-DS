import sys
import time
#for line in sys.stdin:
    #print(line)
result = [None]*2600


def eucildian_distance(point1,point2):
    dist = (point1[0]['x'] - point2[0]['x'])**2 + (point1[0]['y'] - point2[0]['y'])**2
    return dist

def compute_pairwise(loc_data,distance_matrix):
    for i in range (0,len(loc_data)):
        temp = []
        for j in range (i, len(loc_data)):
            dist = eucildian_distance(loc_data[i], loc_data[j])
            distance_matrix[i][j] = dist
            distance_matrix[j][i] = dist

def read_data():
    file = open("data.txt","r")
    first_line  = True
    i = 0
    for line in file:
      fields = line.split(" ")
      if first_line==True:                                               
        N, K, M  = [int(field) for field in fields]
        loc_data = [None]*N
        distance_matrix = [[0 for row in range(0,N)] for col in range(0,N)]
        first_line = False
      else:
          loc = [{'x': float(fields[0]) ,'y': float(fields[1]), 'index': i, 'cluster_id':i, 'd_min': sys.float_info.max}]
          result[i] = i
          loc_data[i] = (loc)
          i = i+1
    compute_pairwise(loc_data,distance_matrix)
    return N,K,M,loc_data, distance_matrix

def read_data_stdin():
    first_line  = True
    i = 0
    for line in sys.stdin:
      fields = line.split(" ")
      if first_line==True:                                               
        N, K, M  = [int(field) for field in fields]
        loc_data = [None]*N
        distance_matrix = [[0 for row in range(0,N)] for col in range(0,N)]
        first_line = False
      else:
          loc = [{'x': float(fields[0]) ,'y': float(fields[1]), 'index': i, 'cluster_id':i }]
          result[i] = i
          loc_data[i] = (loc)
          i = i+1
    compute_pairwise(loc_data,distance_matrix)
    return N,K,M,loc_data, distance_matrix
    

def merge_cluster(loc_data,i,j):
    for loc in loc_data[j]:
        result[loc['index']] = loc_data[i][0]['cluster_id']
    loc_data[i] = loc_data[i] + loc_data[j]
    del loc_data[j]
    return

def compare_cluster(loc_data, i,j, Measure,distance_matrix):

    cluster1 = loc_data[i]
    cluster2 = loc_data[j]
    best = {'cluster1':i,'cluster2':j, 'dist': 0}
    if(Measure ==0):
        best['dist'] = sys.float_info.max
        for point1 in cluster1:
            for point2 in cluster2:
                #current = eucildian_distance(point1, point2)
                current = distance_matrix[point1['index']][point2['index']]
                if(current < best['dist']):
                    best = {'cluster1':i,'cluster2':j, 'dist': current}
    elif(Measure ==1):
        for point1 in cluster1:
            for point2 in cluster2:
                #current = eucildian_distance(point1, point2)
                current = distance_matrix[point1['index']][point2['index']]
                if(current > best['dist']):
                    best = {'cluster1':i,'cluster2':j, 'dist': current}
    elif(Measure ==2):
        count = 0
        agg_dist = 0
        for point1 in cluster1:
            for point2 in cluster2:
                agg_dist = agg_dist + distance_matrix[point1['index']][point2['index']]
                count = count +1
       
        best = {'cluster1':i,'cluster2':j, 'dist': agg_dist/count}

    return best

def check_clusters(loc_data, Measure, total_clusters,distance_matrix,first_time):

    if(first_time):
        for i in range(0,total_clusters-1):
            best = {'cluster1':0,'cluster2':0, 'dist': sys.float_info.max}
            for j in range (i+1, total_clusters):
                current = compare_cluster(loc_data,i,j, Measure,distance_matrix)
                if(current['dist'] < best['dist']):
                   best = current  
               # print("i and j", i, j)
            if (loc_data[best['cluster1']][0]['d_min'] >  best['dist']):
                loc_data[best['cluster1']][0]['d_min'] =  best['dist']
            if (loc_data[best['cluster2']][0]['d_min'] >  best['dist']):
                loc_data[best['cluster2']][0]['d_min'] =  best['dist']
            first_time = False
    #Find min
    loc_data.sort(key=lambda x: x[0]['d_min'])
    
    for cluster in loc_data:
        for point in cluster:


    merge_cluster(loc_data,best['cluster1'],best['cluster2'])
    return total_clusters -1

def run_clustering():
    N,K,M,loc_data, distance_matrix = read_data()
    cluster_count = N
    first_time = True
    while cluster_count > K:
        cluster_count = check_clusters(loc_data, M, cluster_count,distance_matrix,first_time)
        first_time = False
    return loc_data, N

loc_data, N = run_clustering()

for line in result:
    if line is None:
        break
    sys.stdout.write( str(line) + "\n")

