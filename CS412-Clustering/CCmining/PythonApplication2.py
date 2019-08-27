from collections import defaultdict
import csv, re
from itertools import product

class GSP(object):
    def __init__(self, minSupp, minConf):
        """ Parameters setting
        """
        self.minSupp = minSupp  # min support (used for mining frequent sets)
        self.minConf = minConf  # min confidence (used for mining association rules)

    def fit(self, filePath):
        """ Run the GSP algorithm, return the frequent *-term sets. 
        """
        # Initialize some variables to hold the tmp result
        transListSet, transListDict  = self.getTransListSet(filePath)   # get transactions (list that contain sets)
        itemSet       = self.getOneItemSet(transListSet) # get 1-item set
        itemCountDict = defaultdict(int)         # key=candiate k-item(k=1/2/...), value=count
        freqSet       = dict()                   # a dict store all frequent *-items set
        
        self.transLength = len(transListSet)     # number of transactions
        self.itemSet     = itemSet
        
        # Get the frequent 1-term set
        freqOneTermSet = self.getItemsWithMinSupp(transListSet, itemSet, 
                                             itemCountDict, self.minSupp, transListDict)
        #freqSet[1] = freqOneTermSet # Remove when handling len n seq in for loop below
        #Main loop
        currFreqTermSet = freqOneTermSet
        #while currFreqTermSet != set():
        k=1
        freqSet[k] = currFreqTermSet  # save the result
        k = k+1
        cand_two_itemsets = list()
        freq1_list = [list(x) for x in currFreqTermSet]
        freq1_list = [y for x in currFreqTermSet for y in x]
        for record in transListSet:
            for freq1_word in freq1_list:
                word_index = record.index(freq1_word)
                if word_index + 1 < len(record):
                    next_word = record[word_index + 1]
                    if next_word in freq1_list:
                        phrase = freq1_word + ' ' + next_word
                    if not phrase in cand_two_itemsets:
                            cand_two_itemsets.append(phrase)
        for phrase in cand_two_itemsets:
            cc_reg =  re.compile(r'\b%s\b' %phrase, re.I)
            for record in transListSet:
                record_str = ' '.join(record)
                match = cc_reg.search(record_str)
                if (match):
                    phrase_set = frozenset({phrase})
                    freqSet[phrase_set] = freqSet[phrase_set] +1
                    itemSet_[phrase_set]  = freqSet[phrase_set]
        # Only conserve frequent item-set 
        n = len(transListSet)
        for item, cnt in localSet_.items():
            itemSet_.add(item) if float(cnt)/n >= minSupp else None

        self.itemCountDict = itemCountDict # 
        self.freqSet       = freqSet       # Only frequent items(a dict: freqSet[1] indicate frequent 1-term set )
        return itemCountDict, freqSet
            
            
    def getSpecRules(self, rhs):
        """ Specify a right item, construct rules for it
        """
        if rhs not in self.itemSet:
            print('Please input a term contain in the term-set !')
            return None
        
        rules = dict()
        for key, value in self.freqSet.items():
            for item in value:
                if rhs.issubset(item) and len(item) > 1:
                    item_supp = self.getSupport(item)
                    item = item.difference(rhs)
                    conf = item_supp / self.getSupport(item)
                    if conf >= self.minConf:
                        rules[item] = conf
        return rules
        
    
    def getSupport(self, item):
        """ Get the support of item """
        return self.itemCountDict[item] / self.transLength
     
  
        
    def getJoinedItemSet(self, termSet, freqOneTermSet):
        """ Generate new k-terms candiate itemset"""
        print("Start Generating 2 item candidates")
#        if(termSet!= None):
#            print('Generating ', len(termSet[0]), '-len itemsets',sep='')
        candidates = set()
        for term in termSet:
            candidates1 =  set([(term,x) for x in freqOneTermSet])
            candidates2 =  set([(x,term) for x in freqOneTermSet])
            new_candidates =  candidates2.union(candidates1)      
            candidates = candidates.union(new_candidates)
        print("End Generating 2 item candidates")
        return candidates
        
    def getOneItemSet(self, transListSet):
        """ Get unique 1-item set in `set` format 
        """
        itemSet = set()
        for line in transListSet:
            for item in line:
                itemSet.add(frozenset([item]))
        return itemSet
        
    
    def getTransListSet(self, filePath):
        """ Get transactions in list format 
        """
        transListSet = []
        transListDict = []
        with open(filePath, 'r') as file:
            reader = csv.reader(file, delimiter=' ')
            for line in reader:
                transListSet.append(list(line))
                temp_dict = defaultdict(int)    
                for item in list(line):
                    temp_dict[item] +=1 ;
                transListDict.append(temp_dict) 
        return transListSet, transListDict

    def getItemsWithMinSuppPattern(self, transListSet, itemSet, freqSet,minSupp, transListDict):
        """ Get frequent item set using min support
        """
        print("Start Generating 2-len frequent items ")
        itemSet_  = set()
        localSet_ = defaultdict(int)
        count = 0
        for item in itemSet:
            if(not count%1000):
              print(count)
              cand =  [list(x) for x in item ]
              cand =  [y for x in cand for y in x]
              for dict, record  in zip(transListDict,  transListSet):
                  skip = False
                  for element in cand:
                      if(element not in dict):
                          skip = True
                  if skip == False:
                     for k in range(len(record) - len(cand)+1 ):
                        slice = record[k:k+len(cand)]     
                        if(cand==slice):
                            freqSet[item] = freqSet[item] +1
                            count = count +1;
                            localSet_[item]  = localSet_[item] +1 
                            found = True
                            break
     

        
        # Only conserve frequent item-set 
        n = len(transListSet)
        for item, cnt in localSet_.items():
            itemSet_.add(item) if float(cnt)/n >= minSupp else None
        print("End Generating 2-len  frequent items ")
        return itemSet_             
    
    def getItemsWithMinSupp(self, transListSet, itemSet, freqSet, minSupp, transListDict):
        """ Get frequent item set using min support
        """
        itemSet_  = set()
        localSet_ = defaultdict(int)
        count = 0
        print('Mining frequent 1 items')
        for item in itemSet:
            item_str =  set(item).pop()
            if(count%1000==0):
                print(count)
            freqSet[item]   += sum([1 for dict in transListDict if item_str in dict])
            count = count +1;
            localSet_[item]   += sum([1 for dict in transListDict if item_str in dict])
        
        # Only conserve frequent item-set 
        n = len(transListSet)
        for item, cnt in localSet_.items():
            itemSet_.add(item) if float(cnt)/n >= minSupp else None
        
        return itemSet_

#objPrefixSpan = GSP(0.01, 0.4)
#itemCountDict, freqSet = objPrefixSpan.fit(r'C:\Users\amana\Downloads\Assignment2\Assignment2\reviews_sample.txt')
objPrefixSpan = GSP(0.66, 0.4)
itemCountDict, freqSet = objPrefixSpan.fit(r'C:\Users\amana\Downloads\Assignment2\Assignment2\test_sample.txt')

text_file = open(r'C:\Users\amana\Downloads\Assignment2\Assignment2\patterns.txt', "w")
for key, value in freqSet.items():
        print('frequent {}-term set:'.format(key))
        print('-'*20)
        for itemset in value:
            #text_file.write(itemCountDict[itemset],':'s)
            print("",file=text_file)
            print(itemCountDict[itemset],':',file=text_file, end="",sep='')
            for list_item in list(itemset):
                if (list_item != list(itemset)[0]):
                    print(";", file = text_file, end="",sep='')    
                print(list_item, file = text_file, end="",sep='')
      
text_file.close()        



