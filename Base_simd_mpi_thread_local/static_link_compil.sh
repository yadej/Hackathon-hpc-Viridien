mpic++ -O3 -march=native -I/tools/openblas/acfl/24.04/include \
-L/tools/acfl/24.04/armpl-24.04.0_AmazonLinux-2_arm-linux-compiler/lib \
/tools/openblas/acfl/24.04/lib/libopenblas.a \
-larmpl BSM.cxx -o BSMwithopt
