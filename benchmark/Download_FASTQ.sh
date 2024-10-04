#!/bin/bash
#
# SRA files -------------------------------------------------------------------------
#
declare -a DATASETS_SRA=("SRR1284073" "SRR9046049" "SRR8858470" "ERR3307082") # "SRR10338867" "SRR10512946")
#
for dataset in "${DATASETS_SRA[@]}"
  do
  echo -e "\e[34m[Downloader]\e[32m Download from SRA - $dataset ...\e[0m";
  ./sratoolkit.3.0.7-ubuntu64/bin/prefetch $dataset
  ./sratoolkit.3.0.7-ubuntu64/bin/fastq-dump --split-3 $dataset/$dataset.sra
done
#
