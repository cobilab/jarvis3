#!/bin/bash
#
FILE_NAME=$1
declare -a ORDER_TOOLS=("AGC" "BSC-m03" "BZIP2" "JARVIS2" "JARVIS3" "LEON" "LZMA" "MFCompress" "NAF" "PAQ8L" "SPRING" );
#
# ==============================================================================
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
CHECK_POS_TOOL () {
  NAME_TOOL=$1
  CORR_POS=0;
  for x in "${ORDER_TOOLS[@]}"
  do    
    [ "$NAME_TOOL" == "$x" ] && { ((++CORR_POS)); echo "$CORR_POS"; break; }
    ((++CORR_POS))
  done
  }
#
# ==============================================================================
#
# Group 1
CHECK_INPUT $FILE_NAME;
cat $FILE_NAME | tr ',' '.' > 2$FILE_NAME
cat 2$FILE_NAME | awk '{ print $1} ' | sort -V | uniq > names.txt
SIZE=`wc -l names.txt | awk '{ print $1;}'`;
CHECK_INPUT "names.txt";
#
plotnames="";
mapfile -t INT_DATA < names.txt;
for dint in "${INT_DATA[@]}"
  do
  #
  idx=$( CHECK_POS_TOOL $dint )  
  grep $dint 2$FILE_NAME > data-$idx.csv
  echo "$(sort -t$'\t' -k 2 -n data-$idx.csv)" > data-$idx.csv
  tmp="'data-$idx.csv' u 2:3 w lp ls $idx title '$dint', ";
  plotnames="$plotnames $tmp";
  #
  done
#
echo "$plotnames";
#
gnuplot << EOF
    reset
    set terminal pdfcairo enhanced color font 'Verdade,12'
    set output "G1-Benchmark-$1-Figure.pdf"
    set style line 101 lc rgb '#000000' lt 1 lw 2
    set border 3 front ls 101
    set tics nomirror out scale 0.75
    set key fixed right top vertical Right noreverse noenhanced autotitle nobox
    set style histogram clustered gap 1 title textcolor lt -1
    set xtics border in scale 0,0 nomirror #rotate by -60  autojustify
    set yrange [:]
    set xrange [:]
    set xtics auto
    set xtics rotate by 45 right
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
    set style line 11 lc rgb '#425152' pt 9 ps 0.6  # circle
    set grid
    set ylabel "Real time (minutes)"
    set xlabel "Compressed data size (bytes)"
    plot $plotnames
EOF
#
#evince G1-Benchmark-$1-Figure.pdf &
#
# ==============================================================================
#
# Group 2
CHECK_INPUT $FILE_NAME;
cat $FILE_NAME | tr ',' '.' > 2$FILE_NAME
cat 2$FILE_NAME | awk '{ print $1} ' | sort -V | uniq > names.txt
SIZE=`wc -l names.txt | awk '{ print $1;}'`;
CHECK_INPUT "names.txt";
#
plotnames="";
mapfile -t INT_DATA < names.txt;
for dint in "${INT_DATA[@]}"
  do
  #
  idx=$( CHECK_POS_TOOL $dint )  
  grep $dint 2$FILE_NAME > data-$idx.csv
  echo "$(sort -t$'\t' -k 2 -n data-$idx.csv)" > data-$idx.csv
  tmp="'data-$idx.csv' u 2:3 w lp ls $idx title '$dint', ";
  plotnames="$plotnames $tmp";
  #
  done
#
echo "$plotnames";
#
gnuplot << EOF
    reset
    set terminal pdfcairo enhanced color font 'Verdade,12'
    set output "G2-Benchmark-$1-Figure.pdf"
    set style line 101 lc rgb '#000000' lt 1 lw 2
    set border 3 front ls 101
    set tics nomirror out scale 0.75
    set key fixed right top vertical Right noreverse noenhanced autotitle nobox
    set style histogram clustered gap 1 title textcolor lt -1
    set xtics border in scale 0,0 nomirror #rotate by -60  autojustify
    set yrange [:]
    set xrange [:]
    set logscale y 2
    set xtics auto
    set xtics rotate by 45 right
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
    set style line 11 lc rgb '#425152' pt 9 ps 0.6  # circle
    set grid
    set ylabel "Real time (minutes)"
    set xlabel "Compressed data size (bytes)"
    plot $plotnames
