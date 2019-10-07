# encoding: utf-8 
import time ,os,time,datetime,random

random.seed(time)

# index select max(aid) from idbase;
index = 1

def Shuffer_1(start, end,fielname):
    global index
    glist=[i for i in range(start, end+1)];
    random.shuffle(glist)    
    with open(fielname, 'w') as f:
        for use in glist:
            index = index +1
            f.writelines("%d\t%d\n" % (index, use))


filename = "/opt/CardServer/randid.txt"

#Shuffer_1(10000000, 30000000, filename)
Shuffer_1(10000000, 11000000, filename)

