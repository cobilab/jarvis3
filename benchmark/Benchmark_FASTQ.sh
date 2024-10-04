#!/bin/bash
#
# ==============================================================================
#
declare -a LEVELS_NAF=("1" "2" "3" "4" "5" "11" "19" "22")
#
CHECK_INPUT () {
  FILE=$1
  if [ -f "$FILE" ];
    then
    echo "Input filename: $FILE"
    else
    echo -e "\e[31mERROR: input file not found ($FILE)!\e[0m";
    exit;
    fi
  }
#
# ==============================================================================
#
function RUN_JARVIS2_SH {
  #
  IN_FILE="$1";
  C_COMMAND="$2";
  D_COMMAND="$3";
  NAME="$4";
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" ./JARVIS2.sh --fastq --level " $C_COMMAND " $6 --input $IN_FILE \
  |& grep "TIME" \
  |& tr '.' ',' \
  |& awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > c_time_mem.txt;
  BYTES=`ls -la $IN_FILE.tar | awk '{ print $5 }'`;
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" ./JARVIS2.sh $D_COMMAND $IN_FILE.tar \
  |& grep "TIME" \
  |& tr '.' ',' \
  |& awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > d_time_mem.txt;
  #
  cmp $IN_FILE.tar.out $IN_FILE > cmp.txt;
  #
  C_TIME=`cat c_time_mem.txt | awk '{ print $1}'`;
  C_MEME=`cat c_time_mem.txt | awk '{ print $2}'`;
  D_TIME=`cat d_time_mem.txt | awk '{ print $1}'`;
  D_MEME=`cat d_time_mem.txt | awk '{ print $2}'`;
  VERIFY="0";
  CMP_SIZE=`ls -la cmp.txt | awk '{ print $5}'`
  if [[ "$CMP_SIZE" != "0" ]]; then CMP_SIZE="1"; fi
  #
  printf "$NAME\t$BYTES\t$C_TIME\t$C_MEME\t$D_TIME\t$D_MEME\t$CMP_SIZE\t$5\t./JARVIS2.sh --fastq --level $C_COMMAND $6 --input $IN_FILE\t./JARVIS2.sh $D_COMMAND $IN_FILE.tar\n";
  #
  }
#
# ==============================================================================
#
function RUN_JARVIS3_SH {
  #
  IN_FILE="$1";
  C_COMMAND="$2";
  D_COMMAND="$3";
  NAME="$4";
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" ./JARVIS3.sh --level " $C_COMMAND " $6 --input $IN_FILE \
  |& grep "TIME" \
  |& tr '.' ',' \
  |& awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > c_time_mem.txt;
  BYTES=`ls -la $IN_FILE.tar | awk '{ print $5 }'`;
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" ./JARVIS3.sh $D_COMMAND $IN_FILE.tar \
  |& grep "TIME" \
  |& tr '.' ',' \
  |& awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > d_time_mem.txt;
  #
  cmp $IN_FILE.tar.out $IN_FILE > cmp.txt;
  #
  C_TIME=`cat c_time_mem.txt | awk '{ print $1}'`;
  C_MEME=`cat c_time_mem.txt | awk '{ print $2}'`;
  D_TIME=`cat d_time_mem.txt | awk '{ print $1}'`;
  D_MEME=`cat d_time_mem.txt | awk '{ print $2}'`;
  VERIFY="0";
  CMP_SIZE=`ls -la cmp.txt | awk '{ print $5}'`
  if [[ "$CMP_SIZE" != "0" ]]; then CMP_SIZE="1"; fi
  #
  printf "$NAME\t$BYTES\t$C_TIME\t$C_MEME\t$D_TIME\t$D_MEME\t$CMP_SIZE\t$5\t./JARVIS3.sh --level $C_COMMAND $6 --input $IN_FILE\t./JARVIS3.sh $D_COMMAND $IN_FILE.tar\n";
  #
  }
