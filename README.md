<div align="center">

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](LICENSE)
[![Format1](https://img.shields.io/static/v1.svg?label=Format&message=DNA%20Sequence&color=green)](#)
[![Format2](https://img.shields.io/static/v1.svg?label=Format&message=FASTA&color=green)](#)
[![Format3](https://img.shields.io/static/v1.svg?label=Format&message=FASTQ&color=green)](#)
[![MX](https://img.shields.io/static/v1.svg?label=Mixing&message=neural%20network&color=yellow)](#)
<br>
[![HGR](https://img.shields.io/static/v1.svg?label=Human%20genome&message=compression%20record&color=orange)](#)
[![CR](https://img.shields.io/static/v1.svg?label=Cassava%20genome&message=compression%20record&color=orange)](#)

<br>
</div>

<p align="center"><img src="imgs/logo.png" 
alt="JARVIS3" width="300" border="0" /></p>
<p align="center"><b>JARVIS3: an improved encoder for genomic sequences</b></p>

### Installation ###

Manually:
<pre>
git clone https://github.com/cobilab/jarvis3.git
cd jarvis3/src/
make
</pre>

Using Conda:
<pre>
conda install -c bioconda jarvis3
</pre>


### Execution ###

#### Run JARVIS3 ####

Example of running JARVIS3 using level 7:

<pre>
./JARVIS3 -v -l 7 File.seq
</pre>

### Parameters ###

To see the possible options type
<pre>
./JARVIS3 -h
</pre>

This will print the following options:
```
                                                                   
           ██ ███████ ███████ ██    ██ ██ ███████ ███████          
           ██ ██   ██ ██   ██ ██    ██ ██ ██           ██          
           ██ ███████ ██████  ██    ██ ██ ███████ ███████          
      ██   ██ ██   ██ ██  ███  ██  ██  ██      ██      ██          
      ███████ ██   ██ ██   ███  ████   ██ ███████ ███████          
                                                                   
NAME                                                               
      JARVIS3 v3.7,                                              
      Efficient lossless encoding of genomic sequences             
                                                                   
SYNOPSIS                                                           
      ./JARVIS3 [OPTION]... [FILE]                                 
                                                                   
SAMPLE                                                             
      Run Compression   -> ./JARVIS3 -v -l 14 sequence.txt         
      Run Decompression -> ./JARVIS3 -v -d sequence.txt.jc         
                                                                   
DESCRIPTION                                                        
      Lossless compression and decompression of genomic            
      sequences for miniaml storage and analysis purposes.         
      Measure an upper bound of the sequence complexity.           
                                                                   
      -h,  --help                                                  
           Usage guide (help menu).                                
                                                                   
      -a,  --version                                               
           Display program and version information.                
                                                                   
      -x,  --explanation                                           
           Explanation of the context and repeat models.           
                                                                   
      -f,  --force                                                 
           Force mode. Overwrites old files.                       
                                                                   
      -v,  --verbose                                               
           Verbose mode (more information).                        
                                                                   
      -p,  --progress                                              
           Show progress bar.                                      
                                                                   
      -d,  --decompress                                            
           Decompression mode.                                     
                                                                   
      -e,  --estimate                                              
           It creates a file with the extension ".iae" with the  
           respective information content. If the file is FASTA or 
           FASTQ it will only use the "ACGT" (genomic) sequence. 
                                                                   
      -s,  --show-levels                                           
           Show pre-computed compression levels (configured).      
                                                                   
      -l [NUMBER],  --level [NUMBER]                               
           Compression level (integer).                            
           Default level: 7.                                      
           It defines compressibility in balance with computational
           resources (RAM & time). Use -s for levels perception.   
                                                                   
      -sd [NUMBER],  --seed [NUMBER]                               
           Pseudo-random seed.                                     
           Default value: 0.                                      
                                                                   
      -hs [NUMBER],  --hidden-size [NUMBER]                        
           Hidden size of the neural network (integer).            
           Default value: 40.                                      
                                                                   
      -lr [DOUBLE],  --learning-rate [DOUBLE]                      
           Neural Network leaning rate (double).                   
           The 0 value turns the Neural Network off.               
           Default value: 0.03.                                   
                                                                   
      -o [FILENAME], --output [FILENAME]                           
           Compressed/decompressed output filename.                        
                                                                   
      [FILENAME]                                                   
           Input sequence filename (to compress) -- MANDATORY.     
           File to compress is the last argument.                  
                                                                   
COPYRIGHT                                                          
      Copyright (C) 2014-2024.                                     
      This is a Free software, under GPLv3. You may redistribute   
      copies of it under the terms of the GNU - General Public     
      License v3 <http://www.gnu.org/licenses/gpl.html>.                                                                   
 
```


To see the possible levels (automatic choosen compression parameters), type:
<pre>
./JARVIS3 -s
</pre>

This will ouput th following pre-set models for each level:
<pre>
Level 1: -rm 1:12:0.90:4:0.72:0:0.1:1 
Level 2: -rm 1:12:0.90:4:0.72:1:0.1:1 
Level 3: -rm 1:13:0.90:4:0.72:1:0.1:1 
Level 4: -rm 1:14:0.90:4:0.72:1:0.1:1 
Level 5: -rm 2:12:0.90:5:0.60:1:0.1:1 
Level 6: -rm 4:12:0.94:7:0.70:1:0.05:3 
Level 7: -rm 3:13:0.90:5:0.72:1:0.1:1 
Level 8: -rm 3:14:0.90:5:0.72:1:0.1:1 
Level 9: -rm 5:14:0.90:5:0.72:1:0.1:1 
Level 10: -rm 6:12:0.90:6:0.78:1:0.03:1 
Level 11: -rm 8:13:0.90:6:0.78:1:0.03:2 
Level 12: -rm 10:12:0.91:7:0.80:1:0.02:3 
Level 13: -rm 12:12:0.90:7:0.81:1:0.02:3 
Level 14: -lr 0 -cm 1:1:0:0.9/0:0:0:0 -rm 2:12:0.92:7:0.80:1:0.05:2 
Level 15: -lr 0 -cm 3:1:0:0.9/0:0:0:0 -rm 3:12:0.93:7:0.81:1:0.05:3 
Level 16: -lr 0 -cm 3:1:0:0.9/0:0:0:0 -rm 4:12:0.92:7:0.81:1:0.03:2 
Level 17: -lr 0 -cm 4:1:0:0.9/0:0:0:0 -rm 4:13:0.94:7:0.81:1:0.04:3 
Level 18: -lr 0 -cm 6:1:0:0.9/0:0:0:0 -rm 4:13:0.94:7:0.81:1:0.04:3 
Level 19: -lr 0 -cm 6:1:0:0.9/0:0:0:0 -rm 8:12:0.93:7:0.81:1:0.02:3 
Level 20: -lr 0 -cm 4:1:0:0.9/0:0:0:0 -rm 20:12:0.9:7:0.85:1:0.01:4 
Level 21: -lr 0 -cm 4:1:0:0.9/0:0:0:0 -rm 50:12:0.9:7:0.85:1:0.01:5 
Level 22: -lr 0 -cm 4:1:0:0.9/0:0:0:0 -rm 100:12:0.9:7:0.85:1:0.01:5 
Level 23: -lr 0 -cm 4:1:0:0.9/0:0:0:0 -rm 200:12:0.9:7:0.85:1:0.01:6 
Level 24: -lr 0 -cm 6:1:0:0.9/0:0:0:0 -rm 6:15:0.93:6:0.81:1:0.02:1 
Level 25: -lr 0.03 -hs 24 -cm 6:1:0:0.9/0:0:0:0 -rm 6:15:0.92:6:0.81:1:0.02:1 
Level 26: -lr 0.03 -hs 32 -cm 4:1:0:0.9/0:0:0:0 -rm 20:15:0.90:7:0.82:1:0.02:1 
Level 27: -lr 0.03 -hs 24 -cm 6:1:0:0.9/0:0:0:0 -rm 15:13:0.92:7:0.85:0:0.02:4 -rm 13:12:0.92:7:0.84:2:0.01:3 
Level 28: -lr 0.03 -hs 42 -cm 6:1:0:0.9/0:0:0:0 -rm 6:15:0.93:6:0.81:1:0.02:1 
Level 29: -lr 0.03 -hs 42 -cm 6:1:0:0.9/0:0:0:0 -rm 10:15:0.93:6:0.81:1:0.02:1 
Level 30: -lr 0.03 -hs 42 -cm 6:1:0:0.9/0:0:0:0 -rm 10:15:0.93:6:0.81:0:0.02:1 -rm 10:15:0.93:6:0.81:2:0.02:1 
Level 31: -lr 0.03 -hs 48 -cm 1:1:0:0.9/0:0:0:0 -cm 4:1:0:0.9/0:0:0:0 -cm 8:1:1:0.89/0:0:0:0 -cm 12:20:1:0.97/0:0:0:0 -rm 300:12:0.9:7:0.85:0:0.01:10 -rm 200:12:0.9:7:0.8:2:0.01:4 
Level 32: -lr 0.04 -hs 64 -cm 1:1:0:0.9/0:0:0:0 -cm 4:1:0:0.9/0:0:0:0 -cm 8:1:1:0.89/0:0:0:0 -cm 12:20:1:0.97/0:0:0:0 -rm 500:12:0.9:7:0.85:0:0.01:12 -rm 200:12:0.9:7:0.8:2:0.01:4 
Level 33: -lr 0.04 -hs 86 -cm 1:1:0:0.9/0:0:0:0 -cm 4:1:0:0.9/0:0:0:0 -cm 8:1:1:0.89/0:0:0:0 -cm 12:20:1:0.97/0:0:0:0 -rm 500:12:0.9:7:0.85:0:0.01:12 -rm 200:12:0.9:7:0.8:2:0.01:4 
Level 34: -lr 0.04 -hs 256 -cm 1:1:0:0.9/0:0:0:0 -cm 4:1:0:0.9/0:0:0:0 -cm 8:1:1:0.9/0:0:0:0 -cm 12:20:1:0.97/0:0:0:0 -rm 1500:12:0.9:7:0.85:0:0.01:10 -rm 500:12:0.9:7:0.82:2:0.01:3 
Level 35: -lr 0.04 -hs 248 -cm 1:1:0:0.9/0:0:0:0 -cm 3:1:0:0.9/0:0:0:0 -cm 7:1:0:0.9/0:0:0:0 -cm 9:1:1:0.9/0:0:0:0 -cm 11:10:0:0.9/0:0:0:0 -rm 100:14:0.9:7:0.85:1:0.01:3 -rm 200:12:0.88:7:0.85:0:0.01:3 -rm 300:12:0.87:7:0.85:2:0.01:3 
Level 36: -lr 0.04 -hs 248 -cm 1:1:0:0.9/0:0:0:0 -cm 3:1:0:0.9/0:0:0:0 -cm 7:1:0:0.9/0:0:0:0 -cm 9:1:1:0.9/0:0:0:0 -cm 11:10:0:0.9/0:0:0:0 -cm 13:200:1:0.9/1:10:1:0.9 -rm 100:14:0.9:7:0.85:1:0.01:3 -rm 200:12:0.88:7:0.85:0:0.01:8 -rm 300:12:0.87:7:0.85:2:0.01:3 
Level 37: -lr 0.01 -hs 248 -cm 1:1:0:0.9/0:0:0:0 -cm 3:1:0:0.9/0:0:0:0 -cm 6:1:0:0.9/0:0:0:0 -cm 9:1:0:0.9/0:0:0:0 -cm 11:10:1:0.9/0:0:0:0 -cm 14:200:1:0.9/1:10:1:0.9 -rm 300:14:0.88:7:0.85:0:0.01:8 -rm 300:14:0.88:7:0.85:2:0.01:8 -rm 500:12:0.88:7:0.85:0:0.01:15 
Level 38: -lr 0 -cm 12:1:0:0.7/0:0:0:0 -rm 2:14:0.95:1:0.9:1:0.1:1 
Level 39: -lr 0 -cm 12:1:0:0.7/0:0:0:0 -rm 3:14:0.95:1:0.9:1:0.1:1 
Level 40: -lr 0.03 -lr 32 -cm 12:1:0:0.7/0:0:0:0 -rm 4:14:0.95:1:0.9:1:0.1:1
</pre>

To see the meaning of the model parameters, type:
<pre>
./JARVIS3 -x
</pre>
This will output the following content:
<pre>
      -cm [NB_C]:[NB_D]:[NB_I]:[NB_G]/[NB_S]:[NB_E]:[NB_R]:[NB_A]  
      Template of a context model.                                 
      Parameters:                                                  
      [NB_C]: (integer [1;14]) order size of the regular context   
              model. Higher values use more RAM but, usually, are  
              related to a better compression score.               
      [NB_D]: (integer [1;5000]) denominator to build alpha, which 
              is a parameter estimator. Alpha is given by 1/[NB_D].
              Higher values are usually used with higher [NB_C],   
              and related to confident bets. When [NB_D] is one,   
              the probabilities assume a Laplacian distribution.   
      [NB_I]: (integer {0,1,2}) number to define if a sub-program  
              which addresses the specific properties of DNA       
              sequences (Inverted repeats) is used or not. The     
              number 1 turns ON the sub-program using at the same  
              time the regular context model. The number 2 does    
              only contemple the inversions only (NO regular). The 
              number 0 does not contemple its use (Inverted repeats
              OFF). The use of this sub-program increases the      
              necessary time to compress but it does not affect the
              RAM.                                                 
      [NB_G]: (real [0;1)) real number to define gamma. This value 
              represents the decayment forgetting factor of the    
              regular context model in definition.                 
      [NB_S]: (integer [0;20]) maximum number of editions allowed  
              to use a substitutional tolerant model with the same 
              memory model of the regular context model with       
              order size equal to [NB_C]. The value 0 stands for   
              turning the tolerant context model off. When the     
              model is on, it pauses when the number of editions   
              is higher that [NB_C], while it is turned on when    
              a complete match of size [NB_C] is seen again. This  
              is probabilistic-algorithmic model very useful to    
              handle the high substitutional nature of genomic     
              sequences. When [NB_S] > 0, the compressor used more 
              processing time, but uses the same RAM and, usually, 
              achieves a substantial higher compression ratio. The 
              impact of this model is usually only noticed for     
              higher [NB_C].                                       
      [NB_R]: (integer {0,1}) number to define if a sub-program    
              which addresses the specific properties of DNA       
              sequences (Inverted repeats) is used or not. It is   
              similar to the [NR_I] but for tolerant models.       
      [NB_E]: (integer [1;5000]) denominator to build alpha for    
              substitutional tolerant context model. It is         
              analogous to [NB_D], however to be only used in the  
              probabilistic model for computing the statistics of  
              the substitutional tolerant context model.           
      [NB_A]: (real [0;1)) real number to define gamma. This value 
              represents the decayment forgetting factor of the    
              substitutional tolerant context model in definition. 
              Its definition and use is analogus to [NB_G].        
                                                                   
      ... (you may use several context models)                     
                                                                   
                                                                   
      -rm [NB_R]:[NB_C]:[NB_B]:[NB_L]:[NB_G]:[NB_I]:[NB_W]:[NB_Y]  
      Template of a repeat model.                                  
      Parameters:                                                  
      [NB_R]: (integer [1;10000] maximum number of repeat models   
              for the class. On very repetive sequences the RAM    
              increases along with this value, however it also     
              improves the compression capability.                 
      [NB_C]: (integer [1;14]) order size of the repeat context    
              model. Higher values use more RAM but, usually, are  
              related to a better compression score.               
      [NB_B]: (real (0;1]) beta is a real value, which is a        
              parameter for discarding or maintaining a certain    
              repeat model.                                        
      [NB_L]: (integer (1;20]) a limit threshold to play with      
              [NB_B]. It accepts or not a certain repeat model.    
      [NB_G]: (real [0;1)) real number to define gamma. This value 
              represents the decayment forgetting factor of the    
              regular context model in definition.                 
      [NB_I]: (integer {0,1,2}) number to define if a sub-program  
              which addresses the specific properties of DNA       
              sequences (Inverted repeats) is used or not. The     
              number 1 turns ON the sub-program using at the same  
              time the regular context model. The number 0 does    
              not contemple its use (Inverted repeats OFF). The    
              number 2 uses exclusively Inverted repeats. The      
              use of this sub-program increases the necessary time 
              to compress but it does not affect the RAM.          
      [NB_W]: (real (0;1)) initial weight for the repeat class.    
      [NB_Y]: (integer {0}, [1;50]) maximum cache size. This will  
              use a table cache with the specified size. The size  
              must be in balance with the k-mer size [NB_C].  

</pre>


#### Compression and decompression of FASTA and FASTQ data ####

First, make sure to give permissions to the script by typing the following at the src/ folder
<pre>
chmod +x JARVIS3.sh
</pre>

The extension of compressing FASTA and FASTQ data contains a menu to expose the parameters that can be accessed using:
<pre>
./JARVIS3.sh --help
</pre>

This will ouput the following menu
<pre>
 -------------------------------------------------------
                                                        
 JARVIS3, v3.7. High reference-free compression of DNA  
                sequences, FASTA data, and FASTQ data.  
                                                        
 Program options ---------------------------------------
                                                        
 -h, --help                   Show this,                
 -a, --about                  Show program information, 
 -c, --install                Install/compile programs, 
 -s, --show                   Show compression levels,  
                                                        
 -l <INT>, --level <INT>      JARVIS3 compression level,
 -b <INT>, --block <INT>      Block size to be splitted,
 -t <INT>, --threads <INT>    Number of JARVIS3 threads,
                                                        
 -dn, --dna                   Assume DNA sequence type, 
 -fa, --fasta                 Assume FASTA data type,   
 -fq, --fastq                 Assume FASTQ data type,   
 -au, --automatic             Detect data type (def),   
                                                        
 -d, --decompress             Decompression mode,       
                                                        
 Input options -----------------------------------------
                                                        
 -i <FILE>, --input <FILE>    Input DNA filename.       
                                                        
 Example -----------------------------------------------
                                                        
 ./JARVIS3.sh --block 16MB -t 8 -i sample.seq           
 ./JARVIS3.sh --decompress -t 4 -i sample.seq.tar       
                                                        
 -------------------------------------------------------
</pre>

Preparing JARVIS3 for FASTA and FASTQ:
<pre>
./JARVIS3.sh --install
</pre>

Compression of FASTA data:
<pre>
./JARVIS3.sh --threads 8 --fasta --block 10MB --input sample.fa
</pre>

Decompression of FASTA data:
<pre>
./JARVIS3.sh --decompress --fasta --threads 4 --input sample.fa.tar
</pre>

Compression of FASTQ data:
<pre>
./JARVIS3.sh --threads 8 --fastq --block 40MB --input sample.fq
</pre>

Decompression of FASTQ data:
<pre>
./JARVIS3.sh --decompress --fastq --threads 4 --input sample.fq.tar
</pre>

### Citation ###

<pre>
In progress...
</pre>

#### Issues ###

For any issue let us know at [issues link](https://github.com/cobilab/JARVIS3/issues).

### License ###

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](LICENSE)

For more information:
<pre>http://www.gnu.org/licenses/gpl-3.0.html</pre>

