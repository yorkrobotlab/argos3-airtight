import matplotlib
matplotlib.use("pdf")

import sys
import re
import matplotlib.pyplot as plt
import numpy as np
import glob
import itertools
from statistics import median
from math import ceil

print("Using trace groups " + str(sys.argv[1:]))

#regex = re.compile(r"DATA: Clock: (?P<clock>[0-9]+) MaxDev: (?P<maxd>[0-9]+.?[0-9]*e?[\+\-]?[0-9]*) Colours: (?P<colours>[0-9]+) RMin: (?P<rmin>[0-9]+.?[0-9]*e?[\+\-]?[0-9]*) RMax: (?P<rmax>[0-9]+.?[0-9]*e?[\+\-]?[0-9]*)")
regex = re.compile(r"Mean velocity was (?P<velocity>[0-9].[0-9]+) Mean max centroid distance was (?P<dist>[0-9].[0-9]+) Data points (?P<data>[0-9]+)")
raw_traces = []


groupre = re.compile(r"traces_n/(?P<prot>(airtight)|(broadcast)|(p2p)|(protocol1))-(?P<pdrmod>[0-9].[0-9]+)-")

for group in sys.argv[1:]:
    files = glob.glob(group)
    print("Parsing group "+group+" with files "+str(files))

    print(group)
    grm = groupre.match(group).groupdict()

    traces = []

    for trace in files:
        f = open(trace, "r")
        line = f.readlines()[-1]
        match = regex.match(line)

        if match:
            groups = match.groupdict()
            # * 100 Steps per second * 100 cm in m
            velocity = float(groups["velocity"]) * 100 * 100
            data = int(groups["data"])
            traces.append({"v":velocity, "d":data})
        else:
            print("NOMATCH")

    print("Avg velocity for group", group, "was", sum([t["v"] for t in traces]) / len(traces))
    print("Avg data points for group", group, "was", sum([t["d"] for t in traces]) / len(traces))

    print("")
    raw_traces.append([grm, traces])

plot_traces = {}
for raw_trace in raw_traces:
    if raw_trace[0]["pdrmod"] not in plot_traces:
        plot_traces[raw_trace[0]["pdrmod"]] = {}
    plot_traces[raw_trace[0]["pdrmod"]][raw_trace[0]["prot"]] = raw_trace[1]



#plt.style.use("ggplot")
#plt.rcParams['figure.figsize'] = [8, 2]
plt.rcParams['figure.figsize'] = [6, 2.2]
plt.rcParams['text.usetex'] = True

print(plot_traces)

fig2, ax2 = plt.subplots(1, len(plot_traces), constrained_layout=True, sharey=True)

#fig2.suptitle("Mean Flocking Velocity (cm/s)")

#print(len(ax2))

for (i, (tracename, tracedata)) in enumerate(sorted(plot_traces.items(), reverse=True)):

    ax2[i].set_title("$PDR \mathrel{*}= "+tracename+"$", fontdict={"fontsize":10})

    ax2[i].set_ylim(0,7)
    ax2[i].tick_params(axis="y", labelsize=12)

    data = [[t["v"] for t in tracedata["airtight"]], [t["v"] for t in tracedata["broadcast"]], [t["v"] for t in tracedata["p2p"]], [t["v"] for t in tracedata["protocol1"]]]
    ax2[i].boxplot(data, whis=(0,100), labels=["A", "B", "P", "1"])

fig2.savefig("flock_velocity2.pdf")

#fig2, ax2 = plt.subplots(1, 1, constrained_layout=True)

#fig2.suptitle("Mean Flocking Velocity (cm/s)")

#print(len(ax2))

#for (i, (tracename, tracedata)) in enumerate(sorted(plot_traces.items(), reverse=True)):

    #ax2[i//3][i%3].set_title("$PDR \mathrel{*}= "+tracename+"$", fontdict={"fontsize":14})

    #ax2[i//3][i%3].set_ylim(0,7)
    #ax2[i//3][i%3].tick_params(axis="y", labelsize=12)