#
# ==============================================================================
#
function RUN_MBGC {
  #
  IN_FILE="$1";
  C_COMMAND="$2";
  D_COMMAND="$3";
  NAME="$4";
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" $C_COMMAND -i $IN_FILE MBGC-OUT.mbgc 1> garbage-MBGC 2> mbgc_tmp_report.txt;
  cat mbgc_tmp_report.txt | grep "TIME" | tr '.' ',' | awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > c_time_mem.txt;
  #
  BYTES=`ls -la MBGC-OUT.mbgc | awk '{ print $5 }'`;
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" $D_COMMAND MBGC-OUT.mbgc MBGC-D-OUT 1> garbage-MBGC 2> mbgc_tmp_report.txt
  cat mbgc_tmp_report.txt | grep "TIME" | tr '.' ',' | awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > d_time_mem.txt;
  #
  cmp MBGC-D-OUT/$IN_FILE $IN_FILE > cmp.txt;
  #
  C_TIME=`cat c_time_mem.txt | awk '{ print $1}'`;
  C_MEME=`cat c_time_mem.txt | awk '{ print $2}'`;
  D_TIME=`cat d_time_mem.txt | awk '{ print $1}'`;
  D_MEME=`cat d_time_mem.txt | awk '{ print $2}'`;
  VERIFY="0";
  CMP_SIZE=`ls -la cmp.txt | awk '{ print $5}'`
  if [[ "$CMP_SIZE" != "0" ]]; then CMP_SIZE="1"; fi
  #
  printf "$NAME\t$BYTES\t$C_TIME\t$C_MEME\t$D_TIME\t$D_MEME\t$CMP_SIZE\t$5\n";
  #
  rm -fr MBGC-OUT.mbgc MBGC-D-OUT garbage-MBGC;
  #
  }
#
# ==============================================================================
#
function RUN_NAF {
  #
  rm -fr tmp 
  mkdir -p tmp/
  #
  IN_FILE="$1";
  C_COMMAND="$2";
  D_COMMAND="$3";
  NAME="$4";
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" $C_COMMAND -o NAF-OUT.naf $IN_FILE 2> naf_tmp_report.txt;
  cat naf_tmp_report.txt | grep "TIME" | tr '.' ',' | awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > c_time_mem.txt;
  #
  BYTES=`ls -la NAF-OUT.naf | awk '{ print $5 }'`;
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" $D_COMMAND -o NAD-D-OUT.naf NAF-OUT.naf 2> naf_tmp_report.txt 
  cat naf_tmp_report.txt | grep "TIME" | tr '.' ',' | awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > d_time_mem.txt;
  #
  cmp NAD-D-OUT.naf $IN_FILE > cmp.txt;
  #
  C_TIME=`cat c_time_mem.txt | awk '{ print $1}'`;
  C_MEME=`cat c_time_mem.txt | awk '{ print $2}'`;
  D_TIME=`cat d_time_mem.txt | awk '{ print $1}'`;
  D_MEME=`cat d_time_mem.txt | awk '{ print $2}'`;
  VERIFY="0";
  CMP_SIZE=`ls -la cmp.txt | awk '{ print $5}'`
  if [[ "$CMP_SIZE" != "0" ]]; then CMP_SIZE="1"; fi
  #
  printf "$NAME\t$BYTES\t$C_TIME\t$C_MEME\t$D_TIME\t$D_MEME\t$CMP_SIZE\t$5\t$C_COMMAND -o NAF-OUT.naf $IN_FILE\t$D_COMMAND -o NAD-D-OUT.naf NAF-OUT.naf\n";
  #
  rm -f NAF-OUT.naf NAF-D-OUT.naf;
  #
  }
