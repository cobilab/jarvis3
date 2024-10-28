#!/bin/bash
#
HELP=0;
ABOUT=0;
DECOMPRESS=0;
INSTALL=0;
SHOW=0;
TYPE=0;
LEVEL=9;
BLOCK="250MB";
THREADS="8";
OUTPUT="";
#
################################################################################
#
SHOW_MENU () {
  echo " -------------------------------------------------------";
  echo "                                                        ";
  echo " JARVIS3, v3.7. High reference-free compression of DNA  ";
  echo "                sequences, FASTA data, and FASTQ data.  ";
  echo "                                                        ";
  echo " Program options ---------------------------------------";
  echo "                                                        ";
  echo " -h, --help                   Show this,                ";
  echo " -a, --about                  Show program information, ";
  echo " -c, --install                Install/compile programs, ";
  echo " -s, --show                   Show compression levels,  ";
  echo "                                                        ";
  echo " -l <INT>, --level <INT>      JARVIS3 compression level,";
  echo " -b <INT>, --block <INT>      Block size to be splitted,";
  echo " -t <INT>, --threads <INT>    Number of JARVIS3 threads,";
  echo " -o <INT>, --output <STR>     Output filename,          ";  
  echo "                                                        ";
  echo " -dn, --dna                   Assume DNA sequence type, ";
  echo " -fa, --fasta                 Assume FASTA data type,   ";
  echo " -fq, --fastq                 Assume FASTQ data type,   ";
  echo " -au, --automatic             Detect data type (def),   ";
  echo "                                                        ";
  echo " -d, --decompress             Decompression mode,       ";
  echo "                                                        ";
  echo " Input options -----------------------------------------";
  echo "                                                        ";
  echo " -i <FILE>, --input <FILE>    Input DNA filename.       ";
  echo "                                                        ";
  echo " Example -----------------------------------------------";
  echo "                                                        ";
  echo " ./JARVIS3.sh --block 16MB -t 8 -i sample.seq           ";
  echo " ./JARVIS3.sh --decompress -t 4 -i sample.seq.tar       ";
  echo "                                                        ";
  echo " -------------------------------------------------------";
  }
#
################################################################################
#
CHECK_INPUT () {
  FILE=$1
  if [ -f "$FILE" ];
    then
    echo "Input filename: $FILE"
    else
    echo -e "\e[31mERROR: input file not found!\e[0m";
    SHOW_MENU;
    exit;
    fi
  }
#
################################################################################
#
Program_installed () {
  if ! [ -x "$(command -v $1)" ];
    then
    echo -e "\e[41mERROR\e[49m: $1 is not installed." >&2;
    echo -e "\e[42mTIP\e[49m: Try: ./JARVIS3.sh --install" >&2;
    exit 1;
    fi
  }
#
################################################################################
#
SHOW_HEADER () {
  echo "                                                        ";
  echo " [JARVIS3 :: DNA, FASTA, and FASTQ Extensions]          ";
  echo "                                                        ";
  echo " Release year: 2024,                                    ";
  echo " Version: 3.7                                           ";
  echo " Author: D. Pratas                                      ";
  echo " Language: Bash / C                                     ";
  echo " License: GPL v3                                        ";
  echo "                                                        ";
  }
#
################################################################################
#
SHOW_LEVELS () {
  ./JARVIS3 -s
  }
#
################################################################################
#
MERGE_DNA () {
  INPUT="$1";    # "Filename"
  THREADS="$2";  # "8"
  #
  tar -xvf $INPUT 1> .DEC_F_JV3;
  #
  mapfile -t FILES < .DEC_F_JV3;
  #
  NAMES="";
  IDX_T=1;
  IDX_POS=1;
  #
  for file in "${FILES[@]}" #
    do
    ./JARVIS3 -d $file 2> .tmp_report_$file &
    if [[ "$IDX_T" -eq "$THREADS" ]] || [[ "${#FILES[@]}" -eq "$IDX_POS" ]]
      then
      wait;
      IDX_T=0;
      fi
    ((++IDX_T));
    ((++IDX_POS));
    done
  #
  rm -f $INPUT.out;
  for file in "${FILES[@]}" #
    do
    cat $file.jd >> $INPUT.out;
    rm -f $file.jd $file .tmp_report_$file
    done
  #
  rm -f .DEC_F_JV3
  }
