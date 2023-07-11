#!/bin/sh
mkdir -p dfrontier
for cm in nc hc cc mc ; do for i in $(seq 1 100) ; do python3 gen_frontier_config.py $cm $i 0.95 0.5 1.0 > dfrontier/$cm-prot1-pdr-0.95_0.5_1.0-$(printf "%04d" $i).argos ; done ; done
for cm in nc hc cc mc ; do for i in $(seq 1 100) ; do python3 gen_frontier_config.py $cm $i 0.95 0.5 2.0 > dfrontier/$cm-prot1-pdr-0.95_0.5_2.0-$(printf "%04d" $i).argos ; done ; done
for cm in nc hc cc mc ; do for i in $(seq 1 100) ; do python3 gen_frontier_config.py $cm $i 0.95 0.5 4.0 > dfrontier/$cm-prot1-pdr-0.95_0.5_4.0-$(printf "%04d" $i).argos ; done ; done
for cm in nc hc cc mc ; do for i in $(seq 1 100) ; do python3 gen_frontier_config.py $cm $i 0.95 0.5 8.0 > dfrontier/$cm-prot1-pdr-0.95_0.5_8.0-$(printf "%04d" $i).argos ; done ; done