#
# ==============================================================================
#
function RUN_LZMA {
  #
  FILE="$1";
  C_COMMAND="$2";
  D_COMMAND="$3";
  NAME="$4";
  #
  cp $FILE $FILE.orig
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" $C_COMMAND $FILE.orig 2> c_tmp_report.txt;
  cat c_tmp_report.txt | grep "TIME" | tr '.' ',' | awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > c_time_mem.txt;
  #
  BYTES=`ls -la $FILE.orig.xz | awk '{ print $5 }'`;
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" $D_COMMAND $FILE.orig.xz 2> d_tmp_report.txt
  cat d_tmp_report.txt | grep "TIME" | tr '.' ',' | awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > d_time_mem.txt;
  #
  cmp $FILE $FILE.orig > cmp.txt;
  #
  C_TIME=`cat c_time_mem.txt | awk '{ print $1}'`;
  C_MEME=`cat c_time_mem.txt | awk '{ print $2}'`;
  D_TIME=`cat d_time_mem.txt | awk '{ print $1}'`;
  D_MEME=`cat d_time_mem.txt | awk '{ print $2}'`;
  VERIFY="0";
  CMP_SIZE=`ls -la cmp.txt | awk '{ print $5}'`
  if [[ "$CMP_SIZE" != "0" ]]; then CMP_SIZE="1"; fi
  #
  printf "$NAME\t$BYTES\t$C_TIME\t$C_MEME\t$D_TIME\t$D_MEME\t$CMP_SIZE\t$5\t$C_COMMAND $FILE.orig\t$D_COMMAND $FILE.orig.xz\n";
  #
  rm -f $FILE.orig $FILE.orig.xz c_tmp_report.txt d_tmp_report.txt c_time_mem.txt d_time_mem.txt
  #
  }
#
# ==============================================================================
#
function RUN_BZIP2 {
  #
  FILE="$1";
  C_COMMAND="$2";
  D_COMMAND="$3";
  NAME="$4";
  #
  cp $FILE $FILE.orig
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" $C_COMMAND $FILE.orig 2> c_tmp_report.txt;
  cat c_tmp_report.txt | grep "TIME" | tr '.' ',' | awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > c_time_mem.txt;
  #
  BYTES=`ls -la $FILE.orig.bz2 | awk '{ print $5 }'`;
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" $D_COMMAND $FILE.orig.bz2 2> d_tmp_report.txt
  cat d_tmp_report.txt | grep "TIME" | tr '.' ',' | awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > d_time_mem.txt;
  #
  cmp $FILE $FILE.orig > cmp.txt;
  #
  C_TIME=`cat c_time_mem.txt | awk '{ print $1}'`;
  C_MEME=`cat c_time_mem.txt | awk '{ print $2}'`;
  D_TIME=`cat d_time_mem.txt | awk '{ print $1}'`;
  D_MEME=`cat d_time_mem.txt | awk '{ print $2}'`;
  VERIFY="0";
  CMP_SIZE=`ls -la cmp.txt | awk '{ print $5}'`
  if [[ "$CMP_SIZE" != "0" ]]; then CMP_SIZE="1"; fi
  #
  printf "$NAME\t$BYTES\t$C_TIME\t$C_MEME\t$D_TIME\t$D_MEME\t$CMP_SIZE\t$5\t$C_COMMAND $FILE.orig\t$D_COMMAND $FILE.orig.bz2\n";
  #
  rm -f $FILE.orig $FILE.orig.bz2 c_tmp_report.txt d_tmp_report.txt c_time_mem.txt d_time_mem.txt
  #
  }
#
# ==============================================================================
#
function RUN_BSC {
  #
  FILE="$1";
  C_COMMAND="$2";
  D_COMMAND="$3";
  NAME="$4";
  #
  cp $FILE $FILE.orig
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" ./bsc-m03 e $FILE.orig $FILE.bsc $C_COMMAND 1> c_stdout.txt 2> c_tmp_report.txt;
  cat c_tmp_report.txt | grep "TIME" | tr '.' ',' | awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > c_time_mem.txt;
  #
  BYTES=`ls -la $FILE.bsc | awk '{ print $5 }'`;
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" $D_COMMAND d $FILE.bsc $FILE.out 1> d_stdout.txt 2> d_tmp_report.txt
  cat d_tmp_report.txt | grep "TIME" | tr '.' ',' | awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > d_time_mem.txt;
  #
  cmp $FILE.out $FILE.orig > cmp.txt;
  #
  C_TIME=`cat c_time_mem.txt | awk '{ print $1}'`;
  C_MEME=`cat c_time_mem.txt | awk '{ print $2}'`;
  D_TIME=`cat d_time_mem.txt | awk '{ print $1}'`;
  D_MEME=`cat d_time_mem.txt | awk '{ print $2}'`;
  VERIFY="0";
  CMP_SIZE=`ls -la cmp.txt | awk '{ print $5}'`
  if [[ "$CMP_SIZE" != "0" ]]; then CMP_SIZE="1"; fi
  #
  printf "$NAME\t$BYTES\t$C_TIME\t$C_MEME\t$D_TIME\t$D_MEME\t$CMP_SIZE\t$5\t./bsc-m03 e $FILE.orig $FILE.bsc $C_COMMAND\t$D_COMMAND d $FILE.bsc $FILE.out\n";
  #
  rm -f $FILE.orig $FILE.bsc $FILE.out c_tmp_report.txt d_tmp_report.txt c_time_mem.txt d_time_mem.txt c_stdout.txt d_stdout.txt
  #
  }
