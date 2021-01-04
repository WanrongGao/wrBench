#!/bin/sh
echo "##### starting postproceeding / selected kernels"
echo "##### starting \"/home/daniel/BenchIT/BenchITv6/kernel/arch_x86-64/throughput/C/pthread/SIMD/multiple-worker-vt-inst-manual\""
/home/daniel/BenchIT/BenchITv6/COMPILE.SH arch_x86-64.throughput.C.pthread.SIMD.multiple-worker-vt-inst-manual
/home/daniel/BenchIT/BenchITv6/RUN.SH arch_x86-64.throughput.C.pthread.SIMD.multiple-worker-vt-inst-manual.0
echo "##### finished \"/home/daniel/BenchIT/BenchITv6/kernel/arch_x86-64/throughput/C/pthread/SIMD/multiple-worker-vt-inst-manual(all Selected)\""
echo "##### removing shellscript "

rm -f postproc1306760178323.sh
