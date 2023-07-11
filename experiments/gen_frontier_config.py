from sys import argv, stderr
from random import shuffle, uniform, randint
from random import seed as rng_seed

NODES = 10
size = 6.0

cohesion = {"nc":0, "cc":1, "mc":2, "hc":3}[argv[1]]
seed = int(argv[2])
rng_seed(seed)

pdrMod = float(argv[3])
dropoffStart = float(argv[4])
dropoffFactor = float(argv[5])

print("""<?xml version="1.0" ?>
<argos-configuration>

  <!-- ************************* -->
  <!-- * General configuration * -->
  <!-- ************************* -->
  <framework>
    <system threads="0" />
    <experiment length="1200" ticks_per_second="100" random_seed="{seed}" />
  </framework>

  <!-- *************** -->
  <!-- * Controllers * -->
  <!-- *************** -->
  <controllers>
    <dfrontier_controller library="build/controllers/dfrontier/libdfrontier_controller"
                       id="dfrontier_controller">
      <actuators>
        <pipuck_differential_drive implementation="default" />
        <slot_radio implementation="protocol1" medium="a1" />
      </actuators>
      <sensors>
        <slot_radio implementation="protocol1" medium="a1" show_rays="true"/>
        <pipuck_rangefinders implementation="default" show_rays="true" />
        <positioning implementation="default" />
      </sensors>
    </dfrontier_controller>
  </controllers>

  <!-- ****************** -->
  <!-- * Loop functions * -->
  <!-- ****************** -->
  <loop_functions library="build/loop_functions/libdfrontier_loop_functions"
                  label="dfrontier_loop_functions" />

  <!-- *********************** -->
  <!-- * Arena configuration * -->
  <!-- *********************** -->
   <arena size="{asize}, {asize}, 1">

    <box id="wall_north" size="{size},0.01,0.25" movable="false">
      <body position="0,{hsize},0" orientation="0,0,0" />
    </box>
    <box id="wall_south" size="{size},0.01,0.25" movable="false">
      <body position="0,{nhsize},0" orientation="0,0,0" />
    </box>
    <box id="wall_east" size="0.01,{size},0.25" movable="false">
      <body position="{hsize},0,0" orientation="0,0,0" />
    </box>
    <box id="wall_west" size="0.01,{size},0.25" movable="false">
      <body position="{nhsize},0,0" orientation="0,0,0" />
    </box>
""".format(seed=seed, size=size, asize=size+0.05, hsize=size/2.0,nhsize=-size/2.0))

slot_tables = [",".join([str(int(i==j)) for j in range(NODES)]) for i in range(NODES)]

positions = []

bx = uniform(-2, 2)
by = uniform(-2, 2)

for i in range(NODES):
    x,y,o = (bx+uniform(-0.8, 0.8), by+uniform(-0.8, 0.8), randint(0, 360))

    attempts = 0
    while positions and min((x-k[0])**2 + (y-k[1])**2 for k in positions) < 0.3**2:
        if attempts > 20:
            raise RuntimeError("Couldn't place robots maintaing separation")
        x,y,o = (bx+uniform(-0.9, 0.9), by+uniform(-0.9, 0.9), randint(0, 360))
        attempts += 1

    positions.append((x,y))

    print('    <pipuck id="node%d">' % i)
    print('      <body position="%.15g,%.15g,0" orientation="%.15g,0,0"/>' % (x, y, o))
    print('      <controller config="dfrontier_controller">')
    print('        <params cohesionMode="{cohesion}">'.format(cohesion=cohesion))

    print('          <protocol1 slotTable="%s">' % slot_tables[i])
    print('            <buffer name="position" criticality=1 recipient="" period="80" targetAcks="{targetAcks}"/>'.format(targetAcks=NODES-1))
    print('            <buffer name="coverage" criticality=1 recipient="" period="-1" targetAcks="{targetAcks}" />'.format(targetAcks=NODES-1))
    print('          </protocol1>')

    print('        </params>')
    print('      </controller>')
    print('    </pipuck>')



for i in range(randint(1,4)):
    x,y,o = (uniform(-2.1, 2.1), uniform(-2.1, 2.1), randint(0, 360))

    attempts = 0
    while positions and min((x-k[0])**2 + (y-k[1])**2 for k in positions) < 0.8**2:
        if attempts > 20:
            break
        x,y,o = (uniform(-2.1, 2.1), uniform(-2.1, 2.1), randint(0, 360))
        attempts += 1

    if attempts > 20:
        continue

    positions.append((x,y))

    print("""
    <box id="box-l-{i}" size="0.4,0.4,0.25" movable="false">
      <body position="{x},{y},0" orientation="{o},0,0" />
    </box>""".format(i=i, x=x, y=y, o=o))

for i in range(randint(2,6)):
    x,y,o = (uniform(-2.1, 2.1), uniform(-2.1, 2.1), randint(0, 360))

    attempts = 0
    while positions and min((x-k[0])**2 + (y-k[1])**2 for k in positions) < 0.8**2:
        if attempts > 20:
            break
        x,y,o = (uniform(-2.1, 2.1), uniform(-2.1, 2.1), randint(0, 360))
        attempts += 1

    if attempts > 20:
        continue

    positions.append((x,y))

    print("""
    <box id="box-s-{i}" size="0.2,0.2,0.25" movable="false">
      <body position="{x},{y},0" orientation="{o},0,0" />
    </box>""".format(i=i, x=x, y=y, o=o))

for i in range(randint(1,3)):
    x,y = (uniform(-2.1, 2.1), uniform(-2.1, 2.1))

    attempts = 0
    while positions and min((x-k[0])**2 + (y-k[1])**2 for k in positions) < 0.8**2:
        if attempts > 20:
            break
        x,y = (uniform(-2.1, 2.1), uniform(-2.1, 2.1))
        attempts += 1

    if attempts > 20:
        continue

    positions.append((x,y))

    print("""
    <cylinder id="cyl-l-{i}" height="0.25" radius="0.2" movable="false">
      <body position="{x},{y},0" orientation="0,0,0" />
    </cylinder>""".format(i=i, x=x, y=y))


for i in range(randint(1,4)):
    x,y = (uniform(-2.1, 2.1), uniform(-2.1, 2.1))

    attempts = 0
    while positions and min((x-k[0])**2 + (y-k[1])**2 for k in positions) < 0.8**2:
        if attempts > 20:
            break
        x,y = (uniform(-2.1, 2.1), uniform(-2.1, 2.1))
        attempts += 1

    if attempts > 20:
        continue

    positions.append((x,y))

    print("""
    <cylinder id="cyl-s-{i}" height="0.25" radius="0.10" movable="false">
      <body position="{x},{y},0" orientation="0,0,0" />
    </cylinder>""".format(i=i, x=x, y=y))


print("""
  </arena>
\
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
    <slot_radio id="a1" pdrModifier="{pdrMod}" dropoffStart="{dropoffStart}" dropoffFactor="{dropoffFactor}" />
  </media>

  <!-- ****************** -->
  <!-- * Visualization * -->
  <!-- ****************** -->
  <visualization>
    <qt-opengl show_boundary="false">
      <user_functions library="build/loop_functions/libdfrontier_opengl_functions"
        label="dfrontier_opengl_functions" />
      <camera>
        <placements>
          <placement index="0" position="0,0,15" look_at="0,0,0" up="1,0,0" lens_focal_length="65" />
        </placements>
      </camera>
    </qt-opengl>
  </visualization>

</argos-configuration>
""".format(pdrMod=pdrMod, dropoffStart=dropoffStart, dropoffFactor=dropoffFactor))