#
# ==============================================================================
#
function RUN_GECO3 {
  #
  IN_FILE="$1";
  C_COMMAND="$2";
  D_COMMAND="$3";
  NAME="$4";
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" $C_COMMAND $IN_FILE \
  |& grep "TIME" \
  |& tr '.' ',' \
  |& awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > c_time_mem.txt;
  BYTES=`ls -la $IN_FILE.co | awk '{ print $5 }'`;
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" $D_COMMAND $IN_FILE.co \
  |& grep "TIME" \
  |& tr '.' ',' \
  |& awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > d_time_mem.txt;
  #
  cmp $IN_FILE.co $IN_FILE > cmp.txt;
  #
  C_TIME=`cat c_time_mem.txt | awk '{ print $1}'`;
  C_MEME=`cat c_time_mem.txt | awk '{ print $2}'`;
  D_TIME=`cat d_time_mem.txt | awk '{ print $1}'`;
  D_MEME=`cat d_time_mem.txt | awk '{ print $2}'`;
  VERIFY="0";
  CMP_SIZE=`ls -la cmp.txt | awk '{ print $5}'`
  if [[ "$CMP_SIZE" != "0" ]]; then CMP_SIZE="1"; fi
  #
  printf "$NAME\t$BYTES\t$C_TIME\t$C_MEME\t$D_TIME\t$D_MEME\t$CMP_SIZE\t$5\t$C_COMMAND $IN_FILE\t$D_COMMAND $IN_FILE.co\n";
  #
  }
#
# ==============================================================================
#
function RUN_PAQ8L {
  #
  FILE="$1";
  C_COMMAND="$2";
  D_COMMAND="$3";
  NAME="$4";
  #
  cp $FILE $FILE.orig
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" $C_COMMAND $FILE 1> c_stdout.txt 2> c_tmp_report.txt;
  cat c_tmp_report.txt | grep "TIME" | tr '.' ',' | awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > c_time_mem.txt;
  #
  BYTES=`ls -la $FILE.paq8l | awk '{ print $5 }'`;
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" $D_COMMAND $FILE.paq8l tmp_dir 1> d_stdout.txt 2> d_tmp_report.txt
  mv tmp_dir/$FILE .
  rm -rf tmp_dir 
  mv $FILE.paq8l $FILE.out
  cat d_tmp_report.txt | grep "TIME" | tr '.' ',' | awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > d_time_mem.txt;
  #
  cmp $FILE.out $FILE.orig > cmp.txt;
  #
  C_TIME=`cat c_time_mem.txt | awk '{ print $1}'`;
  C_MEME=`cat c_time_mem.txt | awk '{ print $2}'`;
  D_TIME=`cat d_time_mem.txt | awk '{ print $1}'`;
  D_MEME=`cat d_time_mem.txt | awk '{ print $2}'`;
  VERIFY="0";
  CMP_SIZE=`ls -la cmp.txt | awk '{ print $5}'`
  if [[ "$CMP_SIZE" != "0" ]]; then CMP_SIZE="1"; fi
  #
  printf "$NAME\t$BYTES\t$C_TIME\t$C_MEME\t$D_TIME\t$D_MEME\t$CMP_SIZE\t$5\t$C_COMMAND $FILE\t$D_COMMAND $FILE.paq8l tmp_dir\n";
  #
  rm -f $FILE.orig $FILE.paq8l $FILE.out c_tmp_report.txt d_tmp_report.txt c_time_mem.txt d_time_mem.txt c_stdout.txt d_stdout.txt
  #
  } 
