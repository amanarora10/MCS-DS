Y = list()

def read_file(file_name): 
    file = open(file_name,"r")
    i = 0
    for line in file:
      fields = line.split(" ")
      index = int(fields[1])
      data[i] = index
      i = i+ 1
      data_count[index] = data_count[index] +1
    return


read_file(r"C:\Users\amana\Source\Repos\amanarora10\MCS-DS\CS 412\Decision Tree\training.txt")