#
################################################################################
#
SPLIT_DNA () {
  #
  INPUT="$1";
  SIZE="$2";
  THREADS="$3";
  LEVEL="$4";
  #
  split --bytes=$SIZE --verbose $INPUT $INPUT- \
  | tr -d "\'" | awk '{ print $3;}' > .ENC_F_JV3 2> .tmp_report;
  #
  mapfile -t FILES < .ENC_F_JV3;
  #
  C_NAMES="";
  IDX_T=1;
  IDX_POS=1;
  #
  for file in "${FILES[@]}" #
    do
    ./JARVIS3 -l $LEVEL $file 2> .tmp_report_$file &
    if [[ "$IDX_T" -eq "$THREADS" ]] || [[ "${#FILES[@]}" -eq "$IDX_POS" ]]
      then
      wait;
      IDX_T=0;
      fi
    C_NAMES+=" $file.jc ";
    ((++IDX_T));
    ((++IDX_POS));
    done
  #
  tar -cvf $INPUT.tar $C_NAMES 1>> .tmp_report 2>> .tmp_err;
  for file in "${FILES[@]}" #
    do
    rm -f $file.jc $file .tmp_report_$file;
    done
  #
  rm -f .ENC_F_JV3 .tmp_report .tmp_err;
  }	
#
################################################################################
#
if [[ "$#" -lt 1 ]];
  then
  HELP=1;
  fi
#
POSITIONAL=();
#
while [[ $# -gt 0 ]]
  do
  i="$1";
  case $i in
    -h|--help|?)
      HELP=1;
      shift
    ;;
    -a|--about|--version)
      ABOUT=1;
      shift
    ;;
    -c|--install|--compile)
      INSTALL=1;
      shift
    ;;
    -d|--decompress|--uncompress|--decompression)
      DECOMPRESS=1;
      shift
    ;;
    -l|--level|--compression-level|--JARVIS3-level|--mode)
      LEVEL="$2";
      shift 2;
    ;;
    -b|--block)
      BLOCK="$2";
      shift 2;
    ;;
    -dn|--dna|--adn|--DNA|--genomic)
      TYPE=1;
      shift 1;
    ;;
    -fa|--fas|--fasta|--FASTA|--Fasta)
      TYPE=2;
      shift 1;
    ;;
    -fq|--faq|--fastq|--FASTQ|--Fastq)
      TYPE=3;
      shift 1;
    ;;
    -au|--automatic|--automat|--auto)
      TYPE=0;
      shift 1;
    ;;
    -t|--threads)
      THREADS="$2";
      shift 2;
    ;;
    -s|--show)
      SHOW=1;
      shift;
    ;;
    -i|--input)
      INPUT="$2";
      shift 2;
    ;;
    -o|--output)
      OUTPUT="$2";
      shift 2;
    ;;
    -*) # unknown option with small
    echo "Invalid arg ($1)!";
    echo "For help, try: ./JARVIS3.sh -h"
    exit 1;
    ;;
  esac
  done
#
set -- "${POSITIONAL[@]}" # restore positional parameters
#
################################################################################
#
if [[ "$HELP" -eq "1" ]];
  then
  SHOW_MENU;
  exit;
  fi
#
################################################################################
#
if [[ "$SHOW" -eq "1" ]];
  then
  SHOW_LEVELS;
  exit;
  fi
#
################################################################################
#
if [[ "$ABOUT" -eq "1" ]];
  then
  SHOW_HEADER;
  exit;
  fi
#
################################################################################
#
if [[ "$INSTALL" -eq "1" ]];
  then
  echo "Running installation ...";
  cd extra/
  make clean ; make ;
  cp MergeFastaStreamsJ3 ..
  cp MergeFastqStreamsJ3 ..
  cp SplitFastaStreamsJ3 ..
  cp SplitFastqStreamsJ3 ..
  cp XScoreC ..
  cp XScoreD ..
  gcc bzip2.c -o bzip2
  cp bzip2 ..
  cd ../
  make
  echo "Done!"; 
  exit;
  fi
