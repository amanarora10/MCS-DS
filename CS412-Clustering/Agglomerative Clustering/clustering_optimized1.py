import sys
import time
#for line in sys.stdin:
    #print(line)

def read_data(result):
    file = open("data.txt","r")
    first_line  = True
    loc_data = [None]*2500
    i = 0
    for line in file:
      fields = line.split(" ")
      if first_line==True:                                               
        N, K, M  = [int(field) for field in fields]
        result = [None]*N
        first_line = False
      else:
          loc = [{'x': float(fields[0]) ,'y': float(fields[1]), 'index': i, 'cluster_id':i }]
          result[i] = i
          loc_data[i] = (loc)
          i = i+1

    return N,K,M,loc_data,result

def read_data_stdin(result):
    first_line  = True
    loc_data = [None]*2500
    i = 0
    for line in sys.stdin:
      fields = line.split(" ")
      if first_line==True:                                               
        N, K, M  = [int(field) for field in fields]
        result = [None]*N
        first_line = False
      else:
          loc = [{'x': float(fields[0]) ,'y': float(fields[1]), 'index': i, 'cluster_id':i }]
          result[i] = i
          loc_data[i] = (loc)
          i = i+1
    return N,K,M,loc_data, result

def merge_cluster(loc_data,i,j,result):
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

def check_clusters(loc_data, Measure, total_clusters,result):
    best = {'cluster1':0,'cluster2':0, 'dist': sys.float_info.max}
    for i in range(0, total_clusters -1 ):
        for j in range (i+1, total_clusters):
            current = compare_cluster(loc_data,i,j, Measure)
            if(current['dist'] < best['dist']):
               best = current
    merge_cluster(loc_data,best['cluster1'],best['cluster2'],result)
    return total_clusters -1, result


def check_clusters_min(loc_data, Measure, total_clusters,K,result):
    dist_meas = []

    #Calculate distance measure
    for i in range(0, total_clusters -1 ):
        for j in range (i+1, total_clusters):
            current = compare_cluster(loc_data,i,j, Measure)
            dist_meas.append(current)
    dist_meas.sort(key=lambda x: x['dist'])
    i = 0
    for i in range(0,len(dist_meas1)-1):
        if(total_clusters == K):
            break
        replace_cand = dist_meas[i]['cluster2']
        new_cluster_id = dist_meas[i]['cluster1']
        #update distance matrix
        for j in range(i+1,len(dist_meas)):
               if dist_meas[j]['cluster1'] ==replace_cand:
                  dist_meas[j]['cluster1'] = new_cluster_id
               elif dist_meas[j]['cluster2'] ==replace_cand:
                  dist_meas[j]['cluster2'] = new_cluster_id
        #update result
        result = [new_cluster_id if x==replace_cand else x for x in result]
        i = i+1
        total_clusters = len(set(result))
    return result


def run_clustering():
    result = []
    N,K,M,loc_data,result = read_data(result)
    cluster_count = N
    if(M ==1 or M==2):
        while cluster_count > K:
            cluster_count, result = check_clusters(loc_data, M, cluster_count,result )
    elif(M==0):
            result = check_clusters_min(loc_data, M, cluster_count,K,result )
    return loc_data, M, result
        

loc_data, M, result = run_clustering()

for line in result:
    if line is None:
        break
    temp = line
    if(M==0):
        while(temp != result[temp]):
            temp = result[temp]
    sys.stdout.write( str(temp) + "\n")
