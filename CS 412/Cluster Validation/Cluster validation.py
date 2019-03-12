import numpy

def read_file(file_name, data): 
    file = open(file_name,"r")
    for line in file:
      fields = line.split(" ")
      index = int(fields[1])
      data[index] = data[index] +1
    return

def read_data():
    clusters =  numpy.zeros(shape=(5,5))
    truth = numpy.zeros(5)
    #Read ground truth
    read_file('partitions.txt',truth)

    #Read cluster data 
    for i in range(0,5):
        read_file('clustering_'+str(i+1) +'.txt',clusters[i])
    return truth, clusters

truth, clusters = read_data()