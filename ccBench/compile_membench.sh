#!/bin/bash
rm -rf compile_membench.log
rm -rf compile_membench.err

echo -e "\nthroughput:\n" | tee -a compile_membench.log
echo -e "\nthroughput:\n" >> compile_membench.err
./COMPILE.SH kernel/arch_x86-64/throughput/C/pthread/SIMD/multiple-worker-vt-inst-manual/ 2>> compile_membench.err | tee -a compile_membench.log
echo -e "\nbandwidth/multiple-reader:\n" | tee -a compile_membench.log
echo -e "\nbandwidth/multiple-reader:\n" >> compile_membench.err
./COMPILE.SH kernel/arch_x86-64/memory_bandwidth/C/pthread/SIMD/multiple-reader/ 2>> compile_membench.err | tee -a compile_membench.log
echo -e "\nbandwidth/multiple-writer:\n" | tee -a compile_membench.log
echo -e "\nbandwidth/multiple-writer:\n" >> compile_membench.err
./COMPILE.SH kernel/arch_x86-64/memory_bandwidth/C/pthread/SIMD/multiple-writer/ 2>> compile_membench.err | tee -a compile_membench.log
echo -e "\nbandwidth/multiple-r1w1:\n" | tee -a compile_membench.log
echo -e "\nbandwidth/multiple-r1w1:\n" >> compile_membench.err
./COMPILE.SH kernel/arch_x86-64/memory_bandwidth/C/pthread/SIMD/multiple-r1w1/ 2>> compile_membench.err | tee -a compile_membench.log
echo -e "\nbandwidth/single-reader:\n" | tee -a compile_membench.log
echo -e "\nbandwidth/single-reader:\n" >> compile_membench.err
./COMPILE.SH kernel/arch_x86-64/memory_bandwidth/C/pthread/SIMD/single-reader/ 2>> compile_membench.err | tee -a compile_membench.log
echo -e "\nbandwidth/single-writer:\n" | tee -a compile_membench.log
echo -e "\nbandwidth/single-writer:\n" >> compile_membench.err
./COMPILE.SH kernel/arch_x86-64/memory_bandwidth/C/pthread/SIMD/single-writer/ 2>> compile_membench.err | tee -a compile_membench.log
echo -e "\nbandwidth/single-r1w1:\n" | tee -a compile_membench.log
echo -e "\nbandwidth/single-r1w1:\n" >> compile_membench.err
./COMPILE.SH kernel/arch_x86-64/memory_bandwidth/C/pthread/SIMD/single-r1w1/ 2>> compile_membench.err | tee -a compile_membench.log
echo -e "\nlatency/read:\n" | tee -a compile_membench.log
echo -e "\nlatency/read:\n" >> compile_membench.err
./COMPILE.SH kernel/arch_x86-64/memory_latency/C/pthread/0/read/ 2>> compile_membench.err | tee -a compile_membench.log

echo -e "\n Errors:\n"
cat compile_membench.err

