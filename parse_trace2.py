import sys
import re

print("Using trace " + sys.argv[1])

inputname = sys.argv[1]

f = open(inputname, "r")

steps = []
devs = []
cols = []

#regex = re.compile(r"DATA: Clock: (?P<clock>[0-9]+) MaxDev: (?P<maxd>[0-9]+.[0-9]*e?[\+\-]?[0-9]*) Colours: (?P<colours>[0-9]+)")
regex = re.compile(r"DATA: Clock: (?P<clock>[0-9]+) MaxDev: (?P<maxd>[0-9]+.?[0-9]*e?[\+\-]?[0-9]*) Colours: (?P<colours>[0-9]+)")

for line in f.readlines():
    #print(line)
    line = line[:-1]

    match = regex.match(line)
    if match:
        groups = match.groupdict()
        steps.append(int(groups["clock"]))
        devs.append(float(groups["maxd"]))
        cols.append(int(groups["colours"]))
 #   if regex.match(line)
 #   if line.startswith("DATA:"):
 #       parts = line.split(" ")
 #       dev = float(parts[2])
 #       col = int(parts[4])

 #       devs.append(dev)
 #       cols.append(col)

#print(steps)


import matplotlib.pyplot as plt
import numpy as np

plt.rcParams['figure.figsize'] = [5, 4]

#steps = np.arange(1, 1+len(devs), 1)

fig, ax = plt.subplots()
#ax.plot(steps, np.cumsum(devs))
ax.plot(steps, devs)
ax.set(xlabel="Time Step", ylabel="Distance", title="Maximum Radius Deviation")
ax.grid()
#plt.yscale("log")
#plt.ylim(10**-2, 10**3)
fig.savefig("radius_deviation-%s.png" % inputname, dpi=300)
plt.show()

fig2, ax2 = plt.subplots()
ax2.plot(steps, cols)

ax2.set(xlabel="Time Step", ylabel="Colour Count", title="Number of Distinct LED Colours")
ax2.grid()

fig2.savefig("num_colours-%s.png" % inputname, dpi=300)
plt.show()


