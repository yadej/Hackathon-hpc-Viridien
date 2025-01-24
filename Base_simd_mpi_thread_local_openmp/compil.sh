mpic++ -O -march=native -larmpl_mp -fopenmp BSM.cxx -o BSM
mpic++ -O3 -larmpl_mp -march=native -fopenmp BSM.cxx -o BSMwithopt