#
# ==============================================================================
#
function RUN_FQZCOMP {
  #
  FILE="$1";
  C_COMMAND="$2";
  D_COMMAND="$3";
  NAME="$4";
  #
  cp $FILE $FILE.orig
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" $C_COMMAND $FILE $FILE.fqzcomp 1> c_stdout.txt 2> c_tmp_report.txt;
  cat c_tmp_report.txt | grep "TIME" | tr '.' ',' | awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > c_time_mem.txt;
  #
  BYTES=`ls -la $FILE.fqzcomp | awk '{ print $5 }'`;
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" $D_COMMAND $FILE.fqzcomp $FILE.out 1> d_stdout.txt 2> d_tmp_report.txt
  cat d_tmp_report.txt | grep "TIME" | tr '.' ',' | awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > d_time_mem.txt;
  #
  cmp $FILE.out $FILE.orig > cmp.txt;
  #
  C_TIME=`cat c_time_mem.txt | awk '{ print $1}'`;
  C_MEME=`cat c_time_mem.txt | awk '{ print $2}'`;
  D_TIME=`cat d_time_mem.txt | awk '{ print $1}'`;
  D_MEME=`cat d_time_mem.txt | awk '{ print $2}'`;
  VERIFY="0";
  CMP_SIZE=`ls -la cmp.txt | awk '{ print $5}'`
  if [[ "$CMP_SIZE" != "0" ]]; then CMP_SIZE="1"; fi
  #
  printf "$NAME\t$BYTES\t$C_TIME\t$C_MEME\t$D_TIME\t$D_MEME\t$CMP_SIZE\t$5\t$C_COMMAND $FILE $FILE.fqzcomp\t$D_COMMAND $FILE.fqzcomp $FILE.out\n";
  #
  rm -f $FILE.fqzcomp $FILE.out c_tmp_report.txt d_tmp_report.txt c_time_mem.txt d_time_mem.txt c_stdout.txt d_stdout.txt
  #
  } 
#
# ==============================================================================
#
function RUN_SPRING {
  #
  FILE="$1";
  C_COMMAND="$2";
  D_COMMAND="$3";
  NAME="$4";
  #
  cp $FILE $FILE.orig
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" $C_COMMAND -i $FILE -o $FILE.spring 1> c_stdout.txt 2> c_tmp_report.txt;
  cat c_tmp_report.txt | grep "TIME" | tr '.' ',' | awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > c_time_mem.txt;
  #
  BYTES=`ls -la $FILE.spring | awk '{ print $5 }'`;
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" $D_COMMAND -i $FILE.spring -o $FILE.out 1> d_stdout.txt 2> d_tmp_report.txt
  cat d_tmp_report.txt | grep "TIME" | tr '.' ',' | awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > d_time_mem.txt;
  #
  cmp $FILE.out $FILE.orig > cmp.txt;
  #
  C_TIME=`cat c_time_mem.txt | awk '{ print $1}'`;
  C_MEME=`cat c_time_mem.txt | awk '{ print $2}'`;
  D_TIME=`cat d_time_mem.txt | awk '{ print $1}'`;
  D_MEME=`cat d_time_mem.txt | awk '{ print $2}'`;
  VERIFY="0";
  CMP_SIZE=`ls -la cmp.txt | awk '{ print $5}'`
  if [[ "$CMP_SIZE" != "0" ]]; then CMP_SIZE="1"; fi
  #
  printf "$NAME\t$BYTES\t$C_TIME\t$C_MEME\t$D_TIME\t$D_MEME\t$CMP_SIZE\t$5\t$C_COMMAND -i $FILE -o $FILE.spring\t$D_COMMAND -i $FILE.spring -o $FILE.out\n";
  #
  rm -f $FILE.spring $FILE.out c_tmp_report.txt d_tmp_report.txt c_time_mem.txt d_time_mem.txt c_stdout.txt d_stdout.txt
  #
  } 
