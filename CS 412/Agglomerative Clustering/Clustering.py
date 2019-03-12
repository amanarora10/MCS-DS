import sys
import time
#for line in sys.stdin:
    #print(line)
result = [None]*2500
def read_data():
    file = open("data.txt","r")
    first_line  = True
    loc_data = [None]*2500
    i = 0
    for line in file:
      fields = line.split(" ")
      if first_line==True:                                               
        N, K, M  = [int(field) for field in fields]
        first_line = False
      else:
          loc = [{'x': float(fields[0]) ,'y': float(fields[1]), 'index': i, 'cluster_id':i }]
          result[i] = i
          loc_data[i] = (loc)
          i = i+1

    return N,K,M,loc_data

def read_data_stdin():
    first_line  = True
    loc_data = [None]*2500
    i = 0
    for line in sys.stdin:
      fields = line.split(" ")
      if first_line==True:                                               
        N, K, M  = [int(field) for field in fields]
        first_line = False
      else:
          loc = [{'x': float(fields[0]) ,'y': float(fields[1]), 'index': i, 'cluster_id':i }]
          result[i] = i
          loc_data[i] = (loc)
          i = i+1
    return N,K,M,loc_data

def merge_cluster(loc_data,i,j):
    for loc in loc_data[j]:
        result[loc['index']] = loc_data[i][0]['cluster_id']
    loc_data[i] = loc_data[i] + loc_data[j]
    del loc_data[j]
    return

def eucildian_distance(point1,point2):
    dist = (point1['x'] - point2['x'])**2 + (point1['y'] - point2['y'])**2
    return dist

def compare_cluster(loc_data, i,j, Measure):

    cluster1 = loc_data[i]
    cluster2 = loc_data[j]
    best = {'cluster1':i,'cluster2':j, 'dist': 0}
    if(Measure ==0):
        best['dist'] = sys.float_info.max
        for point1 in cluster1:
            for point2 in cluster2:
                current = eucildian_distance(point1, point2)
                if(current < best['dist']):
                    best = {'cluster1':i,'cluster2':j, 'dist': current}
    elif(Measure ==1):
        for point1 in cluster1:
            for point2 in cluster2:
                current = eucildian_distance(point1, point2)
                if(current > best['dist']):
                    best = {'cluster1':i,'cluster2':j, 'dist': current}
    elif(Measure ==2):
        count = 0
        agg_dist = 0
        for point1 in cluster1:
            for point2 in cluster2:
                agg_dist = agg_dist + eucildian_distance(point1, point2)
                count = count +1
       
        best = {'cluster1':i,'cluster2':j, 'dist': agg_dist/count}

    return best

def check_clusters(loc_data, Measure, total_clusters):
    start = time.perf_counter()
    best = {'cluster1':0,'cluster2':0, 'dist': sys.float_info.max}
    for i in range(0, total_clusters -1 ):
        for j in range (i+1, total_clusters):
            current = compare_cluster(loc_data,i,j, Measure)
            if(current['dist'] < best['dist']):
               best = current
    merge_cluster(loc_data,best['cluster1'],best['cluster2'])
    end = time.perf_counter()
    print("check_clusters()", end - start)
    return total_clusters -1

def run_clustering():
    N,K,M,loc_data = read_data()
    cluster_count = N
    while cluster_count > K:
        cluster_count = check_clusters(loc_data, M, cluster_count )
    return loc_data, N

loc_data, N = run_clustering()

for line in result:
    if line is None:
        break
    start = time.perf_counter()
    sys.stdout.write( str(line) + "\n")
    end = time.perf_counter()
    print("final print()", end - start)
