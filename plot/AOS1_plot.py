#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Oct 23 00:23:58 2021

@author: aaron
"""

import matplotlib.pyplot as plt
import pandas as pd

title = ["algoName", "memSize", "pageFault", "Interrupt", "write"]
algoName = ["FIFO", "Optimal", "ARB", "LIFO", "NRA"]
performance = ["Page faults", "Interrupts", "Disk writes"]
dataName = ["Random data", "Locality data", "Normal random data"]
mark = ["o", "v", "s", "*", "D"]

font = {'family' : 'normal',
        'weight' : 'bold',
        'size'   : 18}

plt.rc('font', **font)

# 1. The plots between performance and frames each algorithm.
for i in range(5) :
    data = pd.read_csv(algoName[i] + ".csv")
    k = 0
    for j in range(0, 15, 5) :
    
        plt.plot(data[title[1]][j:j+5], data[title[2]][j:j+5], label=performance[0], linewidth=5, linestyle='--', marker=mark[0], markersize=15)
        plt.plot(data[title[1]][j:j+5], data[title[3]][j:j+5], label=performance[1], linewidth=5, linestyle=':', marker=mark[1], markersize=15)
        plt.plot(data[title[1]][j:j+5], data[title[4]][j:j+5], label=performance[2], linewidth=5, alpha=0.5, marker=mark[2], markersize=15)
        
        plt.title(algoName[i] + " : " + dataName[k], fontsize=36, fontweight='bold')
        plt.xlabel("The number of frames", fontsize=24, fontweight='bold')
        plt.ylabel("Values", fontsize=24, fontweight='bold')
        plt.xlim(20, 160)
        plt.ylim(0)
        plt.legend()
        plt.grid(True)
        
        fig = plt.gcf()
        fig.set_size_inches(16, 12)
        k += 1
        fig.savefig("img/" + algoName[i] + str(k) + '.jpg', dpi=100)
        plt.clf()


#2. The plots between performance and frames each data.
FIFO = pd.read_csv(algoName[0] + ".csv")
Optimal = pd.read_csv(algoName[1] + ".csv")
ARB = pd.read_csv(algoName[2] + ".csv")
LIFO = pd.read_csv(algoName[3] + ".csv")
NRA = pd.read_csv(algoName[4] + ".csv")


for i in range(2, 5, 1) :
    for j in range(0, 15, 5) :
        plt.plot(FIFO[title[1]][j:j+5],    FIFO[title[i]][j:j+5], label=algoName[0], linewidth=5, linestyle='--', marker=mark[0], markersize=15)
        plt.plot(Optimal[title[1]][j:j+5], Optimal[title[i]][j:j+5], label=algoName[1], linewidth=5, linestyle=':', marker=mark[1], markersize=15)
        plt.plot(ARB[title[1]][j:j+5],     ARB[title[i]][j:j+5], label=algoName[2], linewidth=5, linestyle='-', marker=mark[2], markersize=15, alpha=0.5)
        plt.plot(LIFO[title[1]][j:j+5],    LIFO[title[i]][j:j+5], label=algoName[3], linewidth=5, linestyle=':', marker=mark[3], markersize=15)
        plt.plot(NRA[title[1]][j:j+5],     NRA[title[i]][j:j+5], label=algoName[4], linewidth=5, linestyle=':', marker=mark[4], markersize=15, alpha=0.5)
        
        plt.title(dataName[int(j/5)], fontsize=36, fontweight='bold')
        plt.xlabel("The number of frames", fontsize=24, fontweight='bold')
        plt.ylabel("The number of " + performance[i-2].lower(), fontsize=24, fontweight='bold')
        plt.xlim(20, 160)
        #plt.ylim(0)
        plt.legend()
        plt.grid(True)
        
        fig = plt.gcf()
        fig.set_size_inches(16, 12)
        fig.savefig("img/" + dataName[int(j/5)] + str(i-1) + '.jpg', dpi=100)
        plt.clf()
        #print(str(int(j/5+1)) + " " +str(i-1))
        