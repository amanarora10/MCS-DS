import sys
def read_data_stdin():
    X_train = list()
    y_train = list()
    X_test= list()
    first_line = True
    for line in sys.stdin:
      if(first_line == True):
        first_line = False
      else:
        row = list()
        row = line.split(",")
        if(int(row[17])!=-1):
          X_train.append(list(map(int,row[1:17])))    
          y_train.append(int(row[17]))
        else:
          X_test.append(list(map(int,row[1:17])))
    return X_train, y_train, X_test

def read_data():
    file = open(r"C:\Users\amana\source\repos\amanarora10\MCS-DS\CS 412\Naive Bayes Classifier\zoo.data","r")
    X_train = list()
    y_train = list()
    X_test= list()
    for line in file:
      row = list()
      row = line.split(",")
      if(int(row[17])!=-1):
        X_train.append(list(map(int,row[1:17])))    
        y_train.append(int(row[17]))
      else:
        X_test.append(list(map(int,row[1:17])))

    return X_train, y_train, X_test

X_train, y_train, X_test = read_data()
