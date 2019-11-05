import sys
from collections import Counter

P_c = list()
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

def ComputeLikeyhood(X_train,Y_train,test_row,c):
    attribute_count = [0]*len(X_train[0])
    likley_hood = 1
    label_match_index = [i for i, y in enumerate(Y_train) if (y == c)] 
    j = 0
    for attribute in test_row:
        for index in label_match_index:
            if(X_train[index][j] == test_row[j]):
                attribute_count[j] = attribute_count[j]  +1
        attribute_count[j] = (attribute_count[j] + 0.1)/(len(label_match_index)+ 0.1*len(X_train[0]))                   
        likley_hood = likley_hood*attribute_count[j] 
        j = j+1
    return likley_hood

def read_data():
    file = open(r"C:\Users\amana\source\repos\amanarora10\MCS-DS\CS 412\Naive Bayes Classifier\test.data","r")
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


P_c =  Counter(y_train)
for element in P_c.keys():
     P_c[element] = (P_c[element] + 0.1)/(len(y_train)+ 0.1*len(set(y_train)))

result = [None] * len(X_test)
i = 0
for test_row in X_test: 
    max = -1
    for c in set(y_train):
        P_x_c = ComputeLikeyhood(X_train,y_train,test_row,c)
        current = P_c[c]*P_x_c
        print("class ",c,":",current)
        if(max<current):
           result[i] = c  
           max = current
    i = i +1
#text_file = open(r'C:\Users\amana\Source\Repos\amanarora10\MCS-DS\CS 412\Naive Bayes Classifier\result.txt', "w")
#for label in result:
#    print(label,file=text_file)

for label in result:
    if label is None:
        break
    sys.stdout.write( str(label) + "\n")