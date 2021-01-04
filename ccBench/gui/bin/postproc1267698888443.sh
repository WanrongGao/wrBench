#!/bin/sh
echo "##### starting postproceeding / selected kernels"
echo "##### starting \"/home/daniel/BenchIT/BenchITv6/kernel/arch_x86_64/memory_bandwidth/C/pthread/SSE2/single-r1w1\""
/home/daniel/BenchIT/BenchITv6/COMPILE.SH arch_x86_64.memory_bandwidth.C.pthread.SSE2.single-r1w1
/home/daniel/BenchIT/BenchITv6/RUN.SH arch_x86_64.memory_bandwidth.C.pthread.SSE2.single-r1w1.0
echo "##### finished \"/home/daniel/BenchIT/BenchITv6/kernel/arch_x86_64/memory_bandwidth/C/pthread/SSE2/single-r1w1(all Selected)\""
echo "##### removing shellscript "

rm -f postproc1267698888443.sh
