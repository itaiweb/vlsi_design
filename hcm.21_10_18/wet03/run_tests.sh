#!/bin/bash
GRN='\x1B[32m'
NC='\033[0m'
for((i = 1; i <= 13; i++)); do
        echo -e ${GRN} test $i: ${NC}
        ./gl_verilog_fev -s TEST${i} ../ISCAS-85/stdcell.v ./test1.v -i TEST${i} ../ISCAS-85/stdcell.v ./test2.v
done