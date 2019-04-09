from sklearn import tree
from sklearn.neural_network import MLPClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score
def read_file(file_name, test): 
    Y = list()
    X = list()
    start  = 1
    if(test == True):
        start = 0
    file = open(file_name,"r")
    i = 0
    for line in file:
      fields = line.split(" ")
      if(test == False):
        Y.append(int(fields[0]))
      row = list()
      for i in range(start,len(fields)):
          entry = fields[i]
          data = entry[entry.find(":")+1:]
          row.append(int(data))
      X.append(row)
    return X,Y 

X, Y = read_file(r"C:\Users\amana\Source\Repos\amanarora10\MCS-DS\CS 412\Decision Tree\training.txt", False)
#X_train, X_val, Y_train, Y_val = train_test_split(X, Y, random_state=1)
X_test, Y_test= read_file(r"C:\Users\amana\Source\Repos\amanarora10\MCS-DS\CS 412\Decision Tree\testing.txt", True)

model = tree.DecisionTreeClassifier(max_depth = 841)
model = model.fit(X,Y)
Y_test = model.predict(X_test)
text_file = open(r'C:\Users\amana\Source\Repos\amanarora10\MCS-DS\CS 412\Decision Tree\result.txt', "w")
for label in Y_test:
    print(label,file=text_file)
#maximum = 0.0
#for i in range(1,1001):
#    model = tree.DecisionTreeClassifier(max_depth = i)
#    #model = MLPClassifier(solver = 'lbfgs')
#    model = model.fit(X_train,Y_train)
#    y_predict = model.predict(X_val)
#    current = accuracy_score(Y_val, y_predict)
#    if(maximum < current):
#       maximum = current
#       depth = i
#    print("Max Depth:", i,"Accuracy:",current)
#print ("Max:", maximum,"best depth:", depth)