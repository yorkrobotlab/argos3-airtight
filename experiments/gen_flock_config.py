from copy import deepcopy
from math import sin, cos, pi, ceil
from sys import argv, stderr
from random import shuffle, uniform, randint
from random import seed as rng_seed
from airtight_analysis import runAnalysis


NODES = 10
RADIUS = 1

POS_PERIOD = 500
DAT_PERIOD = 33

#implementation = "airtight"
#implementation = "naive_broadcast"
#implementation = "naive_p2p"

#seed = 0

implementation = argv[1]
seed = int(argv[2])
rng_seed(seed)

if implementation == "airtight":
    sst = {"0":False, "1":True}[argv[3]]
else:
    sst = False

pdrMod = float(argv[4])

stderr.write("Using implementation %s with random seed %d\n" % (implementation, seed))


if implementation == "airtight":
    flows = []

    flows.append({"period":DAT_PERIOD, "criticality":"LO"})

    for i in range(NODES-1):
        flows.append({"period":POS_PERIOD, "criticality":"LO"})

    for i in range(1,NODES):
        flows[i]["criticality"] = "HI"

    #flows.append({"period":POS_PERIOD/2, "criticality":"LO"})


    runAnalysis(flows, NODES)

    #sum_of_flows = sum([flow["RLo"] for flow in flows[:NODES-1]])

    #stderr.write("SUM of FLOWS: "+str(sum_of_flows)+"\n")
    #stderr.write("SUM of MOVFLOWS: "+str(sum([flow["RHi"] for flow in flows[NODES-1:]]))+"\n")

    #if sum_of_flows > POS_PERIOD:
    #    stderr.write("***SUM of Flows exceeds POS_PERIOD***")

    stderr.write(str(flows))


print("""<?xml version="1.0" ?>
<argos-configuration>

  <!-- ************************* -->
  <!-- * General configuration * -->
  <!-- ************************* -->
  <framework>
    <system threads="0" />
    <experiment length="300" ticks_per_second="100" random_seed="%d" />
  </framework>

  <!-- *************** -->
  <!-- * Controllers * -->
  <!-- *************** -->
  <controllers>
    <flocking_controller library="build/controllers/flocking/libflocking_controller"
                       id="flocking_controller">
      <actuators>
        <pipuck_differential_drive implementation="default" />
        <slot_radio implementation="%s" medium="a1" />
      </actuators>
      <sensors>
        <slot_radio implementation="%s" medium="a1" show_rays="true"/>
        <pipuck_rangefinders implementation="default" show_rays="true" />
        <positioning implementation="default" />
      </sensors>
    </flocking_controller>
  </controllers>

  <!-- ****************** -->
  <!-- * Loop functions * -->
  <!-- ****************** -->
  <loop_functions library="build/loop_functions/libflocking_loop_functions"
                  label="flocking_loop_functions" />

  <!-- *********************** -->
  <!-- * Arena configuration * -->
  <!-- *********************** -->
  <arena size="10, 10, 1">

    <box id="wall_north" size="10,0.1,0.25" movable="false">
      <body position="0,5,0" orientation="0,0,0" />
    </box>
    <box id="wall_south" size="10,0.1,0.25" movable="false">
      <body position="0,-5,0" orientation="0,0,0" />
    </box>
    <box id="wall_east" size="0.1,10,0.25" movable="false">
      <body position="5,0,0" orientation="0,0,0" />
    </box>
    <box id="wall_west" size="0.1,10,0.25" movable="false">
      <body position="-5,0,0" orientation="0,0,0" />
    </box>
""" % (seed, implementation, implementation))


slot_tables = [",".join([str(int(i==j)) for j in range(NODES)]) for i in range(NODES)]

if sst:
    shuffle(slot_tables)


