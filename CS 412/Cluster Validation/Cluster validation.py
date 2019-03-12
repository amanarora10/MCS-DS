import numpy

def read_file(file_name, data_count, data): 
    file = open(file_name,"r")
    i = 0
    for line in file:
      fields = line.split(" ")
      index = int(fields[1])
      data[i] = index
      i = i+ 1
      data_count[index] = data_count[index] +1
    return

def read_data():
    truth = numpy.zeros(300,dtype = int)
    cluster= numpy.zeros([5,300],dtype = int)
    cluster_count =  numpy.zeros(shape=(5,5),dtype = int)
    truth_count = numpy.zeros(5,dtype = int)

    #Read ground truth
    read_file('partitions.txt',truth_count, truth)

    #Read cluster data 
    for i in range(0,5):
        read_file('clustering_'+str(i+1) +'.txt',cluster_count[i], cluster[i])
    return truth,truth_count, clusters, cluster_count

truth, truth_count,  cluster_count = read_data()