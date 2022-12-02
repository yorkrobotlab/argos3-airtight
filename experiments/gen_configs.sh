#!/bin/sh
for i in $(seq 1 10); do python3 gen_circle_config2.py airtight $i 0 0 > circle_config-airtight-$i.argos; done
for i in $(seq 1 10); do python3 gen_circle_config2.py naive_broadcast $i > circle_config-broadcast-$i.argos; done
for i in $(seq 1 10); do python3 gen_circle_config2.py naive_p2p $i > circle_config-p2p-$i.argos; done

#for i in $(seq 1 20); do python3 gen_circle_config2.py airtight $i 1 0 > circle_config-airtightsst-$i.argos; done
#for i in $(seq 1 20); do python3 gen_circle_config2.py airtight $i 0 1 > circle_config-airtightsp-$i.argos; done
for i in $(seq 1 10); do python3 gen_circle_config2.py airtight $i 1 1 > SPcircle_config-airtight-$i.argos; done

for i in $(seq 1 10); do python3 gen_flock_config.py airtight $i 0 1.0 > flocking/airtight-1.0-$i.argos ; done
for i in $(seq 1 10); do python3 gen_flock_config.py naive_broadcast $i 0 1.0 > flocking/broadcast-1.0-$i.argos ; done
for i in $(seq 1 10); do python3 gen_flock_config.py naive_p2p $i 0 1.0 > flocking/p2p-1.0-$i.argos ; done

for i in $(seq 1 10); do python3 gen_flock_config.py airtight $i 0 0.8 > flocking/airtight-0.8-$i.argos ; done
for i in $(seq 1 10); do python3 gen_flock_config.py naive_broadcast $i 0 0.8 > flocking/broadcast-0.8-$i.argos ; done
for i in $(seq 1 10); do python3 gen_flock_config.py naive_p2p $i 0 0.8 > flocking/p2p-0.8-$i.argos ; done

for i in $(seq 1 10); do python3 gen_flock_config.py airtight $i 0 0.6 > flocking/airtight-0.6-$i.argos ; done
for i in $(seq 1 10); do python3 gen_flock_config.py naive_broadcast $i 0 0.6 > flocking/broadcast-0.6-$i.argos ; done
for i in $(seq 1 10); do python3 gen_flock_config.py naive_p2p $i 0 0.6 > flocking/p2p-0.6-$i.argos ; done

for i in $(seq 1 10); do python3 gen_flock_config.py airtight $i 0 0.4 > flocking/airtight-0.4-$i.argos ; done
for i in $(seq 1 10); do python3 gen_flock_config.py naive_broadcast $i 0 0.4 > flocking/broadcast-0.4-$i.argos ; done
for i in $(seq 1 10); do python3 gen_flock_config.py naive_p2p $i 0 0.4 > flocking/p2p-0.4-$i.argos ; done

for i in $(seq 1 10); do python3 gen_flock_config.py airtight $i 0 0.2 > flocking/airtight-0.2-$i.argos ; done
for i in $(seq 1 10); do python3 gen_flock_config.py naive_broadcast $i 0 0.2 > flocking/broadcast-0.2-$i.argos ; done
for i in $(seq 1 10); do python3 gen_flock_config.py naive_p2p $i 0 0.2 > flocking/p2p-0.2-$i.argos ; done