for i in range(NODES):
    angle = 2 * pi * i / NODES

    x,y,o = (uniform(-2.0, 2.0), uniform(-2.0, 2.0), randint(0, 360))

    #o = (360 * angle / (2*pi) + 90) % 360
    #o = 360 * i / NODES
    #o = (90 - i * (360 / NODES)) % 360
    stderr.write(str(o)+"\n")


    print('    <pipuck id="node%d">' % i)
    print('      <body position="%.15g,%.15g,0" orientation="%.15g,0,0"/>' % (x, y, o))
    print('      <controller config="flocking_controller">')
    print('        <params controlPeriod="%d" >' % POS_PERIOD)


    if implementation == "airtight":
        #slotTable = ",".join([str(int(i==j)) for j in range(NODES)])
        print('          <airtight slotTable="%s">' % slot_tables[i])

        localflows = deepcopy(flows)

        # LED Buffers

        #flow = localflows.pop(0)
        #print('            <buffer name="pos%d%d-%d" criticality=%d recipient="node%d" alias="pos" RLo="%d" RHi="%d" period="%d" />' % (i, (i-1) % NODES, i, {"LO":0, "HI":1}[flow["criticality"]], (i+1) % NODES, flow["RLo"], flow["RHi"], POS_PERIOD))

        flow = localflows.pop(0)
        if i != 0:
            print('            <buffer name="data" criticality=%d recipient="node%d" alias="pos" RLo="%d" RHi="%d" period="%d" />'
                  % ({"LO":0, "HI":1}[flow["criticality"]],
                     0,
                     flow["RLo"],
                     flow["RHi"],
                     DAT_PERIOD))

        for j in range(i+1, i+NODES):
            flow = localflows.pop(0)
            print('            <buffer name="pos%d%d" criticality=%d recipient="node%d" RLo="%d" RHi="%d" period="%d" />'
              % (i % NODES, # Global FWD From
                 j % NODES, # Global FWD To
                 {"LO":0, "HI":1}[flow["criticality"]],
                 j % NODES,
                 flow["RLo"],
                 flow["RHi"],
                 POS_PERIOD))

        #if i != 0:
        #    print('            <buffer name="data" criticality=%d recipient="node%d" alias="pos" RLo="%d" RHi="%d" period="%d" />'
        #        % (0, 0, 1, -1, 50))



        print('          </airtight>')
    elif implementation == "naive_p2p":
        print('          <naive_p2p maxBackoffExp="2" maxRetries="5">')
        print('            <buffer name="pos">')
        for j in range(i+1, i+NODES):
            print('              <recipient>node%d</recipient>' % (j % NODES))
        print('            </buffer>')
        print('            <buffer name="data">')
        print('              <recipient>node0</recipient>')
        print('            </buffer>')
        print('          </naive_p2p>')

    elif implementation == "naive_broadcast":
        print('          <naive_broadcast numRepeats="%d" />' % ((POS_PERIOD-1) // (NODES*(1+ceil((POS_PERIOD)//DAT_PERIOD)))))

    elif implementation == "protocol1":
        print('          <airtight slotTable="%s">' % slot_tables[i])
        print('            <buffer name="data" criticality=0 recipient=""  RLo="33" RHi="-1" period="33" />')
        print('            <buffer name="pos" criticality=1 recipient=""  RLo="221" RHi="400" period="500" />')
        print('          </airtight>')

    print('        </params>')
    print('      </controller>')
    print('    </pipuck>')


print("""

   <!-- <distribute>
      <position method="uniform" min="-4,-4,0" max="4,4,0" />
      <orientation method="gaussian" mean="0,0,0" std_dev="360,0,0" />
      <entity quantity="5" max_trials="100">
          <box id="obstacle" size="1,1,0.25" movable="false" />
      </entity>
    </distribute>-->
  </arena>

  <!-- ******************* -->
  <!-- * Physics engines * -->
  <!-- ******************* -->
  <physics_engines>
    <dynamics2d id="dyn2d" iterations="5" />
  </physics_engines>

  <!-- ********* -->
  <!-- * Media * -->
  <!-- ********* -->
  <media>
    <slot_radio id="a1" pdrModifier="%f" />
  </media>

  <!-- ****************** -->
  <!-- * Visualization * -->
  <!-- ****************** -->
  <visualization>
    <qt-opengl show_boundary="false">
      <camera>
        <placements>
          <placement index="0" position="0,0,30" look_at="0,0,0" up="1,0,0" lens_focal_length="65" />
        </placements>
      </camera>
    </qt-opengl>
  </visualization>

</argos-configuration>
""" % pdrMod)
