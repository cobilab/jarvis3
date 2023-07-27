#!/bin/bash
#
wget -o CASSAVA.fa https://ftp.cngb.org/pub/gigadb/pub/10.5524/102001_103000/102193/00_Assembly_Fasta/haplotigs/TME204.HiFi_HiC.haplotig1.fa
#
./Install_Tools.sh
#
./Benchmark_FASTA.sh VIRUS_REFS_NCBI_15_JULY.fa
./Benchmark_FASTA.sh CY.fa
./Benchmark_FASTA.sh CASSAVA.fa
./Benchmark_FASTA.sh HUMAN.fa
#