EOF
#
#evince G2-Benchmark-$1-Figure.pdf &
#
# ==============================================================================
#
# Group 3
CHECK_INPUT $FILE_NAME;
cat $FILE_NAME | tr ',' '.' > 2$FILE_NAME
cat 2$FILE_NAME | awk '{ print $1} ' | sort -V | uniq > names.txt
SIZE=`wc -l names.txt | awk '{ print $1;}'`;
CHECK_INPUT "names.txt";
#
plotnames="";
mapfile -t INT_DATA < names.txt;
for dint in "${INT_DATA[@]}"
  do
  #
  idx=$( CHECK_POS_TOOL $dint )  
  grep $dint 2$FILE_NAME > data-$idx.csv
  echo "$(sort -t$'\t' -k 2 -n data-$idx.csv)" > data-$idx.csv
  tmp="'data-$idx.csv' u 2:3 w lp ls $idx title '$dint', ";
  plotnames="$plotnames $tmp";
  #
  done
#
echo "$plotnames";
#
gnuplot << EOF
    reset
    set terminal pdfcairo enhanced color font 'Verdade,12'
    set output "G3-Benchmark-$1-Figure.pdf"
    set style line 101 lc rgb '#000000' lt 1 lw 2
    set border 3 front ls 101
    set tics nomirror out scale 0.75
    set key fixed right top vertical Right noreverse noenhanced autotitle nobox
    set style histogram clustered gap 1 title textcolor lt -1
    set xtics border in scale 0,0 nomirror #rotate by -60  autojustify
    set yrange [:]
    set xrange [:]
    set xtics auto
    set xtics rotate by 45 right
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
    set style line 11 lc rgb '#425152' pt 9 ps 0.6  # circle
    set grid
    set ylabel "Real time (minutes)"
    set xlabel "Compressed data size (bytes)"
    plot $plotnames
EOF
#
#evince G3-Benchmark-$1-Figure.pdf &
#
# ==============================================================================
#
# Group 4
CHECK_INPUT $FILE_NAME;
cat $FILE_NAME | tr ',' '.' > 2$FILE_NAME
cat 2$FILE_NAME | awk '{ print $1} ' | sort -V | uniq > names.txt
SIZE=`wc -l names.txt | awk '{ print $1;}'`;
CHECK_INPUT "names.txt";
#
plotnames="";
mapfile -t INT_DATA < names.txt;
for dint in "${INT_DATA[@]}"
  do
  #
  idx=$( CHECK_POS_TOOL $dint )  
  grep $dint 2$FILE_NAME > data-$idx.csv
  echo "$(sort -t$'\t' -k 2 -n data-$idx.csv)" > data-$idx.csv
  tmp="'data-$idx.csv' u 2:3 w lp ls $idx title '$dint', ";
  plotnames="$plotnames $tmp";
  #
  done
#
echo "$plotnames";
#
gnuplot << EOF
    reset
    set terminal pdfcairo enhanced color font 'Verdade,12'
    set output "G4-Benchmark-$1-Figure.pdf"
    set style line 101 lc rgb '#000000' lt 1 lw 2
    set border 3 front ls 101
    set tics nomirror out scale 0.75
    set key fixed right top vertical Right noreverse noenhanced autotitle nobox
    set style histogram clustered gap 1 title textcolor lt -1
    set xtics border in scale 0,0 nomirror #rotate by -60  autojustify
    set yrange [:]
    set xrange [:]
    set logscale y 2
    set xtics auto
    set xtics rotate by 45 right
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
    set style line 11 lc rgb '#425152' pt 9 ps 0.6  # circle
    set grid
    set ylabel "Real time (minutes)"
    set xlabel "Compressed data size (bytes)"
    plot $plotnames
