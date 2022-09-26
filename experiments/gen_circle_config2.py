from copy import deepcopy
from math import sin, cos, pi
from sys import argv, stderr
from random import shuffle
from airtight_analysis import runAnalysis

NODES = 6
RADIUS = 1

LED_PERIOD = 100
MOV_PERIOD = 500

#implementation = "airtight"
#implementation = "naive_broadcast"
#implementation = "naive_p2p"

#seed = 0

implementation = argv[1]
seed = int(argv[2])

if implementation == "airtight":
    sst = {"0":False, "1":True}[argv[3]]
    sp = {"0":False, "1":True}[argv[4]]
else:
    sst = False
    sp = False

stderr.write("Using implementation %s with random seed %d\n" % (implementation, seed))


if implementation == "airtight":
    flows = []
    for i in range(NODES-1):
        flows.append({"period":LED_PERIOD, "criticality":"LO"})

    for i in range(NODES-1):
        flows.append({"period":MOV_PERIOD, "criticality":"HI"})

    runAnalysis(flows, NODES)

    stderr.write("SUM of LEDFLOWS: "+str(sum([flow["RLo"] for flow in flows[:NODES-1]]))+"\n")
    stderr.write("SUM of MOVFLOWS: "+str(sum([flow["RHi"] for flow in flows[NODES-1:]]))+"\n")

    stderr.write(str(flows))


print("""<?xml version="1.0" ?>
<argos-configuration>

  <!-- ************************* -->
  <!-- * General configuration * -->
  <!-- ************************* -->
  <framework>
    <system threads="0" />
    <experiment length="240" ticks_per_second="100" random_seed="%d" />
  </framework>

  <!-- *************** -->
  <!-- * Controllers * -->
  <!-- *************** -->
  <controllers>
    <circle_controller library="build/controllers/circle/libcircle_controller"
                       id="circle_controller">
      <actuators>
        <pipuck_differential_drive implementation="default" />
        <slot_radio implementation="%s" medium="a1" />
        <big_leds implementation="default" />
      </actuators>
      <sensors>
        <slot_radio implementation="%s" medium="a1" show_rays="true"/>
      </sensors>
    </circle_controller>
  </controllers>

  <!-- ****************** -->
  <!-- * Loop functions * -->
  <!-- ****************** -->
  <loop_functions library="build/loop_functions/libcircle_logging_loop_functions"
                  label="circle_logging_loop_functions" />

  <!-- *********************** -->
  <!-- * Arena configuration * -->
  <!-- *********************** -->
  <arena size="20, 20, 1">

    <box id="wall_north" size="20,0.1,0.25" movable="false">
      <body position="0,10,0" orientation="0,0,0" />
    </box>
    <box id="wall_south" size="20,0.1,0.25" movable="false">
      <body position="0,-10,0" orientation="0,0,0" />
    </box>
    <box id="wall_east" size="0.1,20,0.25" movable="false">
      <body position="10,0,0" orientation="0,0,0" />
    </box>
    <box id="wall_west" size="0.1,20,0.25" movable="false">
      <body position="-10,0,0" orientation="0,0,0" />
    </box>
""" % (seed, implementation, implementation))


pos = [(sin(2*pi*i/NODES), cos(2*pi*i/NODES), (90 - i * (360 / NODES)) % 360) for i in range(NODES)]
slot_tables = [",".join([str(int(i==j)) for j in range(NODES)]) for i in range(NODES)]

if sst:
    shuffle(slot_tables)

if sp:
    shuffle(pos)