#    ax2.set_ylim(0,6.8)

#    data = [[t["v"] for t in tracedata["airtight"]], [t["v"] for t in tracedata["broadcast"]], [t["v"] for t in tracedata["p2p"]]]
#    ax2.boxplot(data, whis=(0,100), labels=["A","B","P"], positions=[5*i, 5*i+1, 5*i+2], widths=0.6)
    #ax2[i//3][i%3].boxplot(data, whis=(0,100), labels=["A", "B", "P"])

#fig2.savefig("flock_velocity2.pdf")



#plt.rcParams['figure.figsize'] = [10, 5]

#fig3, ax3 = plt.subplots(2, ceil(len(plot_traces)/2), constrained_layout=True)

#fig3.suptitle("Total Number of Collected Data Samples")

#for (i, (tracename, tracedata)) in enumerate(sorted(plot_traces.items(), reverse=True)):

    #ax3[i//3][i % 3].set_title("$PDR \mathrel{*}= "+tracename+"$", fontdict={"fontsize":14})

    #ax2[i//2][i%2].set_title(["AirTight (Optimal Routing)", "AirTight (Randomised Routing)", "Broadcast", "Point-to-Point"][i], fontdict={"fontsize":14})
    #ax3[i//3][i % 3].set_ylim(0,9000)
    #ax3[i//3][i % 3].tick_params(axis="y", labelsize=12)

    #if i%3 != 0:
        #ax3[i//3][i % 3].set_yticklabels([])

    #data = [[t["d"] for t in tracedata["airtight"]], [t["d"] for t in tracedata["broadcast"]], [t["d"] for t in tracedata["p2p"]]]
    #ax3[i//3][i % 3].boxplot(data, whis=(0,100), labels=["A", "B", "P"])

#fig3.savefig("data_points.pdf")






plt.rcParams['figure.figsize'] = [6, 2.2]
fig4, ax4 = plt.subplots(1,len(plot_traces),constrained_layout=True, sharey=True)

airtight = []
broadcast = []
p2p = []
prot1 = []



datalabels = []
data = []
lerror = []
herror = []
labels = ["AirTight", "Broadcast", "Point-to-Point", "Protocol1"]

# Labels (G1/G2/G3...) :: Protocol (AirTight/Broadcast/...)
#

for (i, (tracename, tracedata)) in enumerate(sorted(plot_traces.items(), reverse=True)):
    ax4[i].set_title("$PDR \mathrel{*}= "+tracename+"$", fontsize=10)

    ad = [t["d"] for t in tracedata["airtight"]]
    mean_ad = sum(ad) / len(ad)
    max_ad_e = max(ad) - mean_ad
    min_ad_e = mean_ad - min(ad)
    airtight.append((mean_ad, max_ad_e, min_ad_e))
    bd = [t["d"] for t in tracedata["broadcast"]]
    mean_bd = sum(bd) / len(bd)
    max_bd_e = max(bd) - mean_bd
    min_bd_e = mean_bd - min(bd)
    broadcast.append((mean_bd, max_bd_e, min_bd_e))
    pd = [t["d"] for t in tracedata["p2p"]]
    mean_pd = sum(pd) / len(pd)
    max_pd_e = max(pd) - mean_pd
    min_pd_e = mean_pd - min(pd)
    p2p.append((mean_pd, max_pd_e, min_pd_e))
    p1d = [t["d"] for t in tracedata["protocol1"]]
    mean_p1d = sum(p1d) / len(p1d)
    max_p1d_e = max(p1d) - mean_p1d
    min_p1d_e = mean_p1d - min(p1d)
    prot1.append((mean_p1d, max_p1d_e, min_p1d_e))

    ax4[i].bar(0, mean_ad, 1, label="AirTight", yerr=[[max_ad_e], [min_ad_e]])
    ax4[i].bar(1, mean_bd, 1, label="Broadcast", yerr=[[max_bd_e],[min_bd_e]])
    ax4[i].bar(2, mean_pd, 1, label="Point-to-Point", yerr=[[max_pd_e], [min_pd_e]])
    ax4[i].bar(3, mean_p1d, 1, label="Protocol1", yerr=[[max_p1d_e], [min_p1d_e]])

    ax4[i].set_xticks([0,1,2,3], ["A","B","P", "1"])


    datalabels.append(tracename)
    #data.append((mean_ad, mean_bd, mean_pd))
    #lerror.append((min_ad_e, min_bd_e, min_pd_e))
    #herror.append((max_ad_e, max_bd_e, max_pd_e))


