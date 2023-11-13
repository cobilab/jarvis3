#!/bin/bash
#
# ------------------------------------------------------------------------------
#
PROGRAM_EXISTS () {
  printf "Checking $1 ... ";
  if ! [ -x "$(command -v $1)" ];
    then
    echo -e "\e[41mERROR\e[49m: $1 is not installed." >&2;
    exit 1;
    else
    echo -e "\e[42mSUCCESS!\e[49m";
    fi
  }
#
# ==============================================================================
#                        A U X I L I A R Y   T O O L S
# ------------------------------------------------------------------------------
#
# GTO TOOLKIT ------------------------------------------------------------------
#
echo -e "\e[34m[MizaR]\e[32m Installing GTO ...\e[0m";
rm -fr v1.6.3.zip gto-1.6.3
wget https://github.com/cobilab/gto/archive/refs/tags/v1.6.3.zip
unzip v1.6.3.zip
cd gto-1.6.3/src/
make
cd ../bin/
cp gto_* ../../
cd ../../
#
# ==============================================================================
#                S P E C I F I C - P U R P O S E   E N C O D E R S
# ------------------------------------------------------------------------------
#
# MFC --------------------------------------------------------------------------
#
echo -e "\e[34m[MizaR]\e[32m Installing MFCompress ...\e[0m";
rm MFCompress-linux64-1.01.tgz MFCompress-linux64-1.01/ -fr
wget http://sweet.ua.pt/ap/software/mfcompress/MFCompress-linux64-1.01.tgz
tar -xvzf MFCompress-linux64-1.01.tgz
cp MFCompress-linux64-1.01/MFCompressC .
cp MFCompress-linux64-1.01/MFCompressD .
#
# JARVIS2 ----------------------------------------------------------------------
#
echo -e "\e[34m[MizaR]\e[32m Installing JARVIS2 ...\e[0m";
rm -rf JARVIS2-bin-64-Linux.zip extra JARVIS2.sh JARVIS2-bin-64-Linux/
wget https://github.com/cobioders/HumanGenome/raw/main/bin/JARVIS2-bin-64-Linux.zip
unzip -o JARVIS2-bin-64-Linux.zip
cp JARVIS2-bin-64-Linux/extra/* .
cp JARVIS2-bin-64-Linux/JARVIS2.sh .
cp JARVIS2-bin-64-Linux/JARVIS2 .
rm -rf JARVIS2-bin-64-Linux.zip JARVIS2-bin-64-Linux/ 
#
# JARVIS3 ----------------------------------------------------------------------
#
echo -e "\e[34m[MizaR]\e[32m Installing JARVIS3 ...\e[0m";
rm -fr jarvis3-3.3/ v3.3.zip extra
wget https://github.com/cobilab/jarvis3/archive/refs/tags/v3.3.zip
unzip v3.3.zip
cd jarvis3-3.3/src/
make
cp -rf extra/ ../../
cp -f JARVIS3.sh ../../
cp -f JARVIS3 ../../
cd ../../
./JARVIS3.sh --install
#
# NAF --------------------------------------------------------------------------
#
echo -e "\e[34m[MizaR]\e[32m Installing NAF ...\e[0m";
rm -fr naf/
git clone --recurse-submodules https://github.com/KirillKryukov/naf.git
cd naf && make
cp ennaf/ennaf ENNAF # to avoid dir name collision
cd ../
rm -fr ennaf
mv ENNAF ennaf
#
# MBGC -------------------------------------------------------------------------
#
echo -e "\e[34m[MizaR]\e[32m Installing MBGC ...\e[0m";
git clone https://github.com/kowallus/mbgc.git
cd mbgc
mkdir build
cd build
cmake ..
make mbgc
cp mbgc ../../MBGC
cd ../../
rm -fr mbgc
mv MBGC mbgc
#
# AGC --------------------------------------------------------------------------
#
echo -e "\e[34m[MizaR]\e[32m Installing AGC ...\e[0m";
git clone https://github.com/refresh-bio/agc
cd agc && make
cp agc ../AGC
cd ../
rm -fr agc/
mv AGC agc
#
# GeCo3 ------------------------------------------------------------------------
#
echo -e "\e[34m[MizaR]\e[32m Installing GECO3 ...\e[0m";
rm -fr geco3/
git clone https://github.com/cobilab/geco3.git
cd geco3/src/
make
cp GeCo3 ../
cp GeDe3 ../
cd ../../
#
# GeCo2 ------------------------------------------------------------------------
#
echo -e "\e[34m[MizaR]\e[32m Installing GECO2 ...\e[0m";
conda install -y -c bioconda geco2
#
# Fqzcomp ----------------------------------------------------------------------
#
echo -e "\e[34m[MizaR]\e[32m Installing fqzcomp ...\e[0m";
rm -fr fqzcomp-master fqzcomp
wget https://github.com/jkbonfield/fqzcomp/archive/refs/heads/master.zip
unzip master.zip
cd fqzcomp-master/
make
cp fqzcomp ../
cd ../
#
# ==============================================================================
#                 G E N E R A L - P U R P O S E   E N C O D E R S
# ------------------------------------------------------------------------------
#
# LZMA -------------------------------------------------------------------------
#
echo -e "\e[34m[MizaR]\e[32m Installing LZMA ...\e[0m";
rm -fr v5.4.4.zip xz-5.4.4/
wget https://github.com/tukaani-project/xz/archive/refs/tags/v5.4.4.zip
unzip v5.4.4.zip
cd xz-5.4.4/
cmake .
make
cp xz ../
cd ..
#
# BSC --------------------------------------------------------------------------
#
echo -e "\e[34m[MizaR]\e[32m Installing BSC ...\e[0m";
rm -fr v0.2.1.tar.gz bsc-m03-0.2.1
wget https://github.com/IlyaGrebnov/bsc-m03/archive/refs/tags/v0.2.1.tar.gz
tar -vxzf v0.2.1.tar.gz
cd bsc-m03-0.2.1/
cmake .
make
cp bsc-m03 ..
cd ..
#
# NNCP -------------------------------------------------------------------------
#
echo -e "\e[34m[MizaR]\e[32m Installing NNCP ...\e[0m";
rm -fr nncp-2021-06-01.tar.gz nncp-2021-06-01/
wget https://bellard.org/nncp/nncp-2021-06-01.tar.gz
tar -vxzf nncp-2021-06-01.tar.gz
cd nncp-2021-06-01/
make
cp nncp ../
cd ..
#
# PAQ8L ------------------------------------------------------------------------
#
echo -e "\e[34m[MizaR]\e[32m Installing PAQ8L ...\e[0m";
mkdir -p tmp_paq8l
cd tmp_paq8l/
rm -fr paq8l.zip
wget http://mattmahoney.net/dc/paq8l.zip
unzip paq8l.zip
g++ paq8l.cpp -O2 -DUNIX -DNOASM -s -o paq8l
cp paq8l ../
cd ../
rm -fr tmp_paq8l/
#
# CMIX -------------------------------------------------------------------------
#
echo -e "\e[34m[MizaR]\e[32m Installing CMIX ...\e[0m";
rm -fr cmix/
git clone https://github.com/byronknoll/cmix.git
cd cmix
make
cd ../ #TODO: where is binary to copy?
#
# ------------------------------------------------------------------------------
#
# ==============================================================================
#                          C H E C K   P R O G R A M S
# ------------------------------------------------------------------------------
#
PROGRAM_EXISTS "./gto";
PROGRAM_EXISTS "./MFCompressC";
PROGRAM_EXISTS "./MFCompressD";
PROGRAM_EXISTS "./JARVIS2";
PROGRAM_EXISTS "./JARVIS2.sh";
PROGRAM_EXISTS "./JARVIS3";
PROGRAM_EXISTS "./JARVIS3.sh";
PROGRAM_EXISTS "./ennaf";
PROGRAM_EXISTS "./mbgc";
PROGRAM_EXISTS "./agc";
PROGRAM_EXISTS "./GeCo3";
PROGRAM_EXISTS "./GeDe3";
PROGRAM_EXISTS "./GeCo2";
PROGRAM_EXISTS "./GeDe2";
PROGRAM_EXISTS "./fqzcomp";
PROGRAM_EXISTS "./xz";
PROGRAM_EXISTS "./bsc-m03";
PROGRAM_EXISTS "./nncp";
PROGRAM_EXISTS "./paq8l";
PROGRAM_EXISTS "./cmix";
#
# ==============================================================================
