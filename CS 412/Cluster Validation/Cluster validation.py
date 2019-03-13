import numpy
import math
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
    clusters= numpy.zeros([5,300],dtype = int)
    cluster_count =  numpy.zeros(shape=(5,5),dtype = int)
    truth_count = numpy.zeros(5,dtype = int)

    #Read ground truth
    read_file('partitions.txt',truth_count, truth)

    #Read cluster data 
    for i in range(0,5):
        read_file('clustering_'+str(i+1) +'.txt',cluster_count[i], clusters[i])
    return truth,truth_count, clusters, cluster_count

def intersection(cluster, truth, cluster_id, truth_id):
    nij = 0
    i = 0
    for entry in cluster:
        if (entry == cluster_id and truth[i] == truth_id):
           nij = nij + 1
        i = i + 1
    n =  len(cluster)
    return (nij/n)


def calculate_NMI(truth,truth_count, cluster_list, cluster_count ):
    i = 0
    # Calcuate Ict
    Ict = 0
    Hc = 0
    Ht = 0
    first_time = True
    for cluster in cluster_count:
        if(cluster == 0):
            break
        j = 0
        Pci = cluster_count[i]/numpy.sum(cluster_count)                                                                                                                                                                                                                                                     
        Hc  = Hc + Pci*math.log(Pci,2.0)
        for partition in truth_count:
            Pij = intersection(cluster_list, truth, i,j)
            Ptj = truth_count[j]/numpy.sum(truth_count)
            if(first_time == True and Ptj != 0 ):
               Ht = Ht + Ptj*math.log(Ptj,2.0)                 
            if(Pij !=0):
                Ict = Ict + Pij* math.log(Pij/(Pci*Ptj), 2.0)
                print('iteration:', i,j,'Pij:',Pij,Pij* math.log(Pij/(Pci*Ptj), 2.0) )
            j = j+1
        first_time = False   
        i = i+1
    NMI = Ict/(math.sqrt(Ht*Hc))
    return NMI
def calculate_Jaccard(truth,truth_count, cluster_list, cluster_count):

    return 0

result = []
truth, truth_count, clusters, cluster_count = read_data()
record = {'NMI':0, 'Jaccard':0}
i = 0
for cluster_list in clusters:
    record['NMI'] = calculate_NMI(truth, truth_count, cluster_list, cluster_count[i] )
    record['Jaccard'] = calculate_Jaccard(truth,truth_count, cluster_list, cluster_count)
    result.append(record) 
    i = i +1

print(result)