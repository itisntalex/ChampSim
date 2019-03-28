#!/bin/bash

mkdir -p traces
while IFS='' read -r trace_file; do
  wget -P traces -c http://hpca23.cse.tamu.edu/champsim-traces/speccpu/$trace_file
done < scripts/traces.txt