#
################################################################################
#
if [[ "$DECOMPRESS" -eq "0" ]];
  then
  # COMPRESSION
  #
  CHECK_INPUT "$INPUT";
  #
  Program_installed "./JARVIS3";
  #
  if [[ "$TYPE" -eq "0" ]];
    then
    FIRST=`head -c 1 < $INPUT`;
    if [[ "$FIRST" = ">" ]];
      then 
      TYPE=2;
      elif [[ "$FIRST" = "@" ]];
      then
      TYPE=3;
      else
      TYPE=1;
      fi
    fi
  #
  declare -a TYPE_NAME=("other" "DNA sequence" "FASTA" "FASTQ");  
  #
  echo "Running data type: ${TYPE_NAME[$TYPE]}";
  echo "Maximum Blocksize: $BLOCK";
  echo "Number of threads: $THREADS";
  echo "Compression level: $LEVEL";
  echo "Compressing data ...";
  #
  if [[ "$TYPE" -eq "1" ]]; # DNA SEQUENCE =====================================
    then
    #
    SPLIT_DNA "$INPUT" "$BLOCK" "$THREADS" "$LEVEL"
    wait
    echo "Done!";
    ls -lah $INPUT.tar | awk '{ print "TOTAL:\t"$5; }'
    rm -f DNA.JV3.tar .rep_out_enc
    echo "Compressed file: $INPUT.tar";
    #
    elif [[ "$TYPE" -eq "2" ]]; # FASTA DATA ===================================
    then
    #
    Program_installed "./SplitFastaStreamsJ3";
    Program_installed "./bzip2";
    Program_installed "./JARVIS3";
    ./SplitFastaStreamsJ3 < $INPUT
    SPLIT_DNA "DNA.JV3" "$BLOCK" "$THREADS" "$LEVEL" &
    ./bzip2 -f HEADERS.JV3 &
    ./bzip2 -f EXTRA.JV3 &
    wait
    tar -cvf $INPUT.tar DNA.JV3.tar EXTRA.JV3.bz2 HEADERS.JV3.bz2 1> .rep_out_ec
    echo "Done!";
    ls -la HEADERS.JV3.bz2 | awk '{ print "HEADS:\t"$5; }'
    ls -la DNA.JV3.tar | awk '{ print "DNA:\t"$5; }'
    ls -la EXTRA.JV3.bz2 | awk '{ print "EXTRA:\t"$5; }'
    echo "------";
    ls -la $INPUT.tar | awk '{ print "TOTAL:\t"$5; }'
    rm -f DNA.JV3.tar EXTRA.JV3 HEADERS.JV3 .rep_out_ec
    if [ -n "$OUTPUT" ]; then
      mv $INPUT.tar $OUTPUT.tar
      echo "Compressed file: $OUTPUT.tar";
    else
      echo "Compressed file: $INPUT.tar";
    fi
    #
    else # FASTQ DATA ==========================================================
    #
    Program_installed "./SplitFastqStreamsJ3";
    Program_installed "./bzip2";
    Program_installed "./XScoreC";
    Program_installed "./JARVIS3";
    ./SplitFastqStreamsJ3 < $INPUT
    SPLIT_DNA "DNA.JV3" "$BLOCK" "$THREADS" "$LEVEL" &
    ./bzip2 -f N.JV3 &
    ./bzip2 -f HEADERS.JV3 &
    ./XScoreC -l 1 QUALITIES.JV3 1> .tmp_report_out_xc 2> .tmp_report_err_xc &
    wait
    tar -cvf $INPUT.tar DNA.JV3.tar N.JV3.bz2 HEADERS.JV3.bz2 QUALITIES.JV3.co 1> .rep_main_info;
    echo "Done!";
    ls -la HEADERS.JV3.bz2 | awk '{ print "HEADS:\t"$5; }'
    ls -la DNA.JV3.tar | awk '{ print "DNA:\t"$5; }'
    ls -la N.JV3.bz2 | awk '{ print "Ns:\t"$5; }'
    ls -la QUALITIES.JV3.co | awk '{ print "QUALS:\t"$5; }'
    echo "------";
    ls -lah $INPUT.tar | awk '{ print "TOTAL:\t"$5; }'
    rm -f DNA.JV3 N.JV3 HEADERS.JV3 QUALITIES.JV3 .rep_main_info \
    .tmp_report_out_xc .tmp_report_err_xc;
    if [ -n "$OUTPUT" ]; then
      mv $INPUT.tar $OUTPUT.tar
      echo "Compressed file: $OUTPUT.tar";
    else
      echo "Compressed file: $INPUT.tar";
    fi
    #	    
    fi
  #
  else 
  # DECOMPRESSION:
  # 
  # Make sure file exits else die
  CHECK_INPUT "$INPUT";
  #
  if [[ "$TYPE" -eq "1" ]]; # DNA SEQUENCE =====================================
    then
    Program_installed "./JARVIS3";
    #
    echo "Decompressing data ...";
    MERGE_DNA "$INPUT" "$THREADS"
    wait
    echo "Done!";
    echo "Decompressed file: $INPUT.out";
    #
    elif [[ "$TYPE" -eq "2" ]]; # FASTA DATA ===================================
    then
    #
    echo "Number of threads: $THREADS";
    Program_installed "./MergeFastaStreamsJ3";
    Program_installed "./bzip2";
    Program_installed "./JARVIS3";
    echo "Decompressing data ...";
    tar -xvf $INPUT 1> .rep_out_dec
    MERGE_DNA "DNA.JV3.tar" "$THREADS" &
    ./bzip2 -d -f EXTRA.JV3.bz2 &
    ./bzip2 -d -f HEADERS.JV3.bz2 &
    wait
    mv DNA.JV3.tar.out DNA.JV3
    ./MergeFastaStreamsJ3 > $INPUT.out
    rm -f DNA.JV3.jc DNA.JV3.tar.out EXTRA.JV3.bz2 HEADERS.JV3.bz2 .rep_out_dec
    echo "Done!";
    if [ -n "$OUTPUT" ]; then
      mv $INPUT.out $OUTPUT.out
      echo "Decompressed file: $OUTPUT.out";
    else
      echo "Decompressed file: $INPUT.out";
    fi
    #
    else # FASTQ DATA ==========================================================
    #
    echo "Number of threads: $THREADS";
    Program_installed "./MergeFastqStreamsJ3";
    Program_installed "./bzip2";
    Program_installed "./XScoreD";
    Program_installed "./JARVIS3";
    echo "Decompressing data ...";
    tar -xvf $INPUT 1> .rep_main_info;
    ./bzip2 -d -f HEADERS.JV3.bz2 &
    MERGE_DNA "DNA.JV3.tar" "$THREADS" &
    ./bzip2 -d -f N.JV3.bz2 &
    ./XScoreD -d QUALITIES.JV3.co 1> .tmp_report_out_xd 2> .tmp_report_err_xd &
    wait
    mv DNA.JV3.tar.out DNA.JV3
    mv QUALITIES.JV3.de QUALITIES.JV3
    # XXX MERGE HAS ISSUES RELATED W READS LONGER THAN 1,000,000. TODO: FIX IT.
    ./MergeFastqStreamsJ3 > $INPUT.out
    rm -f DNA.JV3.jc DNA.JV3.tar.out N.JV3.bz2 HEADERS.JV3.bz2 \
    QUALITIES.JV3.co .rep_main_info .tmp_report_out_xd .tmp_report_err_xd ;
    if [ -n "$OUTPUT" ]; then
      mv $INPUT.out $OUTPUT.out
      echo "Decompressed file: $OUTPUT.out";
    else
      echo "Decompressed file: $INPUT.out";
    fi
    #
    fi
  fi
#
################################################################################
#
