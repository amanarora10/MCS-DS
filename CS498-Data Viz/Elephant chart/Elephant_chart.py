
# coding: utf-8
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

df = pd.read_csv(r"C:\Users\amana\source\repos\amanarora10\MCS-DS\CS 498 - Data Viz\LMWPIDweb.csv")

f = df[df["mysample"] ==1]
columns = ['Quntile Index (i)','Quntile Bin', 'Quintiles Average RRinc']
RRinc = list()
quantile_bin = list()

#1988 data
df_1988 = df[df["bin_year"] == 1988]
df_1988 = df_1988.sort_values(['RRinc'])

#Create running total of population 
df_1988["runningpop"] =  df_1988["pop"].cumsum()

df_1988.to_csv(r"C:\Users\amana\source\repos\amanarora10\MCS-DS\CS 498 - Data Viz\Stage1.csv")

#Add quantiles
df_1988["quantile"] = 20.0*df_1988["runningpop"]/df_1988["pop"].sum()
df_1988["quantile"]= df_1988["quantile"].apply(lambda x: min(int(x),19))

#Aggreagate quantiles
for quantile in range (0,20):
    agg = df_1988[df_1988["quantile"] == quantile]
    weighted_product = agg["pop"]*agg["RRinc"]
    RRinc.append(round(weighted_product.sum()/agg["pop"].sum(),2))
    bin =  str(quantile*5) + "% to "+str((quantile+1)*5)+ "%"
    quantile_bin.append(bin)

df1 = pd.DataFrame({"Quntile Bin":quantile_bin,
                  "Quintiles Average RRinc:": RRinc})

df1.to_csv(r"C:\Users\amana\source\repos\amanarora10\MCS-DS\CS 498 - Data Viz\Stage2.csv")                   
                   
            


#2008 data
df = df[df["mysample"] ==1]
columns = ['Quntile Index (i)','Quntile Bin', 'Quintiles Average RRinc']
RRinc = list()
quantile_bin = list()

#2008 data
df_2008 = df[df["bin_year"] == 2008]
df_2008 = df_2008.sort_values(['RRinc'])

#Create running total of population 
df_2008["runningpop"] =  df_2008["pop"].cumsum()

#Add quantiles
df_2008["quantile"] = 20.0*df_2008["runningpop"]/df_2008["pop"].sum()
df_2008["quantile"]= df_2008["quantile"].apply(lambda x: min(int(x),19))

#Aggreagate quantiles
for quantile in range (0,20):
    agg = df_2008[df_2008["quantile"] == quantile]
    weighted_product = agg["pop"]*agg["RRinc"]
    RRinc.append(round(weighted_product.sum()/agg["pop"].sum(),2))
    bin =  str(quantile*5) + "% to "+str((quantile+1)*5)+ "%"
    quantile_bin.append(bin)
    
df1["Quintiles Average RRinc 2008"] =  RRinc
df1["Quntile growth"] = 100*(df1["Quintiles Average RRinc 2008"] -  df1["Quintiles Average RRinc:"])/df1["Quintiles Average RRinc:"]
df1["Quntile growth"] = df1["Quntile growth"].apply(round)
df1.to_csv(r"C:\Users\amana\source\repos\amanarora10\MCS-DS\CS 498 - Data Viz\Stage3.csv")   

plt.plot([i for i in range(0,20)], df1["Quntile growth"])
plt.ylabel('Growth')
plt.show()