##
function RUN_LEON {
  #
  FILE="$1";
  C_COMMAND="$2";
  D_COMMAND="$3";
  NAME="$4";
  #
  cp $FILE $FILE.orig
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" $C_COMMAND $FILE 1> c_stdout.txt 2> c_tmp_report.txt;
  cat c_tmp_report.txt | grep "TIME" | tr '.' ',' | awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > c_time_mem.txt;
  #
  BYTES=`ls -la $FILE.leon | awk '{ print $5 }'`;
  #
  /bin/time -f "TIME\t%e\tMEM\t%M" $D_COMMAND $FILE.leon 1> d_stdout.txt 2> d_tmp_report.txt
  cat d_tmp_report.txt | grep "TIME" | tr '.' ',' | awk '{ printf $2/60"\t"$4/1024/1024"\n" }' > d_time_mem.txt;
  #
  cmp $FILE.out $FILE.orig > cmp.txt;
  #
  C_TIME=`cat c_time_mem.txt | awk '{ print $1}'`;
  C_MEME=`cat c_time_mem.txt | awk '{ print $2}'`;
  D_TIME=`cat d_time_mem.txt | awk '{ print $1}'`;
  D_MEME=`cat d_time_mem.txt | awk '{ print $2}'`;
  VERIFY="0";
  CMP_SIZE=`ls -la cmp.txt | awk '{ print $5}'`
  if [[ "$CMP_SIZE" != "0" ]]; then CMP_SIZE="1"; fi
  #
  printf "$NAME\t$BYTES\t$C_TIME\t$C_MEME\t$D_TIME\t$D_MEME\t$CMP_SIZE\t$5\t$C_COMMAND $FILE\t$D_COMMAND $FILE.leon\n";
  #
  rm -f $FILE.leon $FILE.out c_tmp_report.txt d_tmp_report.txt c_time_mem.txt d_time_mem.txt c_stdout.txt d_stdout.txt
  #
  }

#
# ==============================================================================
#

FILE="$1";
FILE_NAME="$1";
#
rm -f data.csv
#
# ==============================================================================
#
echo "Running JARVIS2 ...";
RUN_JARVIS2_SH "$FILE" " 1 " " --decompress --threads 1 --fastq --input " "JARVIS2" "23" " --block 600MB --threads 1 --fastq " >> data.csv
echo "Running JARVIS2 ...";
RUN_JARVIS2_SH "$FILE" " 2 " " --decompress --threads 1 --fastq --input " "JARVIS2" "23" " --block 600MB --threads 1 --fastq " >> data.csv
echo "Running JARVIS2 ...";
RUN_JARVIS2_SH "$FILE" " 3 " " --decompress --threads 1 --fastq --input " "JARVIS2" "23" " --block 600MB --threads 1 --fastq " >> data.csv
echo "Running JARVIS2 ...";
RUN_JARVIS2_SH "$FILE" " 7 " " --decompress --threads 1 --fastq --input " "JARVIS2" "23" " --block 600MB --threads 1 --fastq " >> data.csv
echo "Running NAF ...";
for x in "${LEVELS_NAF[@]}"
  do
  echo "Level $x...";
  RUN_NAF "$FILE" "./ennaf --temp-dir tmp/ --fastq --level $x " "./unnaf " "NAF" "24" >> data.csv;
  done
echo "Running LZMA ...";
for((x=1;x<=9;++x)); 
  do
  echo "Level $x...";
  RUN_LZMA "$FILE" "./xz -$x -f -k " "./xz -f -k -d " "LZMA" "25" >> data.csv;
  done
echo "Running Bzip2 ...";
for((x=1;x<=9;++x));
  do
  echo "Level $x...";
  RUN_BZIP2 "$FILE" "./bzip2 -$x -f -k " "./bzip2 -f -k -d " "BZIP2" "26" >> data.csv;
  done  
echo "Running BSC ...";
RUN_BSC "$FILE" " -b800000000 " "./bsc-m03 " "BSC-m03" "27" >> data.csv;
echo "Running BSC ...";
RUN_BSC "$FILE" " -b400000000 " "./bsc-m03 " "BSC-m03" "28" >> data.csv;
echo "Running BSC ...";
RUN_BSC "$FILE" " -b4096000 " "./bsc-m03 " "BSC-m03" "29" >> data.csv;
#echo "Running cmix ...";
#RUN_CMIX "$FILE" "./cmix -c" "./cmix -d" "cmix" "30" >> data.csv;
#
#echo "Running geco3 ...";
#for((x=1;x<=16;++x));
#  do
#  echo "Level $x...";
#  RUN_GECO3 "$FILE" "./GeCo3 -l $x" "./GeDe " "GeCo3" "31" >> data.csv;
#  done
#
echo "Running paq8l ...";
for((x=1;x<=1;++x));
  do
  echo "Level $x...";
  RUN_PAQ8L "$FILE" "./paq8l -$x " "./paq8l -d " "PAQ8L" "32" >> data.csv;
  done
