#!/bin/bash
#
# CASSAVA ---------------------------------------------------------------------
#
rm -f TME204.HiFi_HiC.haplotig1.fa CASSAVA.fa
wget -o CASSAVA.fa https://ftp.cngb.org/pub/gigadb/pub/10.5524/102001_103000/102193/00_Assembly_Fasta/haplotigs/TME204.HiFi_HiC.haplotig1.fa
#
# HUMAN -----------------------------------------------------------------------
#
rm -f chm13v2.0.fa.gz chm13v2.0.fa;
wget https://s3-us-west-2.amazonaws.com/human-pangenomics/T2T/CHM13/assemblies/analysis_set/chm13v2.0.fa.gz
gunzip chm13v2.0.fa.gz;
mv chm13v2.0.fa HUMAN.fa
#
# CY --------------------------------------------------------------------------
#
rm -f CY.fa
efetch -db nucleotide -id "NC_000024.1" -format fasta > CY.fa
#
# COPUS -----------------------------------------------------------------------
#
ls
#
# -----------------------------------------------------------------------------
