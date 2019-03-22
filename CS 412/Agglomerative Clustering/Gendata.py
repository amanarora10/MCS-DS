import numpy as np
N = 250
K =10
M =0
text_file = open('data.txt', "w")
data = 90*np.random.randn(N,2)
output = str(N) + ' ' + str(K) + ' '+ str(M)
print(output, file=text_file, sep='')

for record in  data:
    output = str(record[0]) + ' '+ str(record[1]) 
    print(output, file=text_file, sep='')

text_file.close()
