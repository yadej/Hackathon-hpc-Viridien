mpic++ -O -march=native -larmpl -I /tools/openblas/acfl/24.04/include -L /tools/openblas/acfl/24.04/lib/libopenblas.a BSM.cxx -o BSM
mpic++ -O3 -larmpl -march=native -I /tools/openblas/acfl/24.04/include -L /tools/openblas/acfl/24.04/lib/libopenblas.a BSM.cxx -o BSMwithopt
