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

regex = re.compile(r"DATA: Clock: (?P<clock>[0-9]+) MaxDev: (?P<maxd>[0-9]+.?[0-9]*e?[\+\-]?[0-9]*) Colours: (?P<colours>[0-9]+) RMin: (?P<rmin>[0-9]+.?[0-9]*e?[\+\-]?[0-9]*) RMax: (?P<rmax>[0-9]+.?[0-9]*e?[\+\-]?[0-9]*)")
plot_traces = []

bucket_size = 3000

for group in sys.argv[1:]:
    files = glob.glob(group)
    print("Parsing group "+group+" with files "+str(files))

    traces = []

    for trace in files:
        f = open(trace, "r")

        steps = []
        devs = []
        cols = []
        rmin = []
        rmax = []

        for line in f.readlines():
            line = line[:-1]

            match = regex.match(line)
            if match:
                groups = match.groupdict()
                clock = int(groups["clock"])
                if clock <= 24000:
                    steps.append(clock)

                    devs.append(float(groups["maxd"]) * 100.0)
                    rmin.append(float(groups["rmin"]))
                    rmax.append(float(groups["rmax"]))
                #if d > 10**-5:
                #    devs.append(d)
                #else:
                #    devs.append(0)
                #devs.append(max(5*10**-5, float(groups["maxd"])))
                    cols.append(int(groups["colours"]))

        traces.append({"name":trace, "steps":steps, "deviation":devs, "colours":cols, "rmin":rmin, "rmax":rmax})

    for trace in traces:
        if trace["steps"] != list(range(len(traces[0]["steps"])+1))[1:]:
            import pdb; pdb.set_trace()
            raise ValueError("Non matching steps for trace "+trace["name"]+" vs "+traces[0]["name"])


    buckets = []
    #for bucket_start in range(1, len(trace["steps"]), bucket_size):
    #    bucket = []
    #    for trace in traces:
    #        for step in range(bucket_start, bucket_start+bucket_size):
    #            try:
    #                bucket.append(trace["deviation"][step-1])
    #            except:
    #                import pdb; pdb.set_trace()
    #    buckets.append(bucket)
    for step in range(bucket_size, len(trace["steps"])+1, bucket_size):
        #data = []
        #for trace in traces:
        #    data.extend(trace["deviation"][step-bucket_size:step])
        #buckets.append(data)
        buckets.append([trace["deviation"][step-1] for trace in traces])

    min_dev = []
    mean_dev = []
    med_dev = []
    max_dev = []
    min_col = []
    max_col = []
    med_col = []
    mean_col = []

    min_rmin = []
    max_rmax = []

    print(len(traces))

    for step in traces[0]["steps"]:
        devs = [trace["deviation"][step-1] for trace in traces]
        min_dev.append(min(devs))
        max_dev.append(max(devs))
        med_dev.append(median(devs))
        mean_dev.append(sum(devs) / len(traces))

        cols = [trace["colours"][step-1] for trace in traces]
        min_col.append(min(cols))
        max_col.append(max(cols))
        med_col.append(median(cols))
        mean_col.append(sum(cols) / len(traces))

        #min_rmin.append(min([trace["rmin"][step-1] for trace in traces]))
        #max_rmax.append(max([trace["rmax"][step-1] for trace in traces]))
        min_rmin.append(min([(trace["rmin"][step-1] + trace["rmax"][step-1]) / 2 for trace in traces]))
        max_rmax.append(max([(trace["rmin"][step-1] + trace["rmax"][step-1]) / 2 for trace in traces]))

    plot_traces.append({"name":group,
                        "steps":list(range(len(traces[0]["steps"])+1))[1:],
                        "min_dev":min_dev,
                        "max_dev":max_dev,
                        "med_dev":med_dev,
                        "deviation":mean_dev,
                        "devbuckets":buckets,
                        "min_col":min_col,
                        "max_col":max_col,
                        "med_col":med_col,
                        "colours":mean_col,
                        "rmin":min_rmin,
                        "rmax":max_rmax})

#plt.style.use("ggplot")
#plt.rcParams['figure.figsize'] = [8, 2]
plt.rcParams['figure.figsize'] = [12, 5]

#fig, ax = plt.subplots()
#ax.set_ylim(10**-6, 10)
#ax.set_yscale("log")

#for trace in plot_traces:
#    #ax.plot(trace["steps"], trace["deviation"], label=trace["name"])
#    ax.plot(trace["steps"], trace["med_dev"], label=trace["name"])
#    ax.fill_between(trace["steps"], trace["min_dev"], trace["max_dev"], alpha=0.5)

#ax.set(xlabel="Time Step", ylabel="Distance", title="Maximum Radius Deviation")
#ax.grid()
#ax.legend()
#fig.savefig("radius_deviation.png", dpi=300)
#plt.show()

