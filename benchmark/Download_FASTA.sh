#!/bin/bash
#
# CASSAVA ---------------------------------------------------------------------
#
echo -e "\e[34m[Downloader]\e[32m Download CASSAVA ...\e[0m";
rm -f TME204.HiFi_HiC.haplotig1.fa CASSAVA.fa
wget -o CASSAVA.fa https://ftp.cngb.org/pub/gigadb/pub/10.5524/102001_103000/102193/00_Assembly_Fasta/haplotigs/TME204.HiFi_HiC.haplotig1.fa
rm CASSAVA.fa
mv TME204.HiFi_HiC.haplotig1.fa CASSAVA.fa
#
# HUMAN -----------------------------------------------------------------------
#
echo -e "\e[34m[Downloader]\e[32m Download HUMAN ...\e[0m";
rm -f chm13v2.0.fa.gz chm13v2.0.fa;
wget https://s3-us-west-2.amazonaws.com/human-pangenomics/T2T/CHM13/assemblies/analysis_set/chm13v2.0.fa.gz
gunzip chm13v2.0.fa.gz;
mv chm13v2.0.fa HUMAN.fa
#
# CY --------------------------------------------------------------------------
#
echo -e "\e[34m[Downloader]\e[32m Download CY ...\e[0m";
rm -f CY.fa
efetch -db nucleotide -id "NC_000024.1" -format fasta > CY.fa
#
# CORPUS ----------------------------------------------------------------------
#
echo -e "\e[34m[Downloader]\e[32m Download CORPUS ...\e[0m";
rm -fr DNACorpus
wget http://sweet.ua.pt/pratas/datasets/DNACorpus.zip
unzip DNACorpus.zip
cat DNACorpus/* | ./gto_fasta_from_seq -n "CORPUS" > CORPUS.fa
#
# EXON ------------------------------------------------------------------------
#
echo -e "\e[34m[Downloader]\e[32m Download EXON ...\e[0m";
rm -fr EXON.fa
wget https://hgdownload.soe.ucsc.edu/goldenPath/hg38/multiz7way/alignments/knownCanonical.exonNuc.fa.gz
gunzip knownCanonical.exonNuc.fa.gz;
mv knownCanonical.exonNuc.fa EXON.fa
#
# VDB -------------------------------------------------------------------------
#
echo -e "\e[34m[Downloader]\e[32m Download VDB ...\e[0m";
rm -fr VDB.fa
wget https://github.com/viromelab/HVRS/raw/main/src/VDB.fa.lzma
lzma -k -f -d VDB.fa.lzma
rm VDB.fa.lzma
#
# -----------------------------------------------------------------------------
#

