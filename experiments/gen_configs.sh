#!/bin/sh
#for i in $(seq 1 10); do python3 gen_circle_config2.py airtight $i 0 0 > circle_config-airtight-$i.argos; done
#for i in $(seq 1 10); do python3 gen_circle_config2.py naive_broadcast $i > circle_config-broadcast-$i.argos; done
#for i in $(seq 1 10); do python3 gen_circle_config2.py naive_p2p $i > circle_config-p2p-$i.argos; done

#for i in $(seq 1 20); do python3 gen_circle_config2.py airtight $i 1 0 > circle_config-airtightsst-$i.argos; done
#for i in $(seq 1 20); do python3 gen_circle_config2.py airtight $i 0 1 > circle_config-airtightsp-$i.argos; done
for i in $(seq 1 20); do python3 gen_circle_config2.py airtight $i 1 1 > SPcircle_config-airtight-$i.argos; done