#fig1, ax1 = plt.subplots(1, len(plot_traces), constrained_layout=True)
#for (i, trace) in enumerate(plot_traces):
    #ax1[i].set_ylim(0, 10)
    #ax1[1].tick_params(axis="y", labelsize=8)

    #colour = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#8c564b', '#e377c2', '#7f7f7f', '#bcbd22', '#17becf'][i]
    #ax1[i].fill_between(trace["steps"], trace["rmin"], trace["rmax"], alpha=0.2, color=colour)
    ##ax1[i].plot(trace["steps"], [(trace["rmin"][i]+trace["rmax"][i])/2 for i in range(len(steps))], color=colour)

    #if i == 0:
        #ax1[i].set_ylabel("Circle Radius", fontdict={"fontsize":10})
    #else:
        #ax1[i].set_yticklabels([])

    #ax1[i].set_xlabel(["AirTight", "Broadcast", "Point to Point"][i])

    #ax1[i].set_xticks([i for i in range(0, trace["steps"][-1], 1000) if i % 3000 != 0], minor=True)
    #ax1[i].set_xticks([0.0, 3000.0, 6000.0, 9000.0, 12000.0, 15000.0, 18000.0, 21000.0, 24000.0])#,
    #ax1[i].set_xticklabels(["0s", "30s", "60s", "90s", "120s", "150s", "180s", "210s","240s"], fontdict={"fontsize":8})

    #ax1[i].set_yticks([i for i in range(11)])

    #ax1[i].grid()

#fig1.savefig("circle_radius.pdf")



fig2, ax2 = plt.subplots(2, len(plot_traces)//2, constrained_layout=True)

#fig2.suptitle("Number of nodes showing incorrect colour")


for (i, trace) in enumerate(plot_traces):

    ax2[i//2][i%2].set_title(["AirTight (Optimal Routing)", "AirTight (Randomised Routing)", "Broadcast", "Point-to-Point"][i], fontdict={"fontsize":14})
    ax2[i//2][i%2].set_ylim(0,5)

    ax2[i//2][i%2].tick_params(axis='x', labelsize=8)
    ax2[i//2][i%2].tick_params(axis='y', labelsize=10)

    colour = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#8c564b', '#e377c2', '#7f7f7f', '#bcbd22', '#17becf'][i]

    #ax2[i].plot(trace["steps"], trace["colours"], alpha=1, linewidth=1, color=colour)
    ax2[i//2][i%2].plot(trace["steps"], trace["med_col"], alpha=1, linewidth=1, color=colour)
    ax2[i//2][i%2].fill_between(trace["steps"], trace["min_col"], trace["max_col"], alpha=0.15, color=colour)

    if (i%2) == 0:
        ax2[i//2][i%2].set_ylabel("$E_{LED}$", fontdict={"fontsize":12})
    else:
        ax2[i//2][i%2].set_yticklabels([])

    #ax2[i].set_xlabel(["AirTight", "Broadcast", "Point-to-Point"][i])

    #ax2[i].set(xlabel="Time Step", ylabel="Colour Count", title="Number of Distinct LED Colours")
    #ax2[i].legend(loc="upper left")

    ax2[i//2][i%2].set_xticks([i for i in range(0, trace["steps"][-1], 1000) if i % 3000 != 0], minor=True)
    ax2[i//2][i%2].set_xticks([0.0, 3000.0, 6000.0, 9000.0, 12000.0, 15000.0, 18000.0, 21000.0, 24000.0])#,
    ax2[i//2][i%2].set_xticklabels(["0s", "30s", "60s", "90s", "120s", "150s", "180s", "210s","240s"], fontdict={"fontsize":8})

#fig2.savefig("num_colours.pgf")
fig2.savefig("num_colours.pdf")











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

plt.rcParams['figure.figsize'] = [12, 6]


num_buckets = len(plot_traces[0]["devbuckets"])
fig4, ax = plt.subplots(2, int(ceil(num_buckets / 2.0)), constrained_layout=True)

#fig4.suptitle("BBB")

for i in range(num_buckets):

    # import pdb; pdb.set_trace()

    ax[i // int(ceil(num_buckets / 2.0))][i % int(ceil(num_buckets / 2.0))].set_title("$n = "+str((bucket_size//100)*(i+1))+"$s", fontdict={"fontsize":14})
#    ax[i // int(ceil(num_buckets / 2.0))][i % int(ceil(num_buckets / 2.0))].set_ylim(-2, 150)
   # ax[i].set_yscale("symlog", linthresh=1, linscale=(1, 0.5))
   # ax[i].tick_params(axis='x', labelrotation=90, labelsize=6)
    ax[i // int(ceil(num_buckets / 2.0))][i % int(ceil(num_buckets / 2.0))].tick_params(axis='x', labelsize=12)
    ax[i // int(ceil(num_buckets / 2.0))][i % int(ceil(num_buckets / 2.0))].tick_params(axis='y', labelsize=12)
#    ax[i].major_ticklabels.set_ha("left")

    if (i % int(ceil(num_buckets / 2.0))) == 0:
        ax[i // int(ceil(num_buckets / 2.0))][i % int(ceil(num_buckets / 2.0))].set_ylabel("$E_{POS}$ (cm)", fontdict={"fontsize":12})
    else:
        ax[i // int(ceil(num_buckets / 2.0))][i % int(ceil(num_buckets / 2.0))].set_yticklabels([])

    if i // int(ceil(num_buckets / 2.0)) == 0:
        ax[i // int(ceil(num_buckets / 2.0))][i % int(ceil(num_buckets / 2.0))].set_ylim(-0.5, 60)
    else:
        ax[i // int(ceil(num_buckets / 2.0))][i % int(ceil(num_buckets / 2.0))].set_ylim(-2.5, 300)

    data = [trace["devbuckets"][i] for trace in plot_traces]
    ax[i // int(ceil(num_buckets / 2.0))][i % int(ceil(num_buckets / 2.0))].boxplot(data, whis=(0,100), labels=["A/O", "A/R","B","P"], flierprops={"marker":"."})

#fig4.savefig("boxplot2.pgf")
fig4.savefig("boxplot2.pdf")

#import tikzplotlib

#tikzplotlib.clean_figure()
#tikzplotlib.save("test.tex")