print(labels)
x_range = np.arange(len(datalabels))

print(x_range)
#f = 1 / 0.0

#ax4.set_ylim(0, 10000.0)

#ax4.bar(x_range - 0.2, [i[0] for i in airtight], 0.2, label="AirTight", yerr=[[i[2] for i in airtight], [i[1] for i in airtight]])
#ax4.bar([], [], label="_Dummy") # Keep colours consistentish
#ax4.bar(x_range, [i[0] for i in broadcast], 0.2, label="Broadcast", yerr=[[i[2] for i in broadcast], [i[1] for i in broadcast]])
#ax4.bar(x_range + 0.2, [i[0] for i in p2p], 0.2, label="Point-to-Point", yerr=[[i[2] for i in p2p], [i[1] for i in p2p]])





#for i in range(3):
    #print(x_range + i*(0.5/len(data)))
    #print(data[i])
    #print(datalabels[i])
    #print("\n\n")
    #h = ax4.bar(x_range + (i-1)*0.25,

   # h = ax4.bar(x_range + i*(0.75/len(data)), data[i], 0.75/len(data), label=datalabels[i], align="edge", yerr=[lerror[i], herror[i]])

#fig4.legend()#bbox_to_anchor=(0, 1.0, 1, 0), loc="upper left", mode="expand", ncol=10)
#ax4.set_xticks(x_range, datalabels)
#fig4.suptitle("Total Number of Collected Data Samples")


print(x_range)
print(labels)
fig4.savefig("data_points_bars2.pdf")



#plt.rcParams['figure.figsize'] = [6, 3]
#fig4, ax4 = plt.subplots(1,1,constrained_layout=True)

#airtight = []
#broadcast = []
#p2p = []

#datalabels = []
#data = []
#lerror = []
#herror = []
#labels = ["AirTight", "Broadcast", "Point-to-Point"]

#Labels (G1/G2/G3...) :: Protocol (AirTight/Broadcast/...)


#for (i, (tracename, tracedata)) in enumerate(sorted(plot_traces.items(), reverse=True)):
    #ad = [t["d"] for t in tracedata["airtight"]]
    #mean_ad = sum(ad) / len(ad)
    #max_ad_e = max(ad) - mean_ad
    #min_ad_e = mean_ad - min(ad)
    #bd = [t["d"] for t in tracedata["broadcast"]]
    #mean_bd = sum(bd) / len(bd)
    #max_bd_e = max(bd) - mean_bd
    #min_bd_e = mean_bd - min(bd)
    #pd = [t["d"] for t in tracedata["p2p"]]
    #mean_pd = sum(pd) / len(pd)
    #max_pd_e = max(pd) - mean_pd
    #min_pd_e = mean_pd - min(pd)

    #datalabels.append(tracename)
    #data.append((mean_ad, mean_bd, mean_pd))
    #lerror.append((min_ad_e, min_bd_e, min_pd_e))
    #herror.append((max_ad_e, max_bd_e, max_pd_e))


#means = sum([t["d"] for t in tracedata["airtight"]]) / len(tracedata["airtight"])
#print(labels)
#x_range = np.arange(len(labels))

#print(x_range)
#f = 1 / 0.0

#ax4.set_ylim(0, 11000.0)