#
#echo "Running fqzcomp ...";
#for((x=1;x<=9;++x));
#  do
#  echo "Level $x...";
#  RUN_FQZCOMP "$FILE" "./fqzcomp -s $x " "./fqzcomp -d " "fqzcomp" "32" >> data.csv;
#  done
#
echo "Running JARVIS3 ...";
for((x=1;x<=26;++x)); 
  do
  echo "Level $x...";
  RUN_JARVIS3_SH "$FILE" " $x --block 600MB --threads 1 " " --decompress --threads 1 --fastq --input " "JARVIS3" "23" " --block 600MB --threads 1 --fastq " >> data.csv;
  done
#
echo "Running SPRING ...";
RUN_SPRING "$FILE" "./spring -c " "./spring -d " "SPRING" "33" >> data.csv;
#
echo "Running LEON ...";
RUN_LEON "$FILE" "./leon -nb-cores 1 -lossless -c -file " "./leon -nb-cores 1 -d -file " "LEON" "34" >> data.csv;
#
printf "PROGRAM\tC_BYTES\tC_TIME (m)\tC_MEM (GB)\tD_TIME (m)\tD_MEM (GB)\tDIFF\tRUN\n";
cat data.csv;
#
# ==============================================================================
#
CHECK_INPUT "data.csv";
cp data.csv data_${FILE_NAME}_FQ.csv
cat data.csv | tr ',' '.' > data2.csv
cat data2.csv | awk '{ print $1} ' | sort -V | uniq > names.txt
SIZE=`wc -l names.txt | awk '{ print $1;}'`;
CHECK_INPUT "names.txt";
#
idx=1;
plotnames="";
mapfile -t INT_DATA < names.txt;
for dint in "${INT_DATA[@]}"
  do
  #
  grep $dint data2.csv > data-$idx.csv
  echo "$(sort -t$'\t' -k 2 -n data-$idx.csv)" > data-$idx.csv
  tmp="'data-$idx.csv' u 2:3 w lp ls $idx title '$dint', ";
  plotnames="$plotnames $tmp";
  ((++idx));
  #
  done
#
echo "$plotnames";
#
gnuplot << EOF
    reset
    set terminal pdfcairo enhanced color font 'Verdade,12'
    set output "Benchmark-$1-Figure.pdf"
    set style line 101 lc rgb '#000000' lt 1 lw 2
    set border 3 front ls 101
    set tics nomirror out scale 0.75
    set key fixed right top vertical Right noreverse noenhanced autotitle nobox
    set style histogram clustered gap 1 title textcolor lt -1
    set xtics border in scale 0,0 nomirror #rotate by -60  autojustify
    set yrange [:]
    set xrange [:]
    set xtics auto
    set ytics auto
    set key top right
    #set style line 4 lc rgb '#CC0000' lt 2 dashtype '---' lw 4 pt 5 ps 0.4 # --- red
    set style line 1 lc rgb '#990099'  pt 1 ps 0.6  # circle
    set style line 2 lc rgb '#004C99'  pt 2 ps 0.6  # circle
    set style line 3 lc rgb '#CCCC00'  pt 3 ps 0.6  # circle
    set style line 4 lc rgb 'red'  pt 7 ps 0.6  # circle
    set style line 5 lc rgb '#009900'  pt 5 ps 0.6  # circle
    set style line 6 lc rgb '#990000'  pt 6 ps 0.6  # circle
    set style line 7 lc rgb '#009999'  pt 4 ps 0.6  # circle
    set style line 8 lc rgb '#19004C'  pt 8 ps 0.6  # circle
    set style line 9 lc rgb '#CC6600'  pt 9 ps 0.6  # circle
    set style line 10 lc rgb '#322152' pt 10 ps 0.6  # circle
    set style line 11 lc rgb '#425152' pt 11 ps 0.6  # circle
    set grid
    set ylabel "Real time (minutes)"
    set xlabel "Compressed data size (bytes)"
    plot $plotnames
EOF
#
evince Benchmark-$1-Figure.pdf &
#
# ==============================================================================
