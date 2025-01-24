#mpic++ -O -march=native -larmpl_mp -fopenmp BSM.cxx -o BSM
mpic++ -g1  -O3 -fno-inline -fno-optimize-sibling-calls -larmpl_mp -march=native -fopenmp BSM.cxx -o BSMwithopt 