#for i in range(len(data)):
    #print(x_range + i*(0.5/len(data)))
    #print(data[i])
    #print(datalabels[i])
    #print("\n\n")

    #h = ax4.bar(x_range + i*(0.75/len(data)), data[i], 0.75/len(data), label=datalabels[i], align="edge", yerr=[lerror[i], herror[i]])
    #ax4.bar_label(h, padding=3)

#ax4.legend(bbox_to_anchor=(0, 1.0, 1, 0), loc="upper left", mode="expand", ncol=len(data), title="PDR Scaling Factor")
#ax4.set_xticks(x_range + (len(data)/2)*(0.75/len(data)), labels)
#ax4.set_title("Collected Data Samples")


#print(x_range)
#print(labels)
#fig4.savefig("data_points_bars.pdf")

#x_range + i*(0.5/len(data))
#    data = [trace["devbuckets"][i] for trace in plot_traces]
#    ax[i // int(ceil(num_buckets / 2.0))][i % int(ceil(num_buckets / 2.0))].boxplot(data, whis=(0,100), labels=["A/O", "A/R","B","P"], flierprops={"marker":"."})




    #ax2[i//2][i%2].tick_params(axis='x', labelsize=8)
    #ax2[i//2][i%2].tick_params(axis='y', labelsize=10)

    #colour = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#8c564b', '#e377c2', '#7f7f7f', '#bcbd22', '#17becf'][i]

    #ax2[i].plot(trace["steps"], trace["colours"], alpha=1, linewidth=1, color=colour)
    #ax2[i//2][i%2].plot(trace["steps"], trace["med_col"], alpha=1, linewidth=1, color=colour)
    #ax2[i//2][i%2].fill_between(trace["steps"], trace["min_col"], trace["max_col"], alpha=0.15, color=colour)

    #if (i%2) == 0:
        #ax2[i//2][i%2].set_ylabel("$E_{LED}$", fontdict={"fontsize":12})
    #else:
        #ax2[i//2][i%2].set_yticklabels([])

    #ax2[i].set_xlabel(["AirTight", "Broadcast", "Point-to-Point"][i])

    #ax2[i].set(xlabel="Time Step", ylabel="Colour Count", title="Number of Distinct LED Colours")
    #ax2[i].legend(loc="upper left")

#    ax2[i//2][i%2].set_xticks([i for i in range(0, trace["steps"][-1], 1000) if i % 3000 != 0], minor=True)
#    ax2[i//2][i%2].set_xticks([0.0, 3000.0, 6000.0, 9000.0, 12000.0, 15000.0, 18000.0, 21000.0, 24000.0])#,
#    ax2[i//2][i%2].set_xticklabels(["0s", "30s", "60s", "90s", "120s", "150s", "180s", "210s","240s"], fontdict={"fontsize":8})

#fig2.savefig("num_colours.pgf")
#fig2.savefig("num_colours.pdf")











#fig3, ax3 = plt.subplots(1, len(plot_traces), constrained_layout=True)
#for (i, trace) in enumerate(plot_traces):
    #ax3[i].set_ylim(0,5)
    #ax3[i].tick_params(axis='y', labelsize=8)
    #colour = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#8c564b', '#e377c2', '#7f7f7f', '#bcbd22', '#17becf'][i]
    #ax3[i].plot(trace["steps"][::5], trace["med_col"][::5], alpha=1, linewidth=1, color=colour)
    ##ax2[i].plot(trace["steps"], trace["med_col"], label="median", alpha=1, linewidth=1)
    #ax3[i].fill_between(trace["steps"][::5], trace["min_col"][::5], trace["max_col"][::5], alpha=0.4, color=colour)

    #if i == 0:
        #ax3[i].set_ylabel("$E_{LED}$", fontdict={"fontsize":10})
    #else:
        #ax3[i].set_yticklabels([])

    #ax3[i].set_xlabel(["AirTight", "Broadcast", "Point to Point", "ASST", "ASP", "ASSTP"][i])

    ##ax2[i].set(xlabel="Time Step", ylabel="Colour Count", title="Number of Distinct LED Colours")
    ##ax2[i].legend(loc="upper left")

    #ax3[i].set_xticks([i for i in range(0, trace["steps"][-1], 1000) if i % 3000 != 0], minor=True)
    #ax3[i].set_xticks([0.0, 3000.0, 6000.0, 9000.0, 12000.0, 15000.0, 18000.0])#,
    #ax3[i].set_xticklabels(["0s", "30s", "60s", "90s", "120s", "150s", "180s"], fontdict={"fontsize":8})

