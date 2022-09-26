import sys
import re
import matplotlib.pyplot as plt
import numpy as np

print("Using traces " + str(sys.argv[1:]))

regex = re.compile(r"DATA: Clock: (?P<clock>[0-9]+) MaxDev: (?P<maxd>[0-9]+.[0-9]*e?[\+\-]?[0-9]*) Colours: (?P<colours>[0-9]+)")

traces = []

for inputname in sys.argv[1:]:
    f= open(inputname, "r")

    steps = []
    devs = []
    cols = []


    for line in f.readlines():
        line = line[:-1]

        match = regex.match(line)
        if match:
            groups = match.groupdict()
            steps.append(int(groups["clock"]))
            devs.append(float(groups["maxd"]))
            cols.append(int(groups["colours"]))

    traces.append({"name":inputname, "steps":steps, "deviation":devs, "colours":cols})

for trace in traces:
    if trace["steps"] != list(range(len(traces[0]["steps"])+1))[1:]:
    #if trace["steps"] != traces[0]["steps"]:
        raise ValueError("Non matching steps")


mean_dev = []
mean_col = []

for step in traces[0]["steps"]:
    mean_dev.append(sum([trace["deviation"][step-1] for trace in traces]) / len(traces))
    mean_col.append(sum([trace["colours"][step-1] for trace in traces]) / len(traces))


#traces.append({"name":"MEANTRACE", "steps":list(range(len(traces[0]["steps"])+1))[1:], "deviation":mean_dev, "colours":mean_col})

plt.rcParams['figure.figsize'] = [5, 4]


fig, ax = plt.subplots()
ax.set_ylim(10**-2, 10)
ax.set_yscale("log")

for trace in traces:
    ax.plot(trace["steps"], trace["deviation"], label=trace["name"])

ax.set(xlabel="Time Step", ylabel="Distance", title="Maximum Radius Deviation")
ax.grid()
ax.legend()
fig.savefig("radius_deviation.png", dpi=300)
plt.show()

fig2, ax2 = plt.subplots()
ax2.set_ylim(0,6)

for trace in traces:
    ax2.plot(trace["steps"], trace["colours"], label=trace["name"])

ax2.set(xlabel="Time Step", ylabel="Colour Count", title="Number of Distinct LED Colours")
ax2.grid()
ax2.legend()
fig2.savefig("num_colours.png", dpi=300)
plt.show()



