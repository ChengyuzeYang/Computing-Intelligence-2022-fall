import numpy as np
import statsmodels.api as sm 
import matplotlib.pyplot as plt
import os

f_cities = "bays29.tsp"

cities_x = []
cities_y = []

with open(f_cities) as f:
    for _ in range(38):
        line = f.readline()
    for _ in range(29):
        line = f.readline()
        s = line.split()
        #print(s)
        cities_x.append(float(s[1]))
        cities_y.append(float(s[2]))

f_opt_route = "bays29.opt.tour"

opt_route = []
opt_x = []
opt_y = []

with open(f_opt_route) as f:
    for _ in range(5):
        line = f.readline()
    for _ in range(29):
        line = f.readline()
        idx = (int)(line) - 1
        opt_route.append(idx)
        opt_x.append(cities_x[idx])
        opt_y.append(cities_y[idx])


for file in os.listdir(".\output"):
    plt.figure(file + ".png")
    plt.title(file)
    plt.fill(opt_x,opt_y,edgecolor='r',fill=False)
    plt.plot(opt_x,opt_y,'o')

    f_neurons = "output\\" + file
    f_winners = "winner\\" + file

    neurons_x = []
    neurons_y = []
    winners_x = []
    winners_y = []

    with open(f_neurons) as f:
        line = f.readline()
        while line:
            cor = line.split()
            neurons_x.append(float(cor[0]))
            neurons_y.append(float(cor[1]))
            line = f.readline()

    with open(f_winners) as f:
        line = f.readline()
        while line:
            cor = line.split()
            winners_x.append(float(cor[0]))
            winners_y.append(float(cor[1]))
            line = f.readline()

    plt.scatter(neurons_x,neurons_y,s=10,c='b',marker ='o')
    plt.fill(winners_x,winners_y,fill=False)
    if len(file) < 4:
        plt.savefig("graphs\\0" + file + ".png")
    else:
        plt.savefig("graphs\\" + file + ".png")