##fig2.savefig("num_colours.pgf")
#fig3.savefig("num_colours-ds.png", dpi=600)







#fig3, ax3 = plt.subplots()
#ax3.set_ylim(-1, 5000)
#ax3.set_yscale("symlog", linthresh=1)
#plotdata = list(itertools.chain.from_iterable(zip(*[trace["devbuckets"] for trace in plot_traces])))
#import pdb; pdb.set_trace()
#positions = [(i // len(plot_traces)) * (len(plot_traces) + 1) + (i % len(plot_traces)) for i in range(len(plotdata))]
#ax3.boxplot(plotdata, positions=positions, whis=(0,100))
#fig3.savefig("boxplot.png", dpi=300)

#plt.rcParams['figure.figsize'] = [12, 6]


#num_buckets = len(plot_traces[0]["devbuckets"])
#fig4, ax = plt.subplots(2, int(ceil(num_buckets / 2.0)), constrained_layout=True)

#fig4.suptitle("BBB")

#for i in range(num_buckets):

    # import pdb; pdb.set_trace()

#    ax[i // int(ceil(num_buckets / 2.0))][i % int(ceil(num_buckets / 2.0))].set_title("$n = "+str((bucket_size//100)*(i+1))+"$s", fontdict={"fontsize":14})
##    ax[i // int(ceil(num_buckets / 2.0))][i % int(ceil(num_buckets / 2.0))].set_ylim(-2, 150)
#   # ax[i].set_yscale("symlog", linthresh=1, linscale=(1, 0.5))
#   # ax[i].tick_params(axis='x', labelrotation=90, labelsize=6)
#    ax[i // int(ceil(num_buckets / 2.0))][i % int(ceil(num_buckets / 2.0))].tick_params(axis='x', labelsize=12)
#    ax[i // int(ceil(num_buckets / 2.0))][i % int(ceil(num_buckets / 2.0))].tick_params(axis='y', labelsize=12)
##    ax[i].major_ticklabels.set_ha("left")

#    if (i % int(ceil(num_buckets / 2.0))) == 0:
#        ax[i // int(ceil(num_buckets / 2.0))][i % int(ceil(num_buckets / 2.0))].set_ylabel("$E_{POS}$ (cm)", fontdict={"fontsize":12})
#    else:
#        ax[i // int(ceil(num_buckets / 2.0))][i % int(ceil(num_buckets / 2.0))].set_yticklabels([])

#    if i // int(ceil(num_buckets / 2.0)) == 0:
#        ax[i // int(ceil(num_buckets / 2.0))][i % int(ceil(num_buckets / 2.0))].set_ylim(-0.5, 60)
#    else:
#        ax[i // int(ceil(num_buckets / 2.0))][i % int(ceil(num_buckets / 2.0))].set_ylim(-2.5, 300)#

#    data = [trace["devbuckets"][i] for trace in plot_traces]
#    ax[i // int(ceil(num_buckets / 2.0))][i % int(ceil(num_buckets / 2.0))].boxplot(data, whis=(0,100), labels=["A/O", "A/R","B","P"], flierprops={"marker":"."})

#fig4.savefig("boxplot2.pgf")
#fig4.savefig("boxplot2.pdf")

#import tikzplotlib

#tikzplotlib.clean_figure()
#tikzplotlib.save("test.tex")