EOF
#
#evince G4-Benchmark-$1-Figure.pdf &
#
# =================================================================================
#
# Group 5
#
CHECK_INPUT $FILE_NAME;
cat $FILE_NAME | tr ',' '.' > 2$FILE_NAME
cat 2$FILE_NAME | awk '{ print $1} ' | sort -V | uniq > names.txt
SIZE=`wc -l names.txt | awk '{ print $1;}'`;
CHECK_INPUT "names.txt";
#
plotnames="";
mapfile -t INT_DATA < names.txt;
for dint in "${INT_DATA[@]}"
  do
  #
  idx=$( CHECK_POS_TOOL $dint )  
  grep $dint 2$FILE_NAME > data-$idx.csv
  echo "$(sort -t$'\t' -k 2 -n data-$idx.csv)" > data-$idx.csv
  tmp="'data-$idx.csv' u 2:3 w lp ls $idx title '$dint', ";
  plotnames="$plotnames $tmp";
  #
  done
#
echo "$plotnames";
#
gnuplot << EOF
    reset
    set terminal pdfcairo enhanced color font 'Verdade,12'
    set output "G5-Benchmark-$1-Figure.pdf"
    set style line 101 lc rgb '#000000' lt 1 lw 2
    set border 3 front ls 101
    set tics nomirror out scale 0.75
    set key fixed right top vertical Right noreverse noenhanced autotitle nobox
    set style histogram clustered gap 1 title textcolor lt -1
    set xtics border in scale 0,0 nomirror #rotate by -60  autojustify
    set yrange [:]
    set xrange [:]
    set logscale y 2
    set xtics auto
    set xtics rotate by 45 right
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
    set style line 11 lc rgb '#425152' pt 9 ps 0.6  # circle
    set grid
    set ylabel "Memory used"
    set xlabel "Compressed data size (bytes)"
    plot $plotnames
EOF
#
#evince G5-Benchmark-$1-Figure.pdf &
#
#
# =================================================================================
#
# Group 6
#
CHECK_INPUT $FILE_NAME;
cat $FILE_NAME | tr ',' '.' > 2$FILE_NAME
cat 2$FILE_NAME | awk '{ print $1} ' | sort -V | uniq > names.txt
SIZE=`wc -l names.txt | awk '{ print $1;}'`;
CHECK_INPUT "names.txt";
#
plotnames="";
mapfile -t INT_DATA < names.txt;
for dint in "${INT_DATA[@]}"
  do
  #
  idx=$( CHECK_POS_TOOL $dint )  
  grep $dint 2$FILE_NAME > data-$idx.csv
  echo "$(sort -t$'\t' -k 2 -n data-$idx.csv)" > data-$idx.csv
  tmp="'data-$idx.csv' u 2:3 w lp ls $idx title '$dint', ";
  plotnames="$plotnames $tmp";
  #
  done
#
echo "$plotnames";
#
gnuplot << EOF
    reset
    set terminal pdfcairo enhanced color font 'Verdade,12'
    set output "G6-Benchmark-$1-Figure.pdf"
    set style line 101 lc rgb '#000000' lt 1 lw 2
    set border 3 front ls 101
    set tics nomirror out scale 0.75
    set key fixed right top vertical Right noreverse noenhanced autotitle nobox
    set style histogram clustered gap 1 title textcolor lt -1
    set xtics border in scale 0,0 nomirror #rotate by -60  autojustify
    set yrange [:]
    set xrange [:]
    set logscale y 2
    set xtics auto
    set xtics rotate by 45 right
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
    set style line 11 lc rgb '#425152' pt 9 ps 0.6  # circle
    set grid
    set ylabel "Memory used"
    set xlabel "Compressed data size (bytes)"
    plot $plotnames
EOF
#
#evince G6-Benchmark-$1-Figure.pdf &
#