for i in range(NODES):
    angle = 2 * pi * i / NODES

    x,y,o = pos[i]

    #o = (360 * angle / (2*pi) + 90) % 360
    #o = 360 * i / NODES
    #o = (90 - i * (360 / NODES)) % 360
    stderr.write(str(o)+"\n")


    print('    <pipuck id="node%d">' % i)
    print('      <body position="%.15g,%.15g,0" orientation="%.15g,0,0"/>' % (x, y, o))
    print('      <controller config="circle_controller">')
    print('        <params>')


    if implementation == "airtight":
        #slotTable = ",".join([str(int(i==j)) for j in range(NODES)])
        print('          <airtight slotTable="%s">' % slot_tables[i])

        localflows = deepcopy(flows)

        # LED Buffers

        for j in range(i+2, i+NODES):
            flow = localflows.pop(0)
            print('            <buffer name="fwdLed%d%d-%d" criticality=%d recipient="node%d" fwdFrom="node%d" fwdBuffer="fwdLed%d%d-%d" RLo="%d" RHi="%d" period="%d" />'
              % (j % NODES, # Global FWD From
                 (j-1) % NODES, # Global FWD To
                 i % NODES, # STEP,
                 {"LO":0, "HI":1}[flow["criticality"]],
                 (i+1) % NODES, # Local FWD To
                 (i-1) % NODES, # Local FWD From
                 j % NODES, # Global FWD From
                 (j-1) % NODES, # Global FWD To
                 (i-1) % NODES, # Prev step
                 flow["RLo"],
                 flow["RHi"],
                 LED_PERIOD))

        flow = localflows.pop(0)
        print('            <buffer name="fwdLed%d%d-%d" criticality=%d recipient="node%d" alias="led" RLo="%d" RHi="%d" period="%d" />' % (i, (i-1) % NODES, i, {"LO":0, "HI":1}[flow["criticality"]], (i+1) % NODES, flow["RLo"], flow["RHi"], LED_PERIOD))

        # Move Buffers
        for j in range(i+2, i+NODES):
            flow = localflows.pop(0)
            print('            <buffer name="fwdMov%d%d-%d" criticality=%d recipient="node%d" fwdFrom="node%d" fwdBuffer="fwdMov%d%d-%d" RLo="%d" RHi="%d" period="%d" />'
              % (j % NODES, # Global FWD From
                 (j-1) % NODES, # Global FWD To
                 i % NODES, # STEP,
                 {"LO":0, "HI":1}[flow["criticality"]],
                 (i+1) % NODES, # Local FWD To
                 (i-1) % NODES, # Local FWD From
                 j % NODES, # Global FWD From
                 (j-1) % NODES, # Global FWD To
                 (i-1) % NODES, # Prev step
                 flow["RLo"],
                 flow["RHi"],
                 MOV_PERIOD))

        flow = localflows.pop(0)
        print('            <buffer name="fwdMov%d%d-%d" criticality=%d recipient="node%d" alias="mov" RLo="%d" RHi="%d" period="%d" />' % (i, (i-1) % NODES, i, {"LO":0, "HI":1}[flow["criticality"]], (i+1) % NODES, flow["RLo"], flow["RHi"], MOV_PERIOD))

        print('          </airtight>')
    elif implementation == "naive_p2p":
        print('          <naive_p2p maxBackoffExp="2" maxRetries="5">')
        for j in range(NODES):
            if j != i:
                print('            <recipient>node%d</recipient>' % j)
        print('          </naive_p2p>')

    elif implementation == "naive_broadcast":
        print('          <naive_broadcast numRepeats="13" />')

    print('        </params>')
    print('      </controller>')
    print('    </pipuck>')


print("""
  </arena>

  <!-- ******************* -->
  <!-- * Physics engines * -->
  <!-- ******************* -->
  <physics_engines>
    <dynamics2d id="dyn2d" />
  </physics_engines>

  <!-- ********* -->
  <!-- * Media * -->
  <!-- ********* -->
  <media>
    <slot_radio id="a1" />
  </media>

  <!-- ****************** -->
  <!-- * Visualization * -->
  <!-- ****************** -->
  <visualization>
    <qt-opengl show_boundary="false">
      <user_functions library="build/loop_functions/libcircle_opengl_functions"
                      label="circle_opengl_functions" />
      <camera>
        <placements>
          <placement index="0" position="0,0,50" look_at="0,0,0" up="1,0,0" lens_focal_length="65" />
        </placements>
      </camera>
    </qt-opengl>
  </visualization>

</argos-configuration>
""")